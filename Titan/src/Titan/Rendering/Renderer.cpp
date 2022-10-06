#include "TNpch.h"
#include "Renderer.h"

#include "Titan/Application.h"

#include <glm/gtx/transform.hpp>

#include "Titan/Rendering/GraphicsContext.h"

namespace Titan
{
	void Renderer::Initialize()
	{
	}

	void Renderer::Begin()
	{
		GraphicsContext::WaitForNextFrame();
		GraphicsContext::GetCurrentCommandAllocator()->Reset();
		GraphicsContext::Reset();
		GraphicsContext::Clear();

		ID3D12CommandList* ppCommandList[] = {GraphicsContext::CommandList().Get()};
		GraphicsContext::CommandQueue()->ExecuteCommandLists(_countof(ppCommandList), ppCommandList);

		GraphicsContext::SignalCommandQueue();

		GraphicsContext::SwapChain()->Present(0, 0);
	}

	void Renderer::Shutdown()
	{
		GraphicsContext::Shutdown();
	}
}
