#include "Pipeline.h"

#include "GraphicsContext.h"
#include "Buffers/Buffers.h"

namespace Titan
{
	VkPipeline PipelineBuilder::BuildPipeline(VkRenderPass renderPass)
	{
		VkPipelineViewportStateCreateInfo viewportState{};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.pNext = nullptr;

		viewportState.viewportCount = 1;
		viewportState.pViewports = &m_Viewport;
		viewportState.scissorCount = 1;
		viewportState.pScissors = &m_Scissor;

		VkPipelineColorBlendStateCreateInfo colorBlendState{};
		colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlendState.pNext = nullptr;
		colorBlendState.logicOpEnable = VK_FALSE;
		colorBlendState.logicOp = VK_LOGIC_OP_COPY;
		colorBlendState.attachmentCount = 1;
		colorBlendState.pAttachments = &m_ColorBlendAttachmentState;

		VkPipelineRenderingCreateInfo pipelineRenderingCreateInfo{};
		pipelineRenderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
		pipelineRenderingCreateInfo.colorAttachmentCount = 1;
		pipelineRenderingCreateInfo.pColorAttachmentFormats = &GraphicsContext::GetSwapChain().GetFormat();

		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.pNext = &pipelineRenderingCreateInfo;

	

		std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
		for (auto& shader : m_ShaderStages)
		{
			shaderStages.push_back(shader->GetCreateInfo());
		}

		pipelineInfo.stageCount = shaderStages.size();
		pipelineInfo.pStages = shaderStages.data();
		pipelineInfo.pVertexInputState = &m_VertexInputState;
		pipelineInfo.pInputAssemblyState = &m_InputAssemblyState;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &m_Rasterizer;
		pipelineInfo.pMultisampleState = &m_Multisampling;
		pipelineInfo.pColorBlendState = &colorBlendState;
		pipelineInfo.pDepthStencilState = &m_DepthStencilState;
		pipelineInfo.layout = m_PipelineLayout;
		pipelineInfo.renderPass = nullptr;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

		/*pipelineInfo.pDepthStencilState = &m_DepthStencilState;*/

		VkPipeline newPipeline;
		if (vkCreateGraphicsPipelines(GraphicsContext::GetDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &newPipeline) != VK_SUCCESS)
		{
			TN_CORE_ERROR("Failed to create graphics pipeline!");
			return VK_NULL_HANDLE;
		}


		return newPipeline;
	}

	Pipeline::Pipeline(PipelineBuilder& builder, Ref<RenderPass> renderPass)
	{
		VkPipelineLayoutCreateInfo info{};
		info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		info.pNext = nullptr;

		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(MeshConstant);

		//empty defaults
		info.flags = 0;
		info.setLayoutCount = 0;
		info.pSetLayouts = nullptr;
		info.pushConstantRangeCount = 1;
		info.pPushConstantRanges = &pushConstantRange;

		info.setLayoutCount = builder.m_DescriptorSetLayouts.size();
		info.pSetLayouts = builder.m_DescriptorSetLayouts.data();
		TN_VK_CHECK(vkCreatePipelineLayout(GraphicsContext::GetDevice(), &info, nullptr, &m_PipelineLayout));


		builder.m_PipelineLayout = m_PipelineLayout;
		m_Pipeline = builder.BuildPipeline(renderPass->GetRenderPass());
		GlobalDeletionQueue.PushFunction([=]() {
			vkDestroyPipeline(GraphicsContext::GetDevice(), m_Pipeline, nullptr);
			vkDestroyPipelineLayout(GraphicsContext::GetDevice(), m_PipelineLayout, nullptr);
		});
	}

	void Pipeline::Bind(Ref<CommandBuffer> commandBuffer)
	{
		vkCmdBindPipeline(commandBuffer->GetHandle(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline);
	}

	Ref<Pipeline> Pipeline::Create(PipelineBuilder& builder, Ref<RenderPass> renderPass)
	{
		return CreateRef<Pipeline>(builder, renderPass);
	}
}
