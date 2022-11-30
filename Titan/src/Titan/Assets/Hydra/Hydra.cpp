#include "TNpch.h"
#include "Hydra.h"


#include "Titan/Assets/Importers/ModelImporters/FBXImporter.h"
#include "Titan/Assets/Importers/ModelImporters/GLTFImporter.h"

#include "Titan/Assets/Model/Submesh.h"

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
		}
	}
}