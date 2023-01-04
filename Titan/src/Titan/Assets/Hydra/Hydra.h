#pragma once

#include <vector>
#include <filesystem>

namespace Titan
{
	class Submesh;
	struct Meshlet;
	class Hydra
	{
	public:
		static void ImportModel(const std::filesystem::path& filepath, std::vector<Submesh>& submeshes);
	private:
		static std::vector<Submesh> InternalImportModel(std::filesystem::path filepath, std::vector<Submesh> submeshes);
		static void GenerateMeshlets(Submesh& submesh);
	};
}
