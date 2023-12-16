#pragma once
class Texture final
{
	class TextureLoadFailedException {};

public:
	explicit Texture(ID3D11Device* pDevice, const std::string& filePath);
	~Texture();

	Texture(const Texture&) = delete;
	Texture(Texture&&) noexcept = delete;
	Texture& operator=(const Texture&) = delete;
	Texture& operator=(Texture&&) noexcept = delete;

	auto GetResource() const { return m_pResource; }
	auto GetShaderResourceView() const { return m_pShaderResourceView; }

private:
	ID3D11Texture2D* m_pResource{};
	ID3D11ShaderResourceView* m_pShaderResourceView{};
};

