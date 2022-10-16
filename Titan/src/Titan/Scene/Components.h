#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "Serializer.hpp"
#include "Snowflake.hpp"
#include "Titan/Core/TitanMemory.h"

COMPONENT(TagComponent)
{
	REGISTER_COMPONENT("{0A07B7F9-8312-40B1-81AA-773BE3B295B9}"_guid);
	char tag[512];
};

COMPONENT(TransformComponent)
{
	REGISTER_COMPONENT("{7B78818B-FBCF-4359-B016-8C8DF1D7F642}"_guid);
	glm::vec3 position = { 0,0,0 };
	glm::quat quaternion = {1,0,0,0};
	glm::vec3 scale = { 1, 1, 1 };
};