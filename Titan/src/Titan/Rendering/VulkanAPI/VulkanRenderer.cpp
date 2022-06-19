#include "VulkanRenderer.h"

#include "GraphicsContext.h"

namespace Titan
{
	void VulkanRenderer::Initialize()
	{
		GraphicsContext::CreateCommandBuffer(m_CommandBuffer);

		{
			RenderPassCreateInfo info{};
			info.width = 1280;
			info.height = 720;
			info.haveDepth = false;
			m_RenderPass = RenderPass::Create(info);
		}
		m_Shader = Shader::Create("Shaders/triangle.frag.spv", ShaderType::Fragment);
	}

	void VulkanRenderer::Begin()
	{
		GraphicsContext::GetSwapChain().WaitOnFences();
		m_CommandBuffer->Reset();
		m_CommandBuffer->Bind();
		VkClearValue clearValue;
		clearValue.color = { {1.0f, 0.f, 1.f, 1.f} };

		VkRenderPassBeginInfo rpInfo{};
		rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		rpInfo.renderPass = m_RenderPass->GetRenderPass();
		rpInfo.framebuffer = m_RenderPass->GetFrameBuffer()->GetFrameBuffer(GraphicsContext::GetSwapChain().GetImageCount());
		rpInfo.renderArea.offset = { 0, 0 };
		rpInfo.renderArea.extent = { 1280, 720 };
		rpInfo.clearValueCount = 1;
		rpInfo.pClearValues = &clearValue;
		vkCmdBeginRenderPass(m_CommandBuffer->GetHandle(), &rpInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdEndRenderPass(m_CommandBuffer->GetHandle());

		TN_VK_CHECK(vkEndCommandBuffer(m_CommandBuffer->GetHandle()));

		std::vector<Ref<CommandBuffer>> commandBuffers;
		commandBuffers.push_back(m_CommandBuffer);
		GraphicsContext::GetSwapChain().Submit(commandBuffers);

		GraphicsContext::GetSwapChain().Present();
	}

	void VulkanRenderer::Shutdown()
	{
		GraphicsContext::ShutDown();
	}
}
