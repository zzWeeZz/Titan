#include "TNpch.h"
#include "Swapchain.h"
#include "Titan/Rendering/Backend/PhysicalDevice.h"
#include "Titan/Rendering/Backend/Device.h"
#include "Titan/Application.h"
#include "GLFW/glfw3.h"
#include "Titan/Rendering/GraphicsContext.h"
#include "Titan/Utils/TitanAllocator.h"
namespace Titan
{
	Swapchain::Swapchain()
	{
		m_CurrentImage = 0;
		m_Width = 0;
		m_Height = 0;
		m_NeedsToResize = false;
		m_Swapchain = {};
		for (size_t i = 0; i < g_FramesInFlight; ++i)
		{
			m_ImageAvailableSemaphores[i] = {};
			m_RenderFinishedSemaphores[i] = {};
			m_InFlightFences[i] = {};
		}
		m_SwapchainExtent = {};
		m_SwapchainFormat = VK_FORMAT_UNDEFINED;
		m_SwapchainRenderPass = {};
	}
	void Swapchain::Create(PhysicalDevice& physicalDevice, Device& device)
	{
		Validate(physicalDevice, device);
		m_Width = m_SwapchainExtent.width;
		m_Height = m_SwapchainExtent.height;
		CreateSyncObject();
		CreateRenderPass();
		m_SwapchainFrameBuffers.resize(m_SwapchainViews.size());
		CreateFrameBuffer();
	}
	int32_t Swapchain::GetNextImage()
	{
		uint32_t imageIndex;
		auto result = vkAcquireNextImageKHR(GraphicsContext::GetDevice().GetHandle(), m_Swapchain, UINT64_MAX, GetImageAvailableSemaphore(GraphicsContext::GetCurrentFrame()), VK_NULL_HANDLE, &imageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || m_NeedsToResize)
		{
			InternalResize(m_Width, m_Height);
			m_Width = m_SwapchainExtent.width;
			m_Height = m_SwapchainExtent.height;
			m_NeedsToResize = false;
			return -1;
		}
		m_CurrentImage = imageIndex;
		return imageIndex;
	}
	uint32_t Swapchain::GetCurrentImageIndex()
	{
		return m_CurrentImage;
	}
	VkImage& Swapchain::GetImage(size_t index)
	{
		return m_SwapchainImages[index];
	}
	VkImageView& Swapchain::GetCurrentView(size_t index)
	{
		return m_SwapchainViews[index];
	}
	void Swapchain::Resize(size_t width, size_t height)
	{
		m_NeedsToResize = true;
		m_Width = width;
		m_Height = height;
	}
	void Swapchain::CleanUp()
	{
		auto& device = GraphicsContext::GetDevice();
		for (size_t i = 0; i < m_SwapchainFrameBuffers.size(); ++i)
		{
			vkDestroyFramebuffer(device.GetHandle(), m_SwapchainFrameBuffers[i], nullptr);
		}
		for (size_t i = 0; i < m_SwapchainViews.size(); ++i)
		{
			vkDestroyImageView(device.GetHandle(), m_SwapchainViews[i], nullptr);
		}
		vkDestroySwapchainKHR(device.GetHandle(), m_Swapchain, nullptr);
	}
	void Swapchain::Shutdown(Device& device, bool destroyRenderTarget)
	{
		if (destroyRenderTarget) vkDestroyRenderPass(device.GetHandle(), m_SwapchainRenderPass, nullptr);

		for (size_t i = 0; i < g_FramesInFlight; ++i)
		{
			vkDestroySemaphore(device.GetHandle(), m_ImageAvailableSemaphores[i], nullptr);
			vkDestroySemaphore(device.GetHandle(), m_RenderFinishedSemaphores[i], nullptr);
			vkDestroyFence(device.GetHandle(), m_InFlightFences[i], nullptr);
		}
	}
	void Swapchain::Validate(PhysicalDevice& physicalDevice, Device& device, int32_t width, int32_t height)
	{
		SwapChainSupportDetails supportDetails;
		GraphicsContext::QuerySwapchainSupport(supportDetails);

		VkSurfaceFormatKHR surfaceFormat = ChooseSwapchainFormat(supportDetails.formats);
		VkPresentModeKHR presentMode = ChooseSwapchainPresentMode(supportDetails.presentModes);

		VkExtent2D extent = ChooseSwapchainExtent(supportDetails.capabilities);

		m_SwapchainExtent = extent;
		m_SwapchainFormat = surfaceFormat.format;

		uint32_t imageCount = supportDetails.capabilities.minImageCount + 1;
		if (supportDetails.capabilities.maxImageCount > 0 && imageCount > supportDetails.capabilities.maxImageCount)
		{
			imageCount = supportDetails.capabilities.maxImageCount;
		}
		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = GraphicsContext::GetSurface();

		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = m_SwapchainExtent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		QueueFamilyIndices indices = GraphicsContext::GetPhysicalDevice().FindQueueFamilies();
		uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

		if (indices.graphicsFamily != indices.presentFamily)
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0; // Optional
			createInfo.pQueueFamilyIndices = nullptr; // Optional
		}

		createInfo.preTransform = supportDetails.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;
		TN_VK_CHECK(vkCreateSwapchainKHR(device.GetHandle(), &createInfo, nullptr, &m_Swapchain));
		vkGetSwapchainImagesKHR(device.GetHandle(), m_Swapchain, &imageCount, nullptr);
		m_SwapchainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(device.GetHandle(), m_Swapchain, &imageCount, m_SwapchainImages.data());


		m_SwapchainViews.resize(m_SwapchainImages.size());
		for (size_t i = 0; i < m_SwapchainViews.size(); ++i)
		{
			VkImageViewCreateInfo viewCreateInfo{};
			viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewCreateInfo.image = m_SwapchainImages[i];

			viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewCreateInfo.format = m_SwapchainFormat;

			viewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			viewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			viewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			viewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

			viewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			viewCreateInfo.subresourceRange.baseMipLevel = 0;
			viewCreateInfo.subresourceRange.levelCount = 1;
			viewCreateInfo.subresourceRange.baseArrayLayer = 0;
			viewCreateInfo.subresourceRange.layerCount = 1;

			TN_VK_CHECK(vkCreateImageView(GraphicsContext::GetDevice().GetHandle(), &viewCreateInfo, nullptr, &m_SwapchainViews[i]));
		}
	}
	void Swapchain::InternalResize(size_t width, size_t height)
	{
		GraphicsContext::GetDevice().WaitForIdle();
		CleanUp();
		Validate(GraphicsContext::GetPhysicalDevice(), GraphicsContext::GetDevice(), (int32_t)width, (int32_t)height);
		CreateFrameBuffer();
	}
	void Swapchain::CreateSyncObject()
	{
		auto& device = GraphicsContext::GetDevice().GetHandle();
		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		for (size_t i = 0; i < g_FramesInFlight; ++i)
		{
			TN_VK_CHECK(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &m_ImageAvailableSemaphores[i]));
			TN_VK_CHECK(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &m_RenderFinishedSemaphores[i]));
			TN_VK_CHECK(vkCreateFence(device, &fenceInfo, nullptr, &m_InFlightFences[i]));
		}
	}
	void Swapchain::CreateRenderPass()
	{
		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = m_SwapchainFormat;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;

		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		dependency.dstStageMask = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependency.dstAccessMask = 0;

		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = 1;
		renderPassInfo.pAttachments = &colorAttachment;
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		TN_VK_CHECK(vkCreateRenderPass(GraphicsContext::GetDevice().GetHandle(), &renderPassInfo, nullptr, &m_SwapchainRenderPass));
	}
	void Swapchain::CreateFrameBuffer()
	{
		for (size_t i = 0; i < m_SwapchainViews.size(); i++)
		{
			VkImageView attachments[] = { m_SwapchainViews[i] };

			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = m_SwapchainRenderPass;
			framebufferInfo.attachmentCount = 1;
			framebufferInfo.pAttachments = attachments;
			framebufferInfo.width = m_SwapchainExtent.width;
			framebufferInfo.height = m_SwapchainExtent.height;
			framebufferInfo.layers = 1;

			TN_VK_CHECK(vkCreateFramebuffer(GraphicsContext::GetDevice().GetHandle(), &framebufferInfo, nullptr, &m_SwapchainFrameBuffers[i]));
		}
	}
	VkSurfaceFormatKHR Swapchain::ChooseSwapchainFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
	{
		for (const auto& availableFormat : availableFormats)
		{
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				return availableFormat;
			}
		}
		return availableFormats[0];
	}
	VkPresentModeKHR Swapchain::ChooseSwapchainPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
	{
		for (const auto& availablePresentMode : availablePresentModes)
		{
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				return availablePresentMode;
			}
		}
		return VK_PRESENT_MODE_FIFO_KHR;
	}
	VkExtent2D Swapchain::ChooseSwapchainExtent(const VkSurfaceCapabilitiesKHR& capabilities)
	{
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		{
			return capabilities.currentExtent;
		}
		else
		{
			int32_t width, height;
			auto& glfwWindow = *static_cast<GLFWwindow*>(Application::GetWindow().GetNativeWindow());
			glfwGetFramebufferSize(&glfwWindow, &width, &height);

			VkExtent2D actualExtent = { static_cast<uint32_t>(width),static_cast<uint32_t>(height) };
			actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
			actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
			return actualExtent;
		}
	}
}