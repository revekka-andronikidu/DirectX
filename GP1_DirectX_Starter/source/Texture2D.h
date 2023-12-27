#pragma once
namespace dae
{
	class Texture2D final
	{
	public:
		Texture2D(ID3D11Device* pDevice , const std::string& path);
		~Texture2D();

		Texture2D(const Texture2D& other) = delete;
		Texture2D& operator=(const Texture2D& other) = delete;
		Texture2D(Texture2D&& other) = delete;
		Texture2D& operator=(Texture2D&& other) = delete;

		ID3D11ShaderResourceView* GetShaderResourceView() const { return m_pShaderResourceView; };

	private:
		ID3D11ShaderResourceView* m_pShaderResourceView{};
		ID3D11Texture2D* m_pResource{};
	};

};