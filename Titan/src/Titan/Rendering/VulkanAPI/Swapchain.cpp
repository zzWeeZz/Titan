#include "Swapchain.h"
#include "VkBootstrap.h"
#include "GraphicsContext.h"
#include "Titan/Application.h"

Titan::Swapchain::Swapchain()
{
	auto& device = GraphicsContext::GetDevice();
	auto& physicalDevice = GraphicsContext::GetPhysicalDevice();
	auto& surface = GraphicsContext::GetSurface();
	vkb::SwapchainBuilder swapchainBuilder{ physicalDevice, device, surface };

	vkb::Swapchain vkbSwapchain = swapchainBuilder
		.use_default_format_selection()
		.set_desired_present_mode(VK_PRESENT_MODE_MAILBOX_KHR)
		.set_desired_extent(Application::GetWindow().GetWidth(), Application::GetWindow().GetHeight())
		.build()
		.value();

	m_SwapChain = vkbSwapchain.swapchain;
	m_SwapchainImages = vkbSwapchain.get_images().value();
	m_SwapchainImageViews = vkbSwapchain.get_image_views().value();
	m_SwapchainImageFormat = vkbSwapchain.image_format;

	for (size_t i = 0; i < m_SwapchainImageViews.size(); ++i)
	{
		m_DeletionQueue.PushFunction([=]
			{
				vkDestroyImageView(device, m_SwapchainImageViews[i], nullptr);
			});
	}
	m_DeletionQueue.PushFunction([=]
		{
			vkDestroySwapchainKHR(device, m_SwapChain, nullptr);
		});
}

void Titan::Swapchain::ShutDown()
{
	m_DeletionQueue.Flush();
}

Titan::Ref<Titan::Swapchain> Titan::Swapchain::Create()
{
	return CreateRef<Swapchain>();
}
