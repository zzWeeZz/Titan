#pragma once
#include <array>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include "Titan/Core/Core.h"
#include <Titan/Rendering/Pipeline.h>

namespace Titan
{
	constexpr uint32_t g_FrameCount = 3;

	struct GraphicsContextInfo
	{
		uint32_t width;
		uint32_t height;
		bool debuging = true;
	};

	class GraphicsContext
	{
	public:
		static void Initialize(const GraphicsContextInfo& info);
		static WinRef<ID3D12CommandAllocator> GetCurrentCommandAllocator();
		static WinRef<ID3D12CommandQueue> CommandQueue() { return m_CommandQueue; }
		static WinRef<ID3D12GraphicsCommandList> CommandList() { return m_CommandList; }
		static WinRef<IDXGISwapChain3> SwapChain() { return m_Swapchain; }
		static WinRef<ID3D12Device> Device() { return m_Device; }
		static uint32_t GetFrameIndex() { return m_FrameIndex; }
		static void WaitForNextFrame(bool getIndexFromSwapchain = true);
		static void Begin();// this function should the renderer prob handle.
		static void Clear(); // this has no functionality, Needs to be figured out.
		static void End(); // same with this one, should be moved to the renderer.
		static void Reset(Ref<Pipeline> initual = nullptr);
		static WinRef<ID3D12Resource> GetCurrentRtv();
		static void ExecuteCommandList();
		static void SignalCommandQueue();
		static void Shutdown();
	private:
		static void InitializeSwapChain(const GraphicsContextInfo& info, IDXGIFactory4* factory);
		static void ValidationLayer();
		static void EnableShaderBasedValidation();
		inline static WinRef<ID3D12Device> m_Device;
		inline static WinRef<IDXGISwapChain3> m_Swapchain;
		inline static WinRef<ID3D12CommandQueue> m_CommandQueue;
		inline static WinRef<ID3D12DescriptorHeap> m_RTVDescriptorHeap;
		inline static std::array<WinRef<ID3D12Resource>, g_FrameCount> m_RenderTargets;
		inline static std::array<WinRef<ID3D12CommandAllocator>, g_FrameCount> m_CommandAllocators;
		inline static WinRef<ID3D12GraphicsCommandList> m_CommandList;
		inline static std::array<WinRef<ID3D12Fence1>, g_FrameCount> m_Fences;

		inline static WinRef<ID3D12InfoQueue> m_InfoQueue;

		inline static HANDLE m_FenceEvent;
		inline static std::array<uint64_t, g_FrameCount> m_FenceValues;
		inline static uint32_t m_FrameIndex;
		inline static int32_t m_RtvDescriptorSize;
	};
}