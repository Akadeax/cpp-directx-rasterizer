#pragma once
#include "Effect.h"
#include "Vertex.h"

class Mesh final
{
public:
	explicit Mesh(
		ID3D11Device* pDevice,
		const std::wstring& effectFile,
		const std::vector<Vertex_PosCol>& vertices,
		const std::vector<uint32_t>& indices,
		const Texture* pDiffuse
	);

	~Mesh();

	Mesh(const Mesh&) = delete;
	Mesh(Mesh&&) noexcept = delete;
	Mesh& operator=(const Mesh&) = delete;
	Mesh& operator=(Mesh&&) noexcept = delete;

	void Render(ID3D11DeviceContext* pDeviceContext, dae::Matrix& worldViewProjection) const;

	auto GetWorldMatrix() const { return m_WorldMatrix; }

private:
	Effect* m_pEffect;

	dae::Matrix m_WorldMatrix{};

	ID3D11InputLayout* m_pInputLayout{};
	ID3D11Buffer* m_pVertexBuffer{};
	ID3D11Buffer* m_pIndexBuffer{};

	uint32_t m_NumIndices{};
};

