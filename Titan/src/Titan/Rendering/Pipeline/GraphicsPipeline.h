#pragma once
#include "Titan/Core/TitanMemory.h"
#include "Titan/Core/TitanFormats.h"
#include <filesystem>
namespace Titan
{
	struct GraphicsPipelineInfo
	{
		std::filesystem::path vsPath = "";
		std::filesystem::path tsPath = "";
		std::filesystem::path msPath = "";
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
		VkDescriptorSetLayout m_DescriptorSetLayout;
		VkPipelineLayout m_PipelineLayout;
		VkPipeline m_Pipeline;
	};
}
