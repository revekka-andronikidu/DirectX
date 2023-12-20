#include "pch.h"
#include "Renderer.h"
#include "Mesh.h"

namespace dae {

	Renderer::Renderer(SDL_Window* pWindow) :
		m_pWindow(pWindow)
	{
		//Initialize
		SDL_GetWindowSize(pWindow, &m_Width, &m_Height);

		//Initialize DirectX pipeline
		const HRESULT result = InitializeDirectX();
		if (result == S_OK)
		{
			m_IsInitialized = true;
			std::cout << "DirectX is initialized and ready!\n";
		}
		else
		{
			std::cout << "DirectX initialization failed!\n";
		}

		//Create some data for our mesh
		std::vector<Vertex_PosCol> vertices
		{
			{{0.0f, 0.5f, 0.5f}, {colors::Red}},
			{{0.5f, -0.5f, 0.5f}, {colors::Blue}},
			{{-0.5f, -0.5f, 0.5f}, {colors::Green}},
		};
		
		std::vector<uint32_t> indices{ 0,1,2 };
		
		m_pMesh = new Mesh(m_pDevice, vertices, indices);
	}

	Renderer::~Renderer()
	{
		DeleteResourceLeaks();

		delete m_pMesh;
	}

	void Renderer::Update(const Timer* pTimer)
	{
		
	}


	void Renderer::Render() const
	{
		if (!m_IsInitialized)
			return;
	
		//1. Clear RTV & DSV
		constexpr float color[4] = { 0.f, 0.f, 0.5f, 1.f };
		m_pDeviceContext->ClearRenderTargetView(m_RenderTargetView, color);
		m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

		//2. Set pipeline + invoke draw calls (= Render)

		m_pMesh->Render(m_pDeviceContext);

		//3. Present backbuffer (swap)
		m_pSwapChain->Present(0, 0);

	}

	HRESULT Renderer::InitializeDirectX()
	{
		//1. Create Device and DeviceContext
		D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_1;
		uint32_t createDeviceFlags = 0;
		//return S_FALSE;
#if defined(DEBUG) || defined(_DEBUG)
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		HRESULT result = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, createDeviceFlags, &featureLevel,
			1, D3D11_SDK_VERSION, &m_pDevice, nullptr, &m_pDeviceContext);
		if (FAILED(result))
			return result;

		//Create DXGI
		IDXGIFactory1* pDxgiFactory{};
		result = CreateDXGIFactory1(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&pDxgiFactory));
		if (FAILED(result))
			return result;

		//2. Create Swapchain
		DXGI_SWAP_CHAIN_DESC swapChainDecs{};
		swapChainDecs.BufferDesc.Width = m_Width;
		swapChainDecs.BufferDesc.Height = m_Height;
		swapChainDecs.BufferDesc.RefreshRate.Numerator = 1;
		swapChainDecs.BufferDesc.RefreshRate.Denominator = 60;
		swapChainDecs.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDecs.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDecs.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainDecs.SampleDesc.Count = 1;
		swapChainDecs.SampleDesc.Quality = 0;
		swapChainDecs.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDecs.BufferCount = 1;
		swapChainDecs.Windowed = true;
		swapChainDecs.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapChainDecs.Flags = 0;

		//Get the handle (HWND) from SDL backbuffer
		SDL_SysWMinfo sysWMInfo{};
		SDL_GetVersion(&sysWMInfo.version);
		SDL_GetWindowWMInfo(m_pWindow, &sysWMInfo);
		swapChainDecs.OutputWindow = sysWMInfo.info.win.window;

		result = pDxgiFactory->CreateSwapChain(m_pDevice, &swapChainDecs, &m_pSwapChain);
		if (FAILED(result))
			return result;

		//3. Create DepthStencil (DS) & DepthStencilView (DSV)
		//Resource
		D3D11_TEXTURE2D_DESC depthStencilDesc{};
		depthStencilDesc.Width = m_Width;
		depthStencilDesc.Height = m_Height;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0;
		depthStencilDesc.MiscFlags = 0;

		//View
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
		depthStencilViewDesc.Format = depthStencilDesc.Format;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;

		result = m_pDevice->CreateTexture2D(&depthStencilDesc, nullptr, &m_pDepthStencilBuffer);
		if (FAILED(result))
			return result;

		result = m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &depthStencilViewDesc, &m_pDepthStencilView);
		if (FAILED(result))
			return result;

		//4. Create RenderTarget (RT) and RenderTargetView (RTV)

		//Resource
		result = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&m_pRenderTargetBuffer));
		if (FAILED(result))
			return result;

		//view
		result = m_pDevice->CreateRenderTargetView(m_pRenderTargetBuffer, nullptr, &m_RenderTargetView);
		if (FAILED(result))
			return result;

		//5. Bind RTV & DSV to Output Merger Stage
		m_pDeviceContext->OMSetRenderTargets(1, &m_RenderTargetView, m_pDepthStencilView);


		/*
		Viewport defines where the back buffer will be rendered on screen. Multiple viewports
		can be handy in case of local multiplayer games or spectator views.
		Viewports are used to translate them directly to NDC space
		*/
		//6. Set viewport
		D3D11_VIEWPORT viewport{};
		viewport.Width = static_cast<float>(m_Width);
		viewport.Height = static_cast<float>(m_Height);
		viewport.TopLeftX = 0.f;
		viewport.TopLeftY = 0.f;
		viewport.MinDepth = 0.f;
		viewport.MaxDepth = 1.f;
		m_pDeviceContext->RSSetViewports(1, &viewport);

		return result;
	}

	void Renderer::DeleteResourceLeaks()
	{
		if (m_pDeviceContext)
		{
			// Restore to default settings
			m_pDeviceContext->ClearState();
			// Send any queued up commands to GPU
			m_pDeviceContext->Flush();
			// Release it into the abyss
			m_pDeviceContext->Release();
		}

		SAFE_RELEASE(m_pDevice);
		SAFE_RELEASE(m_pSwapChain);
		SAFE_RELEASE(m_pDepthStencilBuffer);
		SAFE_RELEASE(m_pDepthStencilView);
		SAFE_RELEASE(m_pRenderTargetBuffer);
		SAFE_RELEASE(m_RenderTargetView);

		/*if (m_pDevice)
			m_pDevice->Release();
		if (m_pSwapChain)
			m_pSwapChain->Release();
		if (m_pDepthStencilBuffer)
			m_pDepthStencilBuffer->Release();
		if (m_pDepthStencilView)
			m_pDepthStencilView->Release();
		if (m_pRenderTargetBuffer)
			m_pRenderTargetBuffer->Release();
		if (m_RenderTargetView)
			m_RenderTargetView->Release();*/
	}

}

