#include "TNpch.h"
#include "PipelineLibrary.h"

namespace Titan
{
	Ptr<GraphicsPipeline> PipelineLibrary::Get(const std::string& key)
	{
		TN_CORE_ASSERT(s_GraphicsLibrary.contains(key), "Graphics pipeline key: {0}, does not exist in library!", key);
		return s_GraphicsLibrary[key];
	}
	void PipelineLibrary::BindPipline(const std::string& key, VkCommandBuffer& cmd)
	{
		TN_CORE_ASSERT(s_GraphicsLibrary.contains(key), "Graphics pipeline key: {0}, does not exist in library!", key);
		vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, s_GraphicsLibrary[key]->GetHandle());
	}
	void PipelineLibrary::Add(const std::string& key, GraphicsPipelineInfo& info)
	{
		if (s_GraphicsLibrary.contains(key))
		{
			return;
		}

		s_GraphicsLibrary[key] = GraphicsPipeline::Create(info);
	}
}