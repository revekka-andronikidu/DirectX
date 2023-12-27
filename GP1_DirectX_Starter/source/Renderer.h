#pragma once
#include "Camera.h"

struct SDL_Window;
struct SDL_Surface;

namespace dae
{
	class Mesh;
	class Effect;
	class Texture2D;

	class Renderer final
	{
	public:
		Renderer(SDL_Window* pWindow);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void Update(const Timer* pTimer);
		void Render() const;

	private:
		SDL_Window* m_pWindow{};

		int m_Width{};
		int m_Height{};

		bool m_IsInitialized{ false };

		//DIRECTX
		HRESULT InitializeDirectX();
		void DeleteResourceLeaks();

		ID3D11Device* m_pDevice;
		ID3D11DeviceContext* m_pDeviceContext;
		IDXGISwapChain* m_pSwapChain;
		ID3D11Texture2D* m_pDepthStencilBuffer;
		ID3D11DepthStencilView* m_pDepthStencilView;
		ID3D11Resource* m_pRenderTargetBuffer;
		ID3D11RenderTargetView* m_RenderTargetView;

		Camera m_Camera;
		Mesh* m_pMesh{};
		Texture2D* m_pTexture;
		Effect* m_pEffect;

		bool m_EnableRotating{false};

		bool m_F5Held;
	};
}
