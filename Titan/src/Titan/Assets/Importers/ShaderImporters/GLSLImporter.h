#pragma once
#include "Titan/Assets/Shaders/Shader.h"
#include <vulkan/vulkan_core.h>

struct SpvReflectShaderModule;
namespace Titan
{
	class GLSLImporter
	{
	public:
		static void Import(const std::filesystem::path& path, Shader& shader);
	private:
		static Shader Compile(const std::filesystem::path& path);
	};
}
