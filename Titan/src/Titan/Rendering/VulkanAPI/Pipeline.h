#pragma once
#include "Titan/Core/Core.h"
#include <vulkan/vulkan.h>
namespace Titan
{
	class Pipeline
	{
	};

	class PipelineBuilder
	{
	public:
		std::vector<VkPipelineShaderStageCreateInfo> m_ShaderStages;
		VkPipelineVertexInputStateCreateInfo m_VertexInputState;
		VkPipelineInputAssemblyStateCreateInfo m_InputAssemblyState;
		VkViewport m_Viewport;
		VkRect2D m_Scissor;
		VkPipelineRasterizationStateCreateInfo m_Rasterizer;
		VkPipelineColorBlendAttachmentState m_ColorBlendAttachmentState;
		VkPipelineMultisampleStateCreateInfo m_Multisampling;
		VkPipelineLayout m_PipelineLayout;
		VkPipelineDepthStencilStateCreateInfo m_DepthStencilState;

		VkPipeline BuildPipeline(VkDevice device, VkRenderPass renderPass);
	};
}