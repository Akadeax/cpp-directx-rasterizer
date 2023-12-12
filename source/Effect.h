#pragma once
class Effect final
{
public:
	explicit Effect(ID3D11Device* pDevice, const std::wstring& assetFile);
	~Effect();

	Effect(const Effect&) = delete;
	Effect(Effect&&) noexcept = delete;
	Effect& operator=(const Effect&) = delete;
	Effect& operator=(Effect&&) noexcept = delete;

	auto GetEffect() const { return m_pEffect; }
	auto GetTechnique() const { return m_pTechnique; }

	void SetMatrix(dae::Matrix& matrix);

private:
	ID3DX11Effect* LoadEffect(const std::wstring& assetFile);

	ID3D11Device* m_pDevice;

	ID3DX11Effect* m_pEffect{};
	ID3DX11EffectTechnique* m_pTechnique{};

	ID3DX11EffectMatrixVariable* m_pEffectWorldViewProjectionVar{};
};
