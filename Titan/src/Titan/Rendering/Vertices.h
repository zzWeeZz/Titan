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
	glm::vec3 Position = {};
	glm::u8 Normal[2];
	glm::u8 Tangent[2];
	glm::vec4 Color = {};
	glm::vec2 TexCoords = {};
	glm::vec2 padding = {};
};