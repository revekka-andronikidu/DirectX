#pragma once
#include "Math.h"


namespace dae
{
	class Effect;
	class Texture2D;

	class Mesh final
	{
	public:
		Mesh(ID3D11Device* pDevice, std::vector<Vertex_PosCol> vertices, std::vector<uint32_t> indices, Effect* pEffect);
		Mesh(ID3D11Device* pDevice, const std::string& objFilePath, Effect* pEffect);
		Mesh(const Mesh& other) = delete;
		Mesh& operator=(const Mesh& other) = delete;
		Mesh(Mesh&& other) = delete;
		Mesh& operator=(Mesh&& other) = delete;
		~Mesh();


		void RotateX(float pitch);
		void RotateY(float yaw);
		void RotateZ(float roll);
		void Render(ID3D11DeviceContext* pDeviceContext) const;
		void UpdateViewMatrices(const Matrix& viewProjectionMatrix, const Matrix& inverseViewMatrix);
		void CycleFilteringMethods();

	private:
		ID3D11Device* m_pDevice{};
		Effect* m_pEffect{};
		ID3DX11EffectTechnique* m_pTechnique{};
		ID3D11InputLayout* m_pInputLayout{};
		ID3D11Buffer* m_pVertexBuffer{};
		ID3D11Buffer* m_pIndexBuffer{};

		uint32_t m_NumIndices{};

		Matrix m_TranslationMatrix{ Vector3::UnitX, Vector3::UnitY, Vector3::UnitZ, Vector3::Zero };
		Matrix m_RotationMatrix{ Vector3::UnitX, Vector3::UnitY, Vector3::UnitZ, Vector3::Zero };
		Matrix m_ScaleMatrix{ Vector3::UnitX, Vector3::UnitY, Vector3::UnitZ, Vector3::Zero };
	};
};

