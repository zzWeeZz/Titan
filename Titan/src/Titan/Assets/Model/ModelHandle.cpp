#include "TNpch.h"
#include "ModelHandle.h"
#include "Titan/Assets/Hydra/Hydra.h"
#include "Titan/Assets/Model/Submesh.h"

namespace Titan
{
	void Titan::ModelHandle::Initialize(const std::filesystem::path& path)
	{
		SetPath(path);
		Hydra::ImportModel(path, m_Submeshes);
	}
}