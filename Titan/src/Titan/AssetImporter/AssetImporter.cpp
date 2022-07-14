#include "AssetImporter.h"
#include "ModelImporters/GLTFImporter.h"
#include "Titan/Core/Core.h"

namespace Titan
{
	void AssetImporter::ImportModel(const std::filesystem::path& path, Ref<VertexArray>& vertexArray)
	{
		if (!std::filesystem::exists(path))
		{
			TN_CORE_ERROR("AssetImporter::ImportModel: File does not exist: {0}", path.string());
			return;
		}

		if (path.extension() == ".obj")
		{
			TN_CORE_ERROR("AssetImporter::ImportModel: File is not an .obj file: {0}", path.string());
		}
		else if (path.extension() == ".gltf" || path.extension() == ".glb")
		{
			if (CheckMeshRegistry(path.filename().string(), vertexArray))
			{
				return;
			}
			GLTFImporter::Import(path.string(), vertexArray);
			s_MeshRegistry[path.filename().string()] = &vertexArray;
		}
		else if (path.extension() == ".fbx")
		{
			TN_CORE_ERROR("AssetImporter::ImportModel: File is not an .fbx file: {0}", path.string());
		}
	}

	void AssetImporter::Shutdown()
	{
		if (s_MeshRegistry.size() > 1)
		{
			for (auto it : s_MeshRegistry)
			{
				it.second = nullptr;
			}
		}
		else
			s_MeshRegistry.begin()->second = nullptr;
	}

	bool AssetImporter::CheckMeshRegistry(const std::string& key, Ref<VertexArray>& outVertex)
	{
		if (s_MeshRegistry.contains(key))
		{
			const auto& registryVArray = *s_MeshRegistry[key];
			outVertex = VertexArray::Create(registryVArray->GetVertexArray(), registryVArray->GetIndexArray());
			TN_CORE_INFO("Pulling {0} from registry", key);
			return true;
		}
		TN_CORE_INFO("could not find {0} in registry, creating a new one.", key);
		return false;
	}
}
