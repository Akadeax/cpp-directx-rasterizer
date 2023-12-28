#include "pch.h"
#include "Mesh.h"

Mesh::Mesh(
	ID3D11Device* pDevice,
	const std::wstring& effectFile,
	const std::vector<Vertex_PosCol>& vertices,
	const std::vector<uint32_t>& indices,
	const Texture* pDiffuse,
	const Texture* pSpecular,
	const Texture* pNormal,
	const Texture* pGlossiness
)
	: m_pEffect{ new Effect(pDevice, effectFile, pDiffuse, pSpecular, pNormal, pGlossiness) }
{
	// Create vertex layout
	constexpr uint32_t numElements{ 4 };
	D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};

	vertexDesc[0].SemanticName = "POSITION";
	vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[0].AlignedByteOffset = 0;
	vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[1].SemanticName = "TEXCOORD";
	vertexDesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	vertexDesc[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[2].SemanticName = "NORMAL";
	vertexDesc[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	vertexDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[3].SemanticName = "TANGENT";
	vertexDesc[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	vertexDesc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	// Create input layout
	D3DX11_PASS_DESC passDesc{};
	m_pEffect->GetTechnique()->GetPassByIndex(0)->GetDesc(&passDesc);

	HRESULT result{ pDevice->CreateInputLayout(
		vertexDesc,
		numElements,
		passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize,
		&m_pInputLayout
	) };

	if (FAILED(result)) return;

	// Create vertex buffer
	D3D11_BUFFER_DESC vertexBufferDesc{};
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.ByteWidth = sizeof(Vertex_PosCol) * static_cast<uint32_t>(vertices.size());
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexInitData{};
	vertexInitData.pSysMem = vertices.data();

	result = pDevice->CreateBuffer(&vertexBufferDesc, &vertexInitData, &m_pVertexBuffer);

	if (FAILED(result)) return;

	// Create index buffer
	m_NumIndices = static_cast<uint32_t>(indices.size());

	D3D11_BUFFER_DESC indexBufferDesc{};
	indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDesc.ByteWidth = sizeof(uint32_t) * m_NumIndices;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA indexInitData{};
	indexInitData.pSysMem = indices.data();

	result = pDevice->CreateBuffer(&indexBufferDesc, &indexInitData, &m_pIndexBuffer);

	if (FAILED(result)) return;
}

Mesh::~Mesh()
{
	m_pIndexBuffer->Release();
	m_pVertexBuffer->Release();
	m_pInputLayout->Release();
	delete m_pEffect;
}

void Mesh::Render(ID3D11DeviceContext* pDeviceContext, const dae::Matrix& worldViewProjection, const dae::Vector3& cameraPos) const
{
	// Set primitive topology
	pDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Set input layout
	pDeviceContext->IASetInputLayout(m_pInputLayout);

	// Update vertex buffer on GPU
	constexpr uint32_t stride{ sizeof(Vertex_PosCol) };
	constexpr uint32_t offset{ 0 };
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	m_pEffect->SetGPUData(worldViewProjection, m_WorldMatrix, cameraPos);

	// Set index buffer
	pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Draw
	D3DX11_TECHNIQUE_DESC techniqueDesc{};
	m_pEffect->GetTechnique()->GetDesc(&techniqueDesc);
	
	for (uint32_t p{ 0 }; p < techniqueDesc.Passes; ++p)
	{
		m_pEffect->GetTechnique()->GetPassByIndex(p)->Apply(0, pDeviceContext);
		pDeviceContext->DrawIndexed(m_NumIndices, 0, 0);
	}
}

void Mesh::CycleSamplerState() const
{
	m_pEffect->CycleSamplerMode();
}
