#pragma once

#include <vector>
#include <filesystem>

namespace Titan
{
	class Submesh;
	class Hydra
	{
	public:
		static void ImportModel(const std::filesystem::path& filepath, std::vector<Submesh>& submeshes);
	};
}
