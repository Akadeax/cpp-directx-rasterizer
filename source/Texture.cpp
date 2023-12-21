#include "pch.h"
#include "Texture.h"

Texture::Texture(ID3D11Device* pDevice, const std::string& filePath)
{
	SDL_Surface* pSurface{ IMG_Load(filePath.c_str()) };
	if (pSurface == nullptr)
	{
		throw TextureLoadFailedException{};
	}

	const DXGI_FORMAT format{ DXGI_FORMAT_R8G8B8A8_UNORM };
	D3D11_TEXTURE2D_DESC desc{};
	desc.Width = pSurface->w;
	desc.Height = pSurface->h;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = format;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData{};
	initData.pSysMem = pSurface->pixels;
	initData.SysMemPitch = static_cast<UINT>(pSurface->pitch);
	initData.SysMemSlicePitch = static_cast<UINT>(pSurface->pitch * pSurface->h);

	HRESULT result{ pDevice->CreateTexture2D(&desc, &initData, &m_pResource) };

	SDL_FreeSurface(pSurface);

	if (FAILED(result))
	{
		throw TextureLoadFailedException{};
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc{};
	viewDesc.Format = format;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	viewDesc.Texture2D.MipLevels = 1;

	result = pDevice->CreateShaderResourceView(m_pResource, &viewDesc, &m_pShaderResourceView);

	if (FAILED(result))
	{
		throw TextureLoadFailedException{};
	}

}

Texture::~Texture()
{
	m_pShaderResourceView->Release();
	m_pResource->Release();
}
