#include "TNpch.h"
#include "Hydra.h"


#include "Titan/Assets/Importers/ModelImporters/FBXImporter.h"
#include "Titan/Assets/Importers/ModelImporters/GLTFImporter.h"

#include "Titan/Assets/Model/Submesh.h"
#include <execution>

namespace Titan
{
	void Hydra::ImportModel(const std::filesystem::path& filepath, std::vector<Submesh>& submeshes)
	{
		if (!std::filesystem::exists(filepath))
		{
			TN_CORE_ERROR("(Hydra::ImportModel) Could not find file {0}.", filepath.string());
			return;
		}
		const auto extension = filepath.extension();
		if (extension == ".fbx")
		{
			FBXImporter::Import(filepath, submeshes);
		}
		else if (extension == ".gltf" || extension == ".glb")
		{
			GLTFImporter::Import(filepath, submeshes);
		}
		else
		{
			TN_CORE_ERROR("(Hydra::ImportModel) Extension: [{0}] is not supported model format!");
			return;
		}
		GenerateMeshlets(submeshes[0]);
	}
	void Hydra::GenerateMeshlets(Submesh& submesh)
	{
		size_t indicesCount = 0;
		size_t iterations = 1;
		auto& meshlet = submesh.GetMeshlets().emplace_back();
		size_t verticesCount = 0;
		while (indicesCount < 378 * iterations)
		{
			meshlet.indices[indicesCount] = submesh.GetIndices()[indicesCount];
			indicesCount++;
			meshlet.indexCount = static_cast<uint32_t>(indicesCount);
		}
		
		

		TN_CORE_TRACE("generate meshlet");
	}
}