#pragma once

#include <vector>
#include <filesystem>

namespace Titan
{
	class Submesh;
	struct Meshlet;
	class Shader;

	class Hydra
	{
	public:
		static void Initialize();
		static void ImportModel(const std::filesystem::path& filepath, std::vector<Submesh>& submeshes);
		static void ImportShader(const std::filesystem::path& filepath, Shader& shader);
	private:
		static std::vector<Submesh> InternalImportModel(std::filesystem::path filepath, std::vector<Submesh>& submeshes);
		static Shader InternalImportShader(std::filesystem::path filepath, Shader& shader);
		static void GenerateMeshlets(Submesh& submesh);
	};
}
