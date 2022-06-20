#include "AssetImporter.h"
#include "ModelImporters/GLTFImporter.h"
#include "Titan/Core/Core.h"

namespace Titan
{
	void AssetImporter::ImportModel(const std::filesystem::path& path, std::vector<Vertex>& outVertex)
	{
		if (!std::filesystem::exists(path))
		{
			TN_CORE_ERROR("AssetImporter::ImportModel: File does not exist: {0}", path.string());
			return;
		}

		if (path.extension() == ".obj")
		{
			TN_CORE_ERROR("AssetImporter::ImportModel: File is not an .obj file: {0}", path.string());
			return;
		}

		if (path.extension() == ".gltf")
		{
			GLTFImporter::Import(path.string(), outVertex);
			return;
		}
		
	}
}
