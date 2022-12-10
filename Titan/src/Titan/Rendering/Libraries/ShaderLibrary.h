#pragma once
#include <unordered_map>
#include <string>
#include <filesystem>

#include <vulkan/vulkan_core.h>

struct SpvReflectShaderModule;

namespace Titan
{
	enum class ShaderType
	{
		Vertex,
		Fragment,
		Compute,
		Geometry,
		TessControl,
		TessEvaluation,
		Task = 26,
		Mesh = 27,
		Count
	};
	struct Shader
	{
		std::string shaderFile = "";
		std::vector<uint32_t> spvBinary;
		std::string spvAssembly;
		ShaderType shaderType = ShaderType::Count;
		VkShaderStageFlags stage = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
		std::unordered_map<uint32_t, std::vector<VkDescriptorSetLayoutBinding>> layouts{};
		VkPushConstantRange pushConstants{};
	};

	class ShaderLibrary
	{
	public:
		static Shader& Get(const std::filesystem::path& path);
	private:
		static Shader Compile(const std::filesystem::path& path);
		static void Reflect(Shader& shader);
		static VkShaderStageFlags GetShaderStage(SpvReflectShaderModule& spvModule);
		static void DumpBinary(const std::filesystem::path& dest, Shader& shader);
		static std::vector<char> ReadBinary(const std::filesystem::path& binPath);
		inline static std::unordered_map<std::filesystem::path, Shader> s_Library;
	};
}