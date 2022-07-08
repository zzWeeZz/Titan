#include "RenderPass.h"

#include "GraphicsContext.h"

namespace Titan
{
	RenderPass::RenderPass(const RenderPassCreateInfo& info)
	{
		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = info.colorFormat;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;

		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		std::vector<VkAttachmentDescription> attachments;
		attachments.push_back(colorAttachment);
		std::vector<VkSubpassDependency> dependencies;
		dependencies.push_back(dependency);
		VkSubpassDescription depthSubpass{};
		if (info.haveDepth)
		{
			VkAttachmentDescription depthAttachment{};
			depthAttachment.flags = 0;
			depthAttachment.format = info.depthFormat;
			depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
			depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			attachments.push_back(depthAttachment);

			VkAttachmentReference depthAttachmentRef{};
			depthAttachmentRef.attachment = 1;
			depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			depthSubpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			depthSubpass.colorAttachmentCount = 1;
			depthSubpass.pColorAttachments = &colorAttachmentRef;
			depthSubpass.pDepthStencilAttachment = &depthAttachmentRef;


			VkSubpassDependency depthDependency{};
			depthDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
			depthDependency.dstSubpass = 0;
			depthDependency.srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
			depthDependency.srcAccessMask = 0;
			depthDependency.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
			depthDependency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			dependencies.push_back(depthDependency);
		}

		VkRenderPassCreateInfo renderPassInfo{};
		std::vector<VkSubpassDescription> sub = { subpass, depthSubpass };
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = attachments.size();
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = info.haveDepth ? &depthSubpass : &subpass;
		renderPassInfo.dependencyCount = dependencies.size();
		renderPassInfo.pDependencies = dependencies.data();

		TN_VK_CHECK(vkCreateRenderPass(GraphicsContext::GetDevice(), &renderPassInfo, nullptr, &m_RenderPass));

		FrameBufferInfo frameInfo{};
		frameInfo.Width = info.width;
		frameInfo.Height = info.height;
		frameInfo.RenderPass = m_RenderPass;
		m_FrameBuffer = FrameBuffer::Create(frameInfo);
		GlobalDeletionQueue.PushFunction([=]
			{
				vkDestroyRenderPass(GraphicsContext::GetDevice(), m_RenderPass, nullptr);
			});
	}

	void RenderPass::Shutdown()
	{
	}

	Ref<RenderPass> RenderPass::Create(const RenderPassCreateInfo& info)
	{
		return CreateRef<RenderPass>(info);
	}
}
