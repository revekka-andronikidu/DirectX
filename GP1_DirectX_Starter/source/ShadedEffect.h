#pragma once
#include "Effect.h"

namespace dae
{
	class ShadedEffect final : public Effect
	{
	public:
		ShadedEffect(ID3D11Device* pDevice, const std::wstring& assetFile);
		virtual ~ShadedEffect();

		ShadedEffect(const ShadedEffect& other) = delete;
		ShadedEffect& operator=(const ShadedEffect& other) = delete;
		ShadedEffect(ShadedEffect&& other) = delete;
		ShadedEffect& operator=(ShadedEffect&& other) = delete;

		void SetNormalMap(Texture2D* pNormalTexture);
		void SetSpecularMap(Texture2D* pSpecularTexture);
		void SetGlossinessMap(Texture2D* pGlossinessTexture);


		virtual void SetWorldMatrix(const Matrix& matrix) override;
		virtual void SetInverseViewMatrix(const Matrix& matrix) override;
		virtual void SetUseNormalMap(bool useNormalMap) override;

	private:
		ID3DX11EffectShaderResourceVariable* m_pNormalMapVariable{};
		ID3DX11EffectShaderResourceVariable* m_pSpecularMapVariable{};
		ID3DX11EffectShaderResourceVariable* m_pGlossinessMapVariable{};

		ID3DX11EffectMatrixVariable* m_pInverseViewVariable{};
		ID3DX11EffectMatrixVariable* m_pWorldVariable{};
	};
}