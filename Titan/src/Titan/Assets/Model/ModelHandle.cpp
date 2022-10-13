#include "TNpch.h"
#include "ModelHandle.h"
namespace Titan
{
	inline void Titan::ModelHandle::Initialize(const std::filesystem::path& path)
	{
		SetPath(path);
	}
}