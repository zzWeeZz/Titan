#pragma once
#include "Titan/Core/Core.h"
#include <vulkan/vulkan.h>

#include "CommandBuffer.h"
#include "RenderPass.h"
#include "ShaderClass/Shader.h"

namespace Titan
{
	class PipelineBuilder
	{
	public:
		std::vector<Ref<Shader>> m_ShaderStages;
		VkPipelineVertexInputStateCreateInfo m_VertexInputState;
		VkPipelineInputAssemblyStateCreateInfo m_InputAssemblyState;
		VkViewport m_Viewport;
		VkRect2D m_Scissor;
		VkPipelineRasterizationStateCreateInfo m_Rasterizer;
		VkPipelineColorBlendAttachmentState m_ColorBlendAttachmentState;
		VkPipelineMultisampleStateCreateInfo m_Multisampling;
		VkPipelineLayout m_PipelineLayout;
		VkPipelineDepthStencilStateCreateInfo m_DepthStencilState;
		std::vector<VkDescriptorSetLayout> m_DescriptorSetLayouts;
		VkPipeline BuildPipeline(VkRenderPass renderPass);
	};

	class Pipeline
	{
	public:
		Pipeline(PipelineBuilder& builder, Ref<RenderPass> renderPass);
		void Bind(Ref<CommandBuffer> commandBuffer);
		void UnBind();
		VkPipelineLayout& GetPipelineLayout() { return m_PipelineLayout; }
		static Ref<Pipeline> Create(PipelineBuilder& builder, Ref<RenderPass> renderPass);
	private:
		VkPipeline m_Pipeline;
		VkPipelineLayout m_PipelineLayout;
	};

	
}
