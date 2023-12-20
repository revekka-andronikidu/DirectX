#pragma once

class Effect;

class Mesh final
{
public:
	Mesh(ID3D11Device* pDevice, std::vector<dae::Vertex_PosCol> vertices, std::vector<uint32_t> indices);
	//Mesh(ID3D11Device* pDevice, const std::string& objFilePath, Effect* pEffect);
	Mesh(const Mesh& other) = delete;
	Mesh& operator=(const Mesh& other) = delete;
	Mesh(Mesh&& other) = delete;
	Mesh& operator=(Mesh&& other) = delete;
	~Mesh();

	void Render(ID3D11DeviceContext* pDeviceContext) const;

private:
	ID3D11Device* m_pDevice{};
	Effect* m_pEffect{};
	ID3DX11EffectTechnique* m_pTechnique{};
	ID3D11InputLayout* m_pInputLayout{};
	ID3D11Buffer* m_pVertexBuffer{};
	ID3D11Buffer* m_pIndexBuffer{};

	uint32_t m_NumIndices{};
};

