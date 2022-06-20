#pragma once
#include <vulkan/vulkan.h>

#include "Titan/Core/Core.h"

namespace Titan
{

	enum class ShaderType
	{
		Vertex = BIT(0),
		Fragment = BIT(1),
		Geometry = BIT(2),
		Compute = BIT(3),
		TessellationControl = BIT(4),
		TessellationEvaluation = BIT(5),
		Count = BIT(6)
	};


	class Shader
	{
		friend class PipelineBuilder;
	public:
		Shader(const std::string& filepath, ShaderType type);

		void Bind();
		static Ref<Shader> Create(const std::string& filepath, ShaderType type);
	private:
		VkPipelineShaderStageCreateInfo GetCreateInfo();

		VkShaderModule m_ShaderModule;
		ShaderType m_Type;
	};
}