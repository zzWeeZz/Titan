#include "Pipeline.h"

namespace Titan
{
	VkPipeline PipelineBuilder::BuildPipeline(VkDevice device, VkRenderPass renderPass)
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

		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.pNext = nullptr;

		pipelineInfo.stageCount = m_ShaderStages.size();
		pipelineInfo.pStages = m_ShaderStages.data();
		pipelineInfo.pVertexInputState = &m_VertexInputState;
		pipelineInfo.pInputAssemblyState = &m_InputAssemblyState;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &m_Rasterizer;
		pipelineInfo.pMultisampleState = &m_Multisampling;
		pipelineInfo.pColorBlendState = &colorBlendState;
		pipelineInfo.layout = m_PipelineLayout;
		pipelineInfo.renderPass = renderPass;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

		pipelineInfo.pDepthStencilState = &m_DepthStencilState;

		VkPipeline newPipeline;
		if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &newPipeline) != VK_SUCCESS)
		{
			TN_CORE_ERROR("Failed to create graphics pipeline!");
			return VK_NULL_HANDLE;
		}
		else
		{
			return newPipeline;
		}
	}
}
