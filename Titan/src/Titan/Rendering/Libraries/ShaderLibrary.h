#pragma once
#include <unordered_map>
#include <string>
#include <filesystem>
#include "Titan/Assets/Shaders/Shader.h"
#include <vulkan/vulkan_core.h>

struct SpvReflectShaderModule;

namespace Titan
{
	class ShaderLibrary
	{
	public:
		static Shader& Get(const std::filesystem::path& path);
	private:
		inline static std::unordered_map<std::filesystem::path, Shader> s_Library;
	};
}