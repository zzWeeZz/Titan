#include "TNpch.h"
#include "ShaderLibrary.h"
#include "shaderc/shaderc.hpp"
#include "SPIRV-Reflect/spirv_reflect.h"
#include "Titan/Utils/FilesystemUtils.h"
#include "Titan/Assets/Hydra/Hydra.h"
namespace Titan
{
	Shader& ShaderLibrary::Get(const std::filesystem::path& path)
	{
		if (s_Library.contains(path.filename()))
		{
			return s_Library[path.filename()];
		}


		Shader shader{};
		Hydra::ImportShader(path, shader);
		s_Library[path.filename()] = shader;
		return s_Library[path.filename()];
	}
}