#include "pch.h"
#include "Effect.h"
#include "Texture2D.h"

namespace dae
{

	Effect::Effect(ID3D11Device* pDevice, const std::wstring& assetFile) :
		m_pEffect(LoadEffect(pDevice, assetFile))

	{
		m_pTechnique = m_pEffect->GetTechniqueByName("PointFilteringTechnique");
		if (!m_pTechnique->IsValid())
			std::wcout << L"Technique not valid\n";
		//else
			//std::wcout << L"[FILTERINGMETHOD] Point\n";

		m_pWorldViewProjectionVariable = m_pEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
		if (!m_pWorldViewProjectionVariable->IsValid())
			std::wcout << L"m_pMatWorldViewProjVariable not valid!\n";

		m_pDiffuseMapVariable = m_pEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();
		if (!m_pDiffuseMapVariable->IsValid())
		{
			std::wcout << L"m_pDiffuseMapVariable not valid!\n";
		}

		
			

	}

	Effect::~Effect()
	{
		
		//SAFE_RELEASE(m_pTechnique);	
		//SAFE_RELEASE(m_pMatWorldViewProjVariable);
		//SAFE_RELEASE(m_pDiffuseMapVariable);
		SAFE_RELEASE(m_pUseNormalMap);
		SAFE_RELEASE(m_pEffect);

	}

	ID3DX11Effect* Effect::GetEffect() const
	{
		return m_pEffect;
	}
	ID3DX11EffectTechnique* Effect::GetTechnique() const
	{
		return m_pTechnique;
	}

	void Effect::SetWorldViewProjectionMatrix(const dae::Matrix& matrix)
	{
		/*
		The last thing you need to do is update the data every frame using the SetMatrix(...) function of the (c++ side)
		matrix effect variable, same as you update the vertex buffer etc. Using your camera, you can build the
		WorldViewProjection matrix that you then pass to that function.
		Hint, you�ll have to reinterpret the Matrix data...
		*/
		m_pWorldViewProjectionVariable->SetMatrix(reinterpret_cast<const float*>(&matrix));
	}

	void Effect::SetDiffuseMap(Texture2D* pDiffuseTexture)
	{
		if (m_pDiffuseMapVariable)
			m_pDiffuseMapVariable->SetResource(pDiffuseTexture->GetShaderResourceView());
	}

	

	void Effect::CycleFilteringMethods()
	{

		m_FilteringMethod = static_cast<FilteringMethod>((static_cast<int>(m_FilteringMethod) + 1) % (static_cast<int>(FilteringMethod::END)));

		std::cout << "[FILTERINGMETHOD] ";
		switch (m_FilteringMethod)
		{
		case Effect::FilteringMethod::Point:
			m_pTechnique = m_pEffect->GetTechniqueByName("PointFilteringTechnique");
			if (!m_pTechnique->IsValid()) std::wcout << L"PointTechnique not valid\n";
			std::cout << "Point\n";
			break;
		case Effect::FilteringMethod::Linear:
			m_pTechnique = m_pEffect->GetTechniqueByName("LinearFilteringTechnique");
			if (!m_pTechnique->IsValid()) std::wcout << L"LinearTechnique not valid\n";
			std::cout << "Linear\n";
			break;
		case Effect::FilteringMethod::Anisotropic:
			m_pTechnique = m_pEffect->GetTechniqueByName("AnisotropicFilteringTechnique");
			if (!m_pTechnique->IsValid()) std::wcout << L"AnisotropicTechnique not valid\n";
			std::cout << "Anisotropic\n";
			break;
		}
	}


	ID3DX11Effect* Effect::LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile)
	{
		HRESULT result;
		ID3D10Blob* pErrorBlob{ nullptr };
		ID3DX11Effect* pEffect;

		DWORD shaderFlags{ 0 };

#if defined(DEBUG) || defined(_DEBUG)
		shaderFlags |= D3DCOMPILE_DEBUG;
		shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		result = D3DX11CompileEffectFromFile
		(
			assetFile.c_str(),
			nullptr,
			nullptr,
			shaderFlags,
			0,
			pDevice,
			&pEffect,
			&pErrorBlob
		);

		if (FAILED(result))
		{
			if (pErrorBlob != nullptr)
			{
				const char* pErrors{ static_cast<char*>(pErrorBlob->GetBufferPointer()) };

				std::wstringstream ss;
				for (unsigned int i{}; i < pErrorBlob->GetBufferSize(); ++i)
				{
					ss << pErrors[i];
				}

				OutputDebugStringW(ss.str().c_str());
				pErrorBlob->Release();
				pErrorBlob = nullptr;

				std::wcout << ss.str() << "\n";
			}
			else
			{
				std::wstringstream ss;
				ss << "EffectLoader: Failed to CreateEffectFromFile!\nPath: " << assetFile;
				std::wcout << ss.str() << "\n";
				return nullptr;
			}
		}

		return pEffect;
	}
};