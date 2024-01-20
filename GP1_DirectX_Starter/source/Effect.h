#pragma once

namespace dae 
{
	struct Matrix;
	class Texture2D;

	class Effect
	{
	public:
		Effect(ID3D11Device* pDevice, const std::wstring& assetFile);
		virtual ~Effect();

		Effect(const Effect& other) = delete;
		Effect& operator=(const Effect& other) = delete;
		Effect(Effect&& other) = delete;
		Effect& operator=(Effect&& other) = delete;

		ID3DX11Effect* GetEffect() const;
		ID3DX11EffectTechnique* GetTechnique() const;
		
		void SetWorldViewProjectionMatrix(const dae::Matrix& matrix);
		void SetDiffuseMap(Texture2D* pDiffuseTexture);
		void CycleFilteringMethods();

		virtual void SetWorldMatrix(const Matrix& matrix) {};
		virtual void SetInverseViewMatrix(const Matrix& matrix){};
		virtual void SetUseNormalMap(bool useNormalMap) {};

	protected:
		enum class FilteringMethod
		{
			Point,
			Linear,
			Anisotropic,
			END
		};

		FilteringMethod m_FilteringMethod{ FilteringMethod::Point };

		ID3DX11Effect* m_pEffect{};
		ID3DX11EffectTechnique* m_pTechnique{};

		ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable{};
		
		
		ID3DX11EffectMatrixVariable* m_pWorldViewProjectionVariable{};

		static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile);

		ID3DX11EffectScalarVariable* m_pUseNormalMap;
	};
};

