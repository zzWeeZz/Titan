#pragma once
#include <d3d12.h>
#include "Titan/Core/TitanMemory.h"
#include "Titan/Core/TitanFormats.h"
namespace Titan
{
	struct GraphicsPipelineInfo
	{
		std::filesystem::path vsPath = "";
		std::filesystem::path psPath = "";

		Topology topology;
		std::vector<ImageFormat> imageFormats;
	};
	class GraphicsPipeline
	{
	public:
		GraphicsPipeline(const GraphicsPipelineInfo& info);

		VkPipeline& GetHandle() { return m_Pipeline; }
		VkPipelineLayout& GetLayout() { return m_PipelineLayout; }

		void Bind(VkCommandBuffer& cmd);

		VkDescriptorSetLayout& DescLayout() { return m_DescriptorSetLayout; }
		static Ref<GraphicsPipeline> Create(const GraphicsPipelineInfo& info);
	private:
		VkShaderModule CreateShaderModule(std::vector<uint32_t> assembly);
		void CreateDescriptorSetLayout();
		VkDescriptorSetLayout m_DescriptorSetLayout;
		VkPipelineLayout m_PipelineLayout;
		VkPipeline m_Pipeline;
	};
}
