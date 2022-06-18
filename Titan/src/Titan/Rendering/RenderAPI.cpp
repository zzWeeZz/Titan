#include "RenderAPI.h"
#include "VulkanAPI/GraphicsContext.h"

namespace Titan
{
	void RenderAPI::Initialize(const API& api)
	{
		s_CurrentAPI = api;
		switch (api)
		{
		case API::Vulkan:
		{
			InitializeVulkan();
		}
		break;
		case API::DX12:
		{

		}
		break;
		default:;
		}
	}

	void RenderAPI::InitializeVulkan()
	{
		GraphicsContext::Initialize();

	}
}