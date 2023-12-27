#include "pch.h"
#include "Mesh.h"
#include "Effect.h"
#include <assert.h>
#include "Math.h"
#include "Texture2D.h"

namespace dae 
{

	//Mesh::Mesh(ID3D11Device* pDevice, const std::string& objFilePath, Effect* pEffect):
	Mesh::Mesh(ID3D11Device* pDevice, std::vector<Vertex_PosCol> vertices, std::vector<uint32_t> indices, Effect* pEffect)
		: m_pDevice{pDevice }	
		, m_pEffect{std::move(pEffect)}
	{
		
		/*std::vector<Vertex_PosCol> vertices;
		std::vector<uint32_t> indices;
		if (!Utils::ParseOBJ(objFilePath, vertices, indices))
		{
			std::cout << "Invalid filepath!\n";
		}*/

		m_pTechnique = m_pEffect->GetTechnique();

		// Create Vertex Layout
		static constexpr uint32_t numElements{ 3 };
		D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};

		vertexDesc[0].SemanticName = "POSITION";
		vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		vertexDesc[0].AlignedByteOffset = 0;
		vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		vertexDesc[1].SemanticName = "COLOR";
		vertexDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		vertexDesc[1].AlignedByteOffset = 12;
		vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		vertexDesc[2].SemanticName = "TEXCOORD";
		vertexDesc[2].Format = DXGI_FORMAT_R32G32_FLOAT;
		vertexDesc[2].AlignedByteOffset = 12;
		vertexDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;


		// Create Input Layout
		D3DX11_PASS_DESC passDesc{};
		m_pTechnique->GetPassByIndex(0)->GetDesc(&passDesc);

		HRESULT result = pDevice->CreateInputLayout
		(
			vertexDesc,
			numElements,
			passDesc.pIAInputSignature,
			passDesc.IAInputSignatureSize,
			&m_pInputLayout
		);
		if (FAILED(result))
			assert(false); //or return

		// Create vertex buffer
		D3D11_BUFFER_DESC bd = {};
		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = sizeof(dae::Vertex_PosCol) * static_cast<uint32_t>(vertices.size());
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA initData{};
		initData.pSysMem = vertices.data();

		result = pDevice->CreateBuffer(&bd, &initData, &m_pVertexBuffer);
		if (FAILED(result)) return;

		// Create index buffer
		m_NumIndices = static_cast<uint32_t>(indices.size());
		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = sizeof(uint32_t) * m_NumIndices;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		initData.pSysMem = indices.data();

		result = pDevice->CreateBuffer(&bd, &initData, &m_pIndexBuffer);
		if (FAILED(result)) return;


		
	}

	Mesh::~Mesh()
	{
		delete m_pEffect;

		

		SAFE_RELEASE(m_pInputLayout);
		SAFE_RELEASE(m_pVertexBuffer);
		SAFE_RELEASE(m_pIndexBuffer);

	}

	void Mesh::Render(ID3D11DeviceContext* pDeviceContext) const
	{
		// 1. Set primitive topology
		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// 2. Set input layout
		pDeviceContext->IASetInputLayout(m_pInputLayout);

		// 3. Set vertex buffer
		constexpr UINT stride{ sizeof(Vertex_PosCol) };
		constexpr UINT offset = 0;

		pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

		// 4. Set index buffer
		pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		// 5. Draw
		D3DX11_TECHNIQUE_DESC techniqueDesc{};
		m_pTechnique->GetDesc(&techniqueDesc);
		for (UINT p = 0; p < techniqueDesc.Passes; ++p)
		{
			m_pTechnique->GetPassByIndex(p)->Apply(0, pDeviceContext);
			pDeviceContext->DrawIndexed(m_NumIndices, 0, 0);
		}
	}
	void Mesh::RotateX(float pitch)
	{
		m_RotationMatrix = Matrix::CreateRotationX(pitch) * m_RotationMatrix;
	}
	void Mesh::RotateY(float yaw)
	{
		m_RotationMatrix = Matrix::CreateRotationY(yaw) * m_RotationMatrix;
	}
	void Mesh::RotateZ(float roll)
	{
		m_RotationMatrix = Matrix::CreateRotationZ(roll) * m_RotationMatrix;
	}

	void Mesh::UpdateViewMatrices(const Matrix& viewProjectionMatrix, const Matrix& inverseViewMatrix)
	{
		dae::Matrix world{ m_ScaleMatrix * m_RotationMatrix * m_TranslationMatrix };
		m_pEffect->SetWorldViewProjectionMatrix(world * viewProjectionMatrix);
		//m_pEffect->SetInverseViewMatrix(inverseViewMatrix);
		//m_pEffect->SetWorldMatrix(world);
	}
}