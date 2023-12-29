#pragma once
#include "Camera.h"

class Effect_DiffusePartialCoverage;
class Effect_PosTex;
class Mesh;
struct SDL_Window;
struct SDL_Surface;

namespace dae
{
	class Renderer final
	{
	public:
		explicit Renderer(SDL_Window* pWindow);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void Update(const Timer* pTimer);
		void Render() const;

		std::string CycleFilterMode() const;
		void CycleDoRotation();
		void CycleNormalMapping();
		void CycleRenderFireFX();

		Camera& GetCamera() { return m_Camera; }

	private:
		SDL_Window* m_pWindow{};

		int m_Width{};
		int m_Height{};

		bool m_IsInitialized{ false };

		Camera m_Camera;

		Effect_PosTex* m_pMeshEffect{};
		Mesh* m_pMesh{};

		Effect_DiffusePartialCoverage* m_pFXEffect{};
		Mesh* m_pFXMesh{};


		bool m_DoRotation{ true };
		bool m_RenderFireFX{ true };

		float m_CurrentMeshRotation{ 0.f };

		// DX
		ID3D11Device* m_pDevice{};
		ID3D11DeviceContext* m_pDeviceContext{};
		IDXGISwapChain* m_pSwapChain{};

		ID3D11Texture2D* m_pDepthStencilBuffer{};
		ID3D11DepthStencilView* m_pDepthStencilView{};

		ID3D11Resource* m_pRenderTargetBuffer{};
		ID3D11RenderTargetView* m_pRenderTargetView{};

		HRESULT InitializeDirectX();
	};
}
