#include "TNpch.h"
#include "GraphicsContext.h"
#include <combaseapi.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <dx12helpers/d3dx12.h>
#include "Titan/Application.h"
#include "Titan/Platform/WindowsWindow.h"
#include "d3d12sdklayers.h"
#include "optick.h"
namespace Titan
{
	void GraphicsContext::Initialize(const GraphicsContextInfo& info)
	{

		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Titan";
		appInfo.applicationVersion = VK_API_VERSION_1_3;
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_API_VERSION_1_3;
		appInfo.apiVersion = VK_API_VERSION_1_3;

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;

		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		createInfo.enabledExtensionCount = glfwExtensionCount;
		createInfo.ppEnabledExtensionNames = glfwExtensions;

		createInfo.enabledLayerCount = 0;

		TN_VK_CHECK(vkCreateInstance(&createInfo, nullptr, &m_Instance));

		IDXGIFactory4* dxgiFactory;
		TN_DX_CHECK(CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory)));

		IDXGIAdapter1* adapter;
		int32_t adapterIndex = 0;
		bool adapterFound = false;

		while (dxgiFactory->EnumAdapters1(adapterIndex, &adapter) != DXGI_ERROR_NOT_FOUND)
		{
			DXGI_ADAPTER_DESC1 desc;
			adapter->GetDesc1(&desc);

			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				// we dont want a software device
				adapterIndex++; // add this line here. Its not currently in the downloadable project
				continue;
			}

			// we want a device that is compatible with direct3d 12 (feature level 11 or higher)
			HRESULT hr = D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr);
			if (SUCCEEDED(hr))
			{
				adapterFound = true;
				TN_CORE_INFO("Device adapter found!");
				break;
			}

			adapterIndex++;
		}
		if (info.debuging)
			EnableShaderBasedValidation();

		// Creates the device
		TN_DX_CHECK(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(m_Device.GetAddressOf())));
		D3D12_COMMAND_QUEUE_DESC cqDesc = {};
		TN_DX_CHECK(m_Device->CreateCommandQueue(&cqDesc, IID_PPV_ARGS(m_CommandQueue.GetAddressOf())));

		D3D12_FEATURE_DATA_SHADER_MODEL shaderModel = { D3D_SHADER_MODEL_6_0 };
		if (m_Device->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &shaderModel, sizeof(shaderModel)) || (shaderModel.HighestShaderModel < D3D_SHADER_MODEL_6_0))
		{
			TN_CORE_ERROR("SHADER MODEL 6.0 is not supported!");
		}

		m_Device->QueryInterface(m_InfoQueue.GetAddressOf());

		InitializeSwapChain(info, dxgiFactory);

		// Render target Creation
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors = g_FrameCount;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		TN_DX_CHECK(m_Device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(m_RTVDescriptorHeap.GetAddressOf())));

		m_RtvDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());


		for (size_t i = 0; i < g_FrameCount; ++i)
		{
			TN_DX_CHECK(m_Swapchain->GetBuffer(static_cast<UINT>(i), IID_PPV_ARGS(m_RenderTargets[i].GetAddressOf())));
			m_Device->CreateRenderTargetView(m_RenderTargets[i].Get(), nullptr, rtvHandle);

			rtvHandle.Offset(1, m_RtvDescriptorSize);
		}

		for (size_t i = 0; i < g_FrameCount; ++i)
		{
			TN_DX_CHECK(m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_CommandAllocators[i].GetAddressOf())));
		}

		TN_DX_CHECK(m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CommandAllocators[0].Get(), NULL, IID_PPV_ARGS(m_CommandList.GetAddressOf())));
		m_CommandList->Close(); 
		for (size_t i = 0; i < g_FrameCount; ++i)
		{
			TN_DX_CHECK(m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_Fences[i].GetAddressOf())));

			m_FenceValues[i] = 0;
		}

		m_FenceEvent = CreateEvent(nullptr, false, false, nullptr);
		if (m_FenceEvent == nullptr)
		{
			TN_CORE_ASSERT(false, "Could not create Event!");
		}
		OPTICK_GPU_INIT_D3D12(m_Device.Get(), m_CommandQueue.GetAddressOf(), 1);
		OPTICK_GPU_CONTEXT(m_CommandList.Get());
		dxgiFactory->Release();
		adapter->Release();
	}

	WinRef<ID3D12CommandAllocator> GraphicsContext::GetCurrentCommandAllocator()
	{
		return  m_CommandAllocators[m_FrameIndex];
	}

	void GraphicsContext::WaitForNextFrame(bool getIndexFromSwapchain)
	{
		if (getIndexFromSwapchain)
		{
			m_FrameIndex = m_Swapchain->GetCurrentBackBufferIndex();
		}

		if (m_Fences[m_FrameIndex]->GetCompletedValue() < m_FenceValues[m_FrameIndex])
		{

			(m_Fences[m_FrameIndex]->SetEventOnCompletion(m_FenceValues[m_FrameIndex], m_FenceEvent));

			WaitForSingleObject(m_FenceEvent, INFINITE);
		}

		m_FenceValues[m_FrameIndex]++;
	}

	void GraphicsContext::Begin()
	{
		auto beginRB = CD3DX12_RESOURCE_BARRIER::Transition(
			m_RenderTargets[m_FrameIndex].Get(),
			D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_RENDER_TARGET);

		m_CommandList->ResourceBarrier(
			1,
			&beginRB);
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), m_FrameIndex, m_RtvDescriptorSize);
		
		static float time = 0;
		time += 0.001f;
		const float clearColor[] = { 0,(cosf(time) + 1.f) / 2.f, 0, 1 };
		m_CommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	}

	void GraphicsContext::End()
	{
		auto endRB = CD3DX12_RESOURCE_BARRIER::Transition(
			m_RenderTargets[m_FrameIndex].Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT);
		m_CommandList->ResourceBarrier(
			1,
			&endRB);
		m_CommandList->Close();
		ValidationLayer();
	}

	void GraphicsContext::Reset(Ref<Pipeline> initual)
	{
		if (initual)
		{
			m_CommandList->Reset(m_CommandAllocators[m_FrameIndex].Get(), initual->m_PipelineStateObject.Get());
			return;
		}
		m_CommandList->Reset(m_CommandAllocators[m_FrameIndex].Get(), NULL);
	}

	WinRef<ID3D12Resource> GraphicsContext::GetCurrentRtv()
	{
		return m_RenderTargets[m_FrameIndex];
	}

	void GraphicsContext::Clear()
	{
		// transfers swapchain rendertarget


		// command list is ready for commands


		//Draw Something ex: triangle


		// send it to the present state
	}

	void GraphicsContext::ExecuteCommandList()
	{
		ID3D12CommandList* ppCommandList[] = { GraphicsContext::CommandList().Get() };
		GraphicsContext::CommandQueue()->ExecuteCommandLists(_countof(ppCommandList), ppCommandList);
		m_FenceValues[m_FrameIndex]++;
		SignalCommandQueue();
	}

	void GraphicsContext::SignalCommandQueue()
	{
		m_CommandQueue->Signal(m_Fences[m_FrameIndex].Get(), m_FenceValues[m_FrameIndex]);
	}

	void GraphicsContext::Shutdown()
	{

		WaitForNextFrame();
		auto error = CloseHandle(m_FenceEvent);
		Reset();
		if (error == 0)
		{
			TN_CORE_ERROR("IT BROKE");
		}


		for (int i = 0; i < g_FrameCount; ++i)
		{
			m_FrameIndex = i;
			WaitForNextFrame(false);
		}



		// get swapchain out of full screen before exiting
		BOOL fs = false;
		m_Swapchain->GetFullscreenState(&fs, NULL);
		if (fs)
			m_Swapchain->SetFullscreenState(false, NULL);


		TN_SAFE_RELEASE(m_Device);
		TN_SAFE_RELEASE(m_Swapchain);
		TN_SAFE_RELEASE(m_CommandQueue);
		TN_SAFE_RELEASE(m_RTVDescriptorHeap);
		TN_SAFE_RELEASE(m_CommandList);

		for (int i = 0; i < g_FrameCount; ++i)
		{
			TN_SAFE_RELEASE(m_RenderTargets[i]);
			TN_SAFE_RELEASE(m_CommandAllocators[i]);
			TN_SAFE_RELEASE(m_Fences[i]);
		};
	}
	void GraphicsContext::InitializeSwapChain(const GraphicsContextInfo& info, IDXGIFactory4* factory)
	{
		// Time to create the swapchain
		DXGI_SWAP_CHAIN_DESC scDesc{};

		DXGI_MODE_DESC mDesc{};
		mDesc.Width = info.width;
		mDesc.Height = info.height;
		DXGI_RATIONAL ratio{};
		mDesc.RefreshRate = ratio;
		mDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		mDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		mDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		scDesc.BufferDesc = mDesc;

		DXGI_SAMPLE_DESC sDesc{};
		sDesc.Count = 1;

		scDesc.SampleDesc = sDesc;
		scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scDesc.BufferCount = g_FrameCount;
		scDesc.OutputWindow = glfwGetWin32Window(reinterpret_cast<GLFWwindow*>(Application::GetWindow().GetNativeWindow()));
		scDesc.Windowed = TRUE;
		scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		scDesc.Flags = 0;

		IDXGISwapChain* tempSwapchain;
		factory->CreateSwapChain(m_CommandQueue.Get(), &scDesc, &tempSwapchain);

		m_Swapchain = reinterpret_cast<IDXGISwapChain3*>(tempSwapchain);

		m_FrameIndex = m_Swapchain->GetCurrentBackBufferIndex();
	}
	void GraphicsContext::ValidationLayer()
	{
		if (!m_InfoQueue) return;

		SIZE_T messageLength = 0;
		HRESULT hr = m_InfoQueue->GetMessage(0, NULL, &messageLength);

		// Allocate space and get the message
		if (messageLength)
		{
			D3D12_MESSAGE* pMessage = nullptr;
			pMessage = (D3D12_MESSAGE*)malloc(messageLength);
			hr = m_InfoQueue->GetMessage(0, pMessage, &messageLength);
			
			switch (pMessage->Severity)
			{
			case D3D12_MESSAGE_SEVERITY_INFO:
				TN_CORE_INFO(pMessage->pDescription);
				break;
			case D3D12_MESSAGE_SEVERITY_WARNING:
				TN_CORE_WARN(pMessage->pDescription);
				break;
			case D3D12_MESSAGE_SEVERITY_ERROR:
				TN_CORE_ERROR(pMessage->pDescription);
				break;
			case D3D12_MESSAGE_SEVERITY_MESSAGE:
				TN_CORE_TRACE(pMessage->pDescription);
				break;
			default:
				break;
			}

			free(pMessage);
		}
	}
	void GraphicsContext::EnableShaderBasedValidation()
	{
		WinRef<ID3D12Debug> spDebugController0;
		WinRef<ID3D12Debug1> spDebugController1;
		TN_DX_CHECK(D3D12GetDebugInterface(IID_PPV_ARGS(&spDebugController0)));
		spDebugController0->EnableDebugLayer();
		TN_DX_CHECK(spDebugController0->QueryInterface(IID_PPV_ARGS(&spDebugController1)));
		//spDebugController1->SetEnableGPUBasedValidation(true);
	}
}