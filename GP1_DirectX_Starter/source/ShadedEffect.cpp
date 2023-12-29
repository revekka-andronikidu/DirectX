#include "pch.h"
#include "ShadedEffect.h"
#include "Texture2D.h"

namespace dae {

	ShadedEffect::ShadedEffect(ID3D11Device* pDevice, const std::wstring& assetFile)
		:Effect(pDevice, assetFile)
	{
		m_pNormalMapVariable = m_pEffect->GetVariableByName("gNormalMap")->AsShaderResource();
		if (!m_pNormalMapVariable->IsValid())
		{
			std::wcout << L"m_pNormalMapVariable not valid!\n";
		}

		m_pSpecularMapVariable = m_pEffect->GetVariableByName("gSpecularMap")->AsShaderResource();
		if (!m_pSpecularMapVariable->IsValid())
		{
			std::wcout << L"m_pSpecularMapVariable not valid!\n";
		}

		m_pGlossinessMapVariable = m_pEffect->GetVariableByName("gGlossinessMap")->AsShaderResource();
		if (!m_pGlossinessMapVariable->IsValid())
		{
			std::wcout << L"m_pGlossinessMapVariable not valid!\n";
		}

		m_pWorldVariable = m_pEffect->GetVariableByName("gWorldMatrix")->AsMatrix();
		if (!m_pWorldVariable->IsValid())
		{
			std::wcout << L"m_pWorldVariable not valid!\n";
		}

		m_pViewInverseVariable = m_pEffect->GetVariableByName("gViewInverseMatrix")->AsMatrix();
		if (!m_pViewInverseVariable->IsValid())
		{
			std::wcout << L"m_pViewInverseVariable not valid!\n";
		}
	}

	ShadedEffect::~ShadedEffect()
	{
		SAFE_RELEASE(m_pNormalMapVariable);
		SAFE_RELEASE(m_pSpecularMapVariable);
		SAFE_RELEASE(m_pGlossinessMapVariable);

		SAFE_RELEASE(m_pViewInverseVariable);
		SAFE_RELEASE(m_pWorldVariable);
	}

	void ShadedEffect::SetNormalMap(Texture2D* pNormalTexture)
	{
		if (m_pNormalMapVariable)
		{
			m_pNormalMapVariable->SetResource(pNormalTexture->GetShaderResourceView());
		}
	}

	void ShadedEffect::SetSpecularMap(Texture2D* pSpecularTexture)
	{
		if (m_pSpecularMapVariable)
		{
			m_pSpecularMapVariable->SetResource(pSpecularTexture->GetShaderResourceView());
		}
	}

	void ShadedEffect::SetGlossinessMap(Texture2D* pGlossinessTexture)
	{
		if (m_pGlossinessMapVariable)
		{
			m_pGlossinessMapVariable->SetResource(pGlossinessTexture->GetShaderResourceView());
		}
	}

	void ShadedEffect::SetWorldMatrix(const Matrix& matrix)
	{
		m_pWorldVariable->SetMatrix(reinterpret_cast<const float*>(&matrix));
	}

	void ShadedEffect::SetInverseViewMatrix(const Matrix& matrix)
	{
		m_pWorldVariable->SetMatrix(reinterpret_cast<const float*>(&matrix));
	}
};