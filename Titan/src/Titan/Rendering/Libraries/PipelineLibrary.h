#pragma once
#include "Titan/Rendering/Pipeline/GraphicsPipeline.h"
namespace Titan
{
	class PipelineLibrary
	{
	public:
		static Ref<GraphicsPipeline> Get(const std::string& key);
		static void BindPipline(const std::string& key, VkCommandBuffer& cmd);
		static void Add(const std::string& key, GraphicsPipelineInfo& info);
	private:
		inline static std::unordered_map<std::string, Ref<GraphicsPipeline>> s_GraphicsLibrary;
	};
}