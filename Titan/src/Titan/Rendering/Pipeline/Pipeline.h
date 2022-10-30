#pragma once
#include <d3d12.h>
#include "Titan/Core/TitanMemory.h"
#include "Titan/Core/TitanFormats.h"
namespace Titan
{
	struct PipelineInfo
	{
		std::filesystem::path vsPath = "";
		std::filesystem::path psPath = "";

		DepthState depthState;
		CullState depthCullState;
	};
	class Pipeline
	{
	public:
		Pipeline(const PipelineInfo& info);

		VkPipeline& GetHandle() { return m_Pipeline; }
		VkPipelineLayout& GetLayout() { return m_PipelineLayout; }
		VkDescriptorSetLayout& DescLayout() { return m_DescriptorSetLayout; }
		static Ref<Pipeline> Create(const PipelineInfo& info);
	private:
		VkShaderModule CreateShaderModule(std::vector<uint32_t> assembly);
		void CreateDescriptorSetLayout();
		VkDescriptorSetLayout m_DescriptorSetLayout;
		VkPipelineLayout m_PipelineLayout;
		VkPipeline m_Pipeline;
	};
}
