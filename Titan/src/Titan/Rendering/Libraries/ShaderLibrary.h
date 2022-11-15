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
		std::string spvAssembly;
		ShaderType shaderType = ShaderType::Count;
	};

	class ShaderLibrary
	{
	public:
		static Shader& Get(const std::filesystem::path& path);
	private:
		static Shader Compile(const std::filesystem::path& path);
		static void DumpBinary(const std::filesystem::path& dest, Shader& shader);
		static std::vector<char> ReadBinary(const std::filesystem::path& binPath);
		inline static std::unordered_map<std::filesystem::path, Shader> s_Library;
	};
}