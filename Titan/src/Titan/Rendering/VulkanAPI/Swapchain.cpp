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

	InitializeSyncStructure();

	for (size_t i = 0; i < m_SwapchainImageViews.size(); ++i)
	{
		GlobalDeletionQueue.PushFunction([=]
			{
				vkDestroyImageView(device, m_SwapchainImageViews[i], nullptr);
			});
	}
	GlobalDeletionQueue.PushFunction([=]
		{
			vkDestroySwapchainKHR(device, m_SwapChain, nullptr);
		});


}

void Titan::Swapchain::Submit(std::vector<Ref<CommandBuffer>>& commandBuffers)
{
	std::vector<VkCommandBuffer> vkCommandBuffers;
	for (auto& commandBuffer : commandBuffers)
	{
		vkCommandBuffers.push_back(commandBuffer->GetHandle());
	}
	VkSubmitInfo submit = {};

	VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

	submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit.commandBufferCount = vkCommandBuffers.size();
	submit.pCommandBuffers = vkCommandBuffers.data();
	submit.pWaitSemaphores = &m_PresentSemaphore;
	submit.waitSemaphoreCount = 1;
	submit.signalSemaphoreCount = 1;
	submit.pSignalSemaphores = &m_RenderSemaphore;
	submit.pWaitDstStageMask = &waitStage;


	TN_VK_CHECK(vkQueueSubmit(GraphicsContext::GetGraphicsQueue(), 1, &submit, m_RenderFence));
}

void Titan::Swapchain::WaitOnFences(bool waitAndReset)
{
	TN_VK_CHECK(vkWaitForFences(GraphicsContext::GetDevice(), 1, &m_RenderFence, true, UINTMAX_MAX));
	if (waitAndReset)
	{
		TN_VK_CHECK(vkResetFences(GraphicsContext::GetDevice(), 1, &m_RenderFence));
		TN_VK_CHECK(vkAcquireNextImageKHR(GraphicsContext::GetDevice(), m_SwapChain, UINTMAX_MAX, m_PresentSemaphore, nullptr, &m_ImageCount));
	}
}

void Titan::Swapchain::Present()
{
	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = nullptr;

	presentInfo.pSwapchains = &m_SwapChain;
	presentInfo.swapchainCount = 1;

	presentInfo.pWaitSemaphores = &m_RenderSemaphore;
	presentInfo.waitSemaphoreCount = 1;

	presentInfo.pImageIndices = &m_ImageCount;

	TN_VK_CHECK(vkQueuePresentKHR(GraphicsContext::GetGraphicsQueue(), &presentInfo));
}

void Titan::Swapchain::ShutDown()
{

}

Titan::Ref<Titan::Swapchain> Titan::Swapchain::Create()
{
	return CreateRef<Swapchain>();
}

void Titan::Swapchain::InitializeSyncStructure()
{
	VkFenceCreateInfo fenceCreateInfo = {};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.pNext = nullptr;

	fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	TN_VK_CHECK(vkCreateFence(GraphicsContext::GetDevice(), &fenceCreateInfo, nullptr, &m_RenderFence));

	VkSemaphoreCreateInfo semaphoreCreateInfo = {};
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semaphoreCreateInfo.pNext = nullptr;
	semaphoreCreateInfo.flags = 0;

	TN_VK_CHECK(vkCreateSemaphore(GraphicsContext::GetDevice(), &semaphoreCreateInfo, nullptr, &m_PresentSemaphore));
	TN_VK_CHECK(vkCreateSemaphore(GraphicsContext::GetDevice(), &semaphoreCreateInfo, nullptr, &m_RenderSemaphore));

	GlobalDeletionQueue.PushFunction([=]
		{
			vkDestroyFence(GraphicsContext::GetDevice(), m_RenderFence, nullptr);
			vkDestroySemaphore(GraphicsContext::GetDevice(), m_PresentSemaphore, nullptr);
			vkDestroySemaphore(GraphicsContext::GetDevice(), m_RenderSemaphore, nullptr);
		});
}
