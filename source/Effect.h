#pragma once
class Texture;


class Effect final
{
public:
	class EffectLoadFailedException {};

	explicit Effect(ID3D11Device* pDevice, const std::wstring& assetFile, const Texture* pDiffuse, const Texture* pNormal, const Texture* pGlossiness);
	~Effect();

	Effect(const Effect&) = delete;
	Effect(Effect&&) noexcept = delete;
	Effect& operator=(const Effect&) = delete;
	Effect& operator=(Effect&&) noexcept = delete;

	auto GetEffect() const { return m_pEffect; }
	auto GetTechnique() const { return m_pTechnique; }

	void SetGPUData(const dae::Matrix& wvp, const dae::Matrix& world, const dae::Vector3& cameraPos) const;

	void CycleSamplerMode();

private:
	ID3DX11Effect* LoadEffect(const std::wstring& assetFile) const;

	ID3D11Device* m_pDevice;

	ID3DX11Effect* m_pEffect{};
	ID3DX11EffectTechnique* m_pTechnique{};

	ID3DX11EffectMatrixVariable* m_pEffectWorldViewProjectionVar{};
	ID3DX11EffectMatrixVariable* m_pEffectWorldVar{};
	ID3DX11EffectVectorVariable* m_pCameraPosVar{};

	ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVar{};
	ID3DX11EffectShaderResourceVariable* m_pNormalMapVar{};
	ID3DX11EffectShaderResourceVariable* m_pGlossinessMapVar{};

	const int m_TechniqueCount{ 4 };
	int m_CurrentTechniqueIndex{ 0 };
};
