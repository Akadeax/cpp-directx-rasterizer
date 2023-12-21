#include "pch.h"
#include "Effect.h"

#include "Texture.h"

Effect::Effect(ID3D11Device* pDevice, const std::wstring& assetFile, const Texture* pDiffuse, const Texture* pNormal, const Texture* pGlossiness)
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
		std::wcout << L"var not valid";
	}

	m_pEffectWorldVar = m_pEffect->GetVariableBySemantic("World")->AsMatrix();
	if (!m_pEffectWorldVar->IsValid())
	{
		std::wcout << L"var not valid";
	}

	m_pCameraPosVar = m_pEffect->GetVariableBySemantic("CameraPos")->AsVector();
	if (!m_pCameraPosVar->IsValid())
	{
		std::wcout << L"var not valid";
	}

	m_pDiffuseMapVar = m_pEffect->GetVariableBySemantic("DiffuseMap")->AsShaderResource();
	if (!m_pDiffuseMapVar->IsValid())
	{
		std::wcout << L"var not valid";
	}
	m_pDiffuseMapVar->SetResource(pDiffuse->GetShaderResourceView());

	m_pNormalMapVar = m_pEffect->GetVariableBySemantic("NormalMap")->AsShaderResource();
	if (!m_pNormalMapVar->IsValid())
	{
		std::wcout << L"var not valid";
	}
	m_pNormalMapVar->SetResource(pNormal->GetShaderResourceView());

	m_pGlossinessMapVar = m_pEffect->GetVariableBySemantic("GlossinessMap")->AsShaderResource();
	if (!m_pGlossinessMapVar->IsValid())
	{
		std::wcout << L"var not valid";
	}
	m_pGlossinessMapVar->SetResource(pGlossiness->GetShaderResourceView());

}

Effect::~Effect()
{
	m_pEffect->Release();
}

void Effect::SetGPUData(const dae::Matrix& wvp, const dae::Matrix& world, const dae::Vector3& cameraPos) const
{
	m_pEffectWorldViewProjectionVar->SetMatrix(reinterpret_cast<const float*>(&wvp));
	m_pEffectWorldVar->SetMatrix(reinterpret_cast<const float*>(&world));
	m_pCameraPosVar->SetFloatVector(reinterpret_cast<const float*>(&cameraPos));
}

void Effect::CycleSamplerMode()
{
	if (++m_CurrentTechniqueIndex == m_TechniqueCount)
	{
		m_CurrentTechniqueIndex = 0;
	}
	m_pTechnique = m_pEffect->GetTechniqueByIndex(m_CurrentTechniqueIndex);

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
