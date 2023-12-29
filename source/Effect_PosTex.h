#pragma once
#include "Effect.h"

class Effect_PosTex final : public Effect
{
public:
	explicit Effect_PosTex(
		ID3D11Device* pDevice,
		Texture* pDiffuse,
		Texture* pSpecular,
		Texture* pNormal,
		Texture* pGlossiness
	);
	~Effect_PosTex() override;

	Effect_PosTex(const Effect_PosTex&) = delete;
	Effect_PosTex(Effect_PosTex&&) noexcept = delete;
	Effect_PosTex& operator=(const Effect_PosTex&) = delete;
	Effect_PosTex& operator=(Effect_PosTex&&) noexcept = delete;

	void SetCameraPos(const dae::Vector3& pos) const;
	void SetDoNormalMapping(bool val) const;

	void CycleNormalMapping();

private:
	ID3DX11EffectVectorVariable* m_pCameraPosVar{};

	Texture* m_pSpecular;
	ID3DX11EffectShaderResourceVariable* m_pSpecularMapVar{};
	Texture* m_pNormal;
	ID3DX11EffectShaderResourceVariable* m_pNormalMapVar{};
	Texture* m_pGlossiness;
	ID3DX11EffectShaderResourceVariable* m_pGlossinessMapVar{};

	bool m_DoNormalMap{ true };
	ID3DX11EffectScalarVariable* m_pDoNormalMapVar{};
};

