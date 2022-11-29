#include "TNpch.h"
#include "ModelHandle.h"
#include "Titan/Assets/Importers/ModelImporters/GLTFImporter.h"
#include "Titan/Assets/Model/Submesh.h"

namespace Titan
{
	void Titan::ModelHandle::Initialize(const std::filesystem::path& path)
	{
		SetPath(path);
		GLTFImporter::Import(path, m_Submeshes);
	}
}