#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "Serializer.hpp"
#include "Snowflake.hpp"
#include "String.hpp"
#include "Titan/Core/TitanMemory.h"
#include "Titan/Scene/PropertyRegistry.h"

COMPONENT(TagComponent)
{
	REGISTER_COMPONENT("{0A07B7F9-8312-40B1-81AA-773BE3B295B9}"_guid);
	Snowflake::String tag;
};


COMPONENT(TransformComponent)
{
	REGISTER_COMPONENT("{7B78818B-FBCF-4359-B016-8C8DF1D7F642}"_guid);
	glm::vec3 position = { 0,0,0 };
	glm::vec3 rotation = { 0,0,0 };
	glm::vec3 scale = { 1, 1, 1 };
	glm::mat4 matrix{ 1 };
	glm::mat4 prevMatrix{ 2 };
	bool isDirty = true;
};

COMPONENT(LightComponent)
{
	REGISTER_COMPONENT("{4394346B-7D2F-4DDF-9BFF-B45FFB9DE175}"_guid);
	glm::vec3 color = { 1,1,1 };
	float intensity = 1.f;
};