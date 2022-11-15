#pragma once
#include "glm/glm.hpp"

namespace Titan
{
	struct CameraData
	{
		glm::mat4 view;
		glm::mat4 proj;
	};

	struct ModelData
	{
		glm::mat4 mdlSpace;
	};
}