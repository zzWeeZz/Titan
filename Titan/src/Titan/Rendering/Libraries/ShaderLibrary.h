#pragma once
#include <unordered_map>
#include <string>
#include <filesystem>
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
		Count
	};
	struct Shader
	{
		std::string shaderFile = "";
		std::vector<uint32_t> spvBinary;
		ShaderType shaderType = ShaderType::Count;
	};

	class ShaderLibrary
	{
	public:
		static Shader& Get(const std::filesystem::path& path);
	private:
		static Shader Compile(const std::filesystem::path& path);
		inline static std::unordered_map<std::filesystem::path, Shader> s_Library;
	};
}