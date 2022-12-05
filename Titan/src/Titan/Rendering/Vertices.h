#pragma once
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
struct VertexInputDescription
{
	std::vector<VkVertexInputAttributeDescription> Attributes;
	std::vector<VkVertexInputBindingDescription> Bindings;

	VkPipelineVertexInputStateCreateFlags Flags = 0;
};


struct Vertex
{
	glm::vec3 Position = {};
	glm::vec3 Color = {};
	glm::vec3 Normal = {};
	glm::vec3 Tangent = {};
	glm::vec2 TexCoords = {};

	static VertexInputDescription GetBindingDesc()
	{
		VertexInputDescription inputDescription{};
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		inputDescription.Bindings.push_back(bindingDescription);

		//Position will be stored at Location 0
		VkVertexInputAttributeDescription positionAttribute = {};
		positionAttribute.binding = 0;
		positionAttribute.location = 0;
		positionAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
		positionAttribute.offset = offsetof(Vertex, Position);

		//Normal will be stored at Location 1
		VkVertexInputAttributeDescription colorAttribute = {};
		colorAttribute.binding = 0;
		colorAttribute.location = 1;
		colorAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
		colorAttribute.offset = offsetof(Vertex, Color);

		//Normal will be stored at Location 2
		VkVertexInputAttributeDescription normalAttribute = {};
		normalAttribute.binding = 0;
		normalAttribute.location = 2;
		normalAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
		normalAttribute.offset = offsetof(Vertex, Normal);

		VkVertexInputAttributeDescription tangentAttribute = {};
		tangentAttribute.binding = 0;
		tangentAttribute.location = 3;
		tangentAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
		tangentAttribute.offset = offsetof(Vertex, Tangent);

		//Color will be stored at Location 3
		VkVertexInputAttributeDescription texCoordAttribute = {};
		texCoordAttribute.binding = 0;
		texCoordAttribute.location = 4;
		texCoordAttribute.format = VK_FORMAT_R32G32_SFLOAT;
		texCoordAttribute.offset = offsetof(Vertex, TexCoords);

		inputDescription.Attributes.push_back(positionAttribute);
		inputDescription.Attributes.push_back(colorAttribute);
		inputDescription.Attributes.push_back(normalAttribute);
		inputDescription.Attributes.push_back(tangentAttribute);
		inputDescription.Attributes.push_back(texCoordAttribute);

		return inputDescription;
	}
};