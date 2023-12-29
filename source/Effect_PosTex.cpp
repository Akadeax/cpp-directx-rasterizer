#include "pch.h"
#include "Effect_PosTex.h"

#include <cassert>

#include "Texture.h"

Effect_PosTex::Effect_PosTex(
	ID3D11Device* pDevice,
	Texture* pDiffuse,
	Texture* pSpecular,
	Texture* pNormal,
	Texture* pGlossiness
)
	: Effect(pDevice, L"Resources/PosTex.fx", pDiffuse)
	, m_pSpecular{ pSpecular }
	, m_pNormal{ pNormal }
	, m_pGlossiness{ pGlossiness }
{
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

	m_pSpecularMapVar = m_pEffect->GetVariableBySemantic("SpecularMap")->AsShaderResource();
	if (!m_pSpecularMapVar->IsValid())
	{
		std::wcout << L"var not valid";
	}
	m_pSpecularMapVar->SetResource(pSpecular->GetShaderResourceView());

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

	m_pDoNormalMapVar = m_pEffect->GetVariableBySemantic("DoNormalMap")->AsScalar();
	if (!m_pDoNormalMapVar->IsValid())
	{
		std::wcout << L"var not valid";
	}
	m_pDoNormalMapVar->SetBool(m_DoNormalMap);
}

Effect_PosTex::~Effect_PosTex()
{
	delete m_pSpecular;
	delete m_pNormal;
	delete m_pGlossiness;
}

void Effect_PosTex::SetCameraPos(const dae::Vector3& pos) const
{
	const HRESULT res{ m_pCameraPosVar->SetFloatVector(reinterpret_cast<const float*>(&pos)) };
	assert(res == S_OK);

}

void Effect_PosTex::SetDoNormalMapping(bool val) const
{
	const HRESULT res{ m_pDoNormalMapVar->SetBool(val) };
	assert(res == S_OK);
}

void Effect_PosTex::CycleNormalMapping()
{
	m_DoNormalMap = !m_DoNormalMap;
	SetDoNormalMapping(m_DoNormalMap);
}
