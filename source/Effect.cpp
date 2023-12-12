#include "pch.h"
#include "Effect.h"

Effect::Effect(ID3D11Device* pDevice, const std::wstring& assetFile)
	: m_pDevice{ pDevice }
{
	m_pEffect = LoadEffect(assetFile);

	m_pTechnique = m_pEffect->GetTechniqueByIndex(0);
	if (!m_pTechnique->IsValid())
	{
		std::wcout << L"Technique not valid";
	}

	m_pEffectWorldViewProjectionVar = m_pEffect->GetVariableByName("gWorldViewProjection")->AsMatrix();
	if (!m_pEffectWorldViewProjectionVar->IsValid())
	{
		std::wcout << L"effect world view projection matrix not valid";
	}

	//m_pEffectWorldViewProjectionVar->SetMatrix()
}

Effect::~Effect()
{
	m_pEffect->Release();
}

void Effect::SetMatrix(dae::Matrix& matrix)
{
	m_pEffectWorldViewProjectionVar->SetMatrix(reinterpret_cast<float*>(&matrix));
}

ID3DX11Effect* Effect::LoadEffect(const std::wstring& assetFile)
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
