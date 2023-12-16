#include "pch.h"
#include "Renderer.h"

#include "FileParsers.h"
#include "Mesh.h"
#include "Texture.h"
#include "Vertex.h"

namespace dae {

	Renderer::Renderer(SDL_Window* pWindow) :
		m_pWindow(pWindow)
	{
		//Initialize
		SDL_GetWindowSize(pWindow, &m_Width, &m_Height);

		//Initialize DirectX pipeline
		const HRESULT result = InitializeDirectX();
		if (result == S_OK)
		{
			m_IsInitialized = true;
			std::cout << "DirectX is initialized and ready!\n";
		}
		else
		{
			std::cout << "DirectX initialization failed!\n";
		}

		if (!m_IsInitialized) return;

		m_Camera.Initialize(
			45.f,
			{ 0.f, 0.f, -50.f },
			static_cast<float>(m_Width) / static_cast<float>(m_Height)
		);

		//const std::vector<Vertex_PosCol> vertices{
		//	{ { -4.f, 4.f, 2.f }, { 0.f, 0.f } },
		//	{ { 4.f, 4.f, 2.f }, { 1.f, 0.f } },
		//	{ { -4.f, -4.f, 2.f }, { 0.f, 1.f } },
		//	{ { 4.f, -4.f, 2.f }, { 1.f, 1.f } },
		//};
		//const std::vector<uint32_t> indices{
		//	0, 3, 2,
		//	0, 1, 3,
		//};

		std::vector<Vertex_PosCol> vertices;
		std::vector<uint32_t> indices;
		fileParsers::ParseOBJ("Resources/vehicle.obj", vertices, indices);

		m_pMesh = new Mesh{
			m_pDevice,
			L"Resources/PosCol3D.fx",
			vertices, indices,
			new Texture(m_pDevice, "Resources/vehicle_diffuse.png")
		};

	}

	Renderer::~Renderer()
	{
		delete m_pMesh;

		// Release all DX resources and views
		m_pRenderTargetView->Release();
		m_pRenderTargetBuffer->Release();
		m_pDepthStencilView->Release();
		m_pDepthStencilBuffer->Release();
		m_pSwapChain->Release();

		if (m_pDeviceContext)
		{
			m_pDeviceContext->ClearState();
			m_pDeviceContext->Flush();
			m_pDeviceContext->Release();
		}

		m_pDevice->Release();
	}

	void Renderer::Update(const Timer* pTimer)
	{
		m_Camera.Update(pTimer);

		m_CurrentMeshRotation += pTimer->GetElapsed() * 90.f * (PI / 180.f);
		m_pMesh->SetWorldMatrix(Matrix::CreateRotationY(m_CurrentMeshRotation));
	}


	void Renderer::Render() const
	{	
		if (!m_IsInitialized) return;

		constexpr float color[4]{ 0.f, 0.f, 0.3f, 1.f };

		// Clear renderViewTarget and depthStencil
		m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, color);
		m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

		// render
		Matrix wvp{ m_pMesh->GetWorldMatrix() * m_Camera.viewMatrix * m_Camera.projectionMatrix };
		m_pMesh->Render(m_pDeviceContext, wvp);

		// present backbuffer
		m_pSwapChain->Present(0, 0);
	}

	void Renderer::CycleFilterMode() const
	{
		m_pMesh->CycleSamplerState();
	}

	HRESULT Renderer::InitializeDirectX()
	{
		// Create device and context
		const D3D_FEATURE_LEVEL featureLevel{ D3D_FEATURE_LEVEL_11_1 };
		uint32_t createDeviceFlags{ 0 };

#if defined(DEBUG) || defined(_DEBUG)
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		HRESULT result{ D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, createDeviceFlags, &featureLevel, 1, D3D11_SDK_VERSION,
		                                        &m_pDevice, nullptr, &m_pDeviceContext) };

		if (FAILED(result))
			return result;


		// Create swapchain
		DXGI_SWAP_CHAIN_DESC swapChainDesc{};
		swapChainDesc.BufferDesc.Width = m_Width;
		swapChainDesc.BufferDesc.Height = m_Height;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 1;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 60;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 1;
		swapChainDesc.Windowed = true;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.Flags = 0;

		// Get HWND
		SDL_SysWMinfo sysWMinfo{};
		SDL_GetVersion(&sysWMinfo.version);
		SDL_GetWindowWMInfo(m_pWindow, &sysWMinfo);
		swapChainDesc.OutputWindow = sysWMinfo.info.win.window;

		// Create DXGI Factory
		IDXGIFactory1* pDXGIFactory{};
		result = CreateDXGIFactory1(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&pDXGIFactory));

		if (FAILED(result))
			return result;

		result = pDXGIFactory->CreateSwapChain(m_pDevice, &swapChainDesc, &m_pSwapChain);

		pDXGIFactory->Release();

		if (FAILED(result))
			return result;


		// Create Depth stencil buffer
		D3D11_TEXTURE2D_DESC depthStencilDesc{};
		depthStencilDesc.Width = m_Width;
		depthStencilDesc.Height = m_Height;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0;
		depthStencilDesc.MiscFlags = 0;

		result = m_pDevice->CreateTexture2D(&depthStencilDesc, nullptr, &m_pDepthStencilBuffer);

		if (FAILED(result))
			return result;

		// Create Depth stencil view
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
		depthStencilViewDesc.Format = depthStencilDesc.Format;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;

		result = m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &depthStencilViewDesc, &m_pDepthStencilView);

		if (FAILED(result))
			return result;


		// Create render target buffer
		result = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&m_pRenderTargetBuffer));

		if (FAILED(result))
			return result;

		// Create render target view
		result = m_pDevice->CreateRenderTargetView(m_pRenderTargetBuffer, nullptr, &m_pRenderTargetView);

		if (FAILED(result))
			return result;

		// bind renderTarget & depthStencil to output merger stage
		m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);


		// Define viewport
		D3D11_VIEWPORT viewport{};
		viewport.Width = static_cast<float>(m_Width);
		viewport.Height = static_cast<float>(m_Height);
		viewport.TopLeftX = 0.f;
		viewport.TopLeftY = 0.f;
		viewport.MinDepth = 0.f;
		viewport.MaxDepth = 1.f;
		m_pDeviceContext->RSSetViewports(1, &viewport);


		return S_OK;
	}
}
