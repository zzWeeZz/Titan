#include "RenderAPI.h"

#include "Titan/AssetImporter/AssetImporter.h"
#include "VulkanAPI/GraphicsContext.h"
#include "VulkanAPI/VulkanRenderer.h"
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

	void RenderAPI::Begin()
	{
		switch (s_CurrentAPI)
		{
		case API::Vulkan:
		{
			BeginVulkanRender();
		}
		break;
		case API::DX12:
		{

		}
		break;
		default:;
		}
	}

	void RenderAPI::Shutdown()
	{
		switch (s_CurrentAPI)
		{
		case API::Vulkan:
		{
			ShutdownVulkan();
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
		VulkanRenderer::Initialize();
	}

	void RenderAPI::BeginVulkanRender()
	{
		VulkanRenderer::Begin();
	}

	void RenderAPI::ShutdownVulkan()
	{
		AssetImporter::Shutdown();
		VulkanRenderer::Shutdown();
	}
}
