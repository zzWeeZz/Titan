#pragma once
#include <vulkan/vulkan.h>

#include "Titan/Core/Core.h"

namespace Titan
{

	enum class ShaderType
	{
		Vertex,
		Fragment,
		Geometry,
		Compute,
		TessellationControl,
		TessellationEvaluation,
		Count
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