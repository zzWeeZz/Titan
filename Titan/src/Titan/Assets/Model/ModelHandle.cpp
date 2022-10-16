#include "TNpch.h"
#include "ModelHandle.h"
#include "Titan/Assets/ModelImporters/GLTFImporter.h"
namespace Titan
{
	void Titan::ModelHandle::Initialize(const std::filesystem::path& path)
	{
		SetPath(path);
		GLTFImporter::Import(path, m_VertexPackage);
	}
}