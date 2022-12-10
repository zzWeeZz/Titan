#pragma once
#include <map>
#include <filesystem>

#include "Titan/Core/TitanMemory.h"
#include "Titan/Core/TitanFormats.h"

namespace Titan
{
	struct Shader;

	struct GraphicsPipelineInfo
	{
		std::filesystem::path tsPath = "";
		std::filesystem::path msPath = "";
		std::filesystem::path fsPath = "";

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

		void GetShaderObjects(std::vector<VkShaderStageFlagBits>& stages, std::vector<Shader>& shaders, const Titan::GraphicsPipelineInfo& info);

		void CombineAndCreateDescriptorlayouts(std::vector<Titan::Shader>& shaders, std::vector<VkDescriptorSetLayout>& descriptorlayouts);

		void CreatePipelineStages(std::vector<VkShaderStageFlagBits>& stages, std::vector<VkPipelineShaderStageCreateInfo>& pipelineStageInfos, std::vector<Shader>& shaders, const Titan::GraphicsPipelineInfo& info);

		void FindStages(const Titan::GraphicsPipelineInfo& info, std::vector<VkShaderStageFlagBits>& stages);

		VkDescriptorSetLayout m_DescriptorSetLayout;
		VkPipelineLayout m_PipelineLayout;
		VkPipeline m_Pipeline;
	};
}
