#pragma once
#include <glm/glm.hpp>

struct RawVertex
{
	glm::vec3 Position = {};
	glm::vec3 Color = {};
	glm::vec3 Normal = {};
	glm::vec3 Tangent = {};
	glm::vec2 TexCoords = {};
};

struct BufferVertex
{
	glm::vec4 Position = {};
	glm::vec4 Color = {};
	glm::vec4 Normal = {};
	glm::vec4 Tangent = {};
	glm::vec2 TexCoords = {};
	glm::vec2 padding = {};
};