#include "pch.h"
#include "Effect.h"

#include "Texture.h"

Effect::Effect(ID3D11Device* pDevice, const std::wstring& assetFile, const Texture* pDiffuse)
	: m_pDevice{ pDevice }
{
	m_pEffect = LoadEffect(assetFile);

	m_pTechnique = m_pEffect->GetTechniqueByIndex(0);
	if (!m_pTechnique->IsValid())
	{
		std::wcout << L"Technique not valid";
	}

	m_pEffectWorldViewProjectionVar = m_pEffect->GetVariableBySemantic("WorldViewProjection")->AsMatrix();
	if (!m_pEffectWorldViewProjectionVar->IsValid())
	{
		std::wcout << L"effect world view projection matrix not valid";
	}

	m_pDiffuseMapVar = m_pEffect->GetVariableBySemantic("DiffuseMap")->AsShaderResource();
	if (!m_pDiffuseMapVar->IsValid())
	{
		std::wcout << L"effect diffuse map not valid";
	}

	m_pDiffuseMapVar->SetResource(pDiffuse->GetShaderResourceView());
}

Effect::~Effect()
{
	m_pEffect->Release();
}

void Effect::SetMatrix(dae::Matrix& matrix) const
{
	m_pEffectWorldViewProjectionVar->SetMatrix(reinterpret_cast<float*>(&matrix));
}

void Effect::CycleSamplerMode()
{
	switch(m_CurrentSampler)
	{
	case SamplerMode::POINT:
		m_CurrentSampler = SamplerMode::LINEAR;
		m_pTechnique = m_pEffect->GetTechniqueByName("LinearTechnique");
		break;
	case SamplerMode::LINEAR:
		m_CurrentSampler = SamplerMode::ANISOTROPIC;
		m_pTechnique = m_pEffect->GetTechniqueByName("AnisotropicTechnique");
		break;
	case SamplerMode::ANISOTROPIC:
		m_CurrentSampler = SamplerMode::POINT;
		m_pTechnique = m_pEffect->GetTechniqueByName("PointTechnique");
		break;
	}
}

ID3DX11Effect* Effect::LoadEffect(const std::wstring& assetFile) const
{
	HRESULT result;
	ID3D10Blob* pErrorBlob{};
	ID3DX11Effect* pEffect;

	DWORD shaderFlags{ 0 };

#if defined(DEBUG) || defined(_DEBUG)
	shaderFlags |= D3DCOMPILE_DEBUG;
	shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	result = D3DX11CompileEffectFromFile(
		assetFile.c_str(),
		nullptr,
		nullptr,
		shaderFlags,
		0,
		m_pDevice,
		&pEffect,
		&pErrorBlob
	);

	if (FAILED(result))
	{
		if (pErrorBlob != nullptr)
		{
			const char* pErrors{ static_cast<char*>(pErrorBlob->GetBufferPointer()) };
			std::wstringstream ss;
			for (uint32_t i{ 0 }; i < pErrorBlob->GetBufferSize(); ++i)
			{
				ss << pErrors[i];
			}

			OutputDebugStringW(ss.str().c_str());
			pErrorBlob->Release();
			pErrorBlob = nullptr;

			std::wcout << ss.str() << std::endl;

			throw EffectLoadFailedException{};
		}
		else
		{
			std::wstringstream ss;
			ss << "EffectLoader: Failed to CreateEffectFromFile!\nPath: " << assetFile;
			std::wcout << ss.str() << std::endl;
			return nullptr;
		}
	}

	return pEffect;
}
