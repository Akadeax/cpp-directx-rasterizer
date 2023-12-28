#pragma once
#include "Effect.h"
#include "Vertex.h"

class Mesh final
{
public:
	explicit Mesh() = default;
	explicit Mesh(
		ID3D11Device* pDevice,
		const std::wstring& effectFile,
		const std::vector<Vertex_PosCol>& vertices,
		const std::vector<uint32_t>& indices,
		const Texture* pDiffuse,
		const Texture* pSpecular,
		const Texture* pNormal,
		const Texture* pGlossiness
	);

	~Mesh();

	Mesh(const Mesh&) = delete;
	Mesh(Mesh&&) noexcept = delete;
	Mesh& operator=(const Mesh&) = delete;
	Mesh& operator=(Mesh&&) noexcept = delete;

	void Render(ID3D11DeviceContext* pDeviceContext, const dae::Matrix& worldViewProjection, const dae::Vector3& cameraPos) const;

	void SetWorldMatrix(const dae::Matrix& matrix) { m_WorldMatrix = matrix; }
	auto GetWorldMatrix() const { return m_WorldMatrix; }

	void CycleSamplerState() const;

private:
	Effect* m_pEffect;

	dae::Matrix m_WorldMatrix{};

	ID3D11InputLayout* m_pInputLayout{};
	ID3D11Buffer* m_pVertexBuffer{};
	ID3D11Buffer* m_pIndexBuffer{};

	uint32_t m_NumIndices{};



};

