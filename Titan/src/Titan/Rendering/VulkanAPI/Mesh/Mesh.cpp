#include "Mesh.h"

#include "Titan/AssetImporter/AssetImporter.h"

namespace Titan
{
	Mesh::Mesh(const std::filesystem::path& filePath)
	{
		AssetImporter::ImportModel(filePath, m_VertexArray);
	}

	Ref<Mesh> Mesh::Create(const std::filesystem::path& filePath)
	{
		return CreateRef<Mesh>(filePath);
	}
}


