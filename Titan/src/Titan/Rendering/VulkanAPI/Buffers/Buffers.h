#pragma once
#include <vulkan/vulkan_core.h>
#include <vk_mem_alloc.h>
#include "glm/glm.hpp"

namespace Titan
{
	// VERTEX
	struct VertexInputDescription
	{
		std::vector<VkVertexInputAttributeDescription> Attributes;
		std::vector<VkVertexInputBindingDescription> Bindings;

		VkPipelineVertexInputStateCreateFlags Flags = 0;
	};

	struct Vertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;

		static VertexInputDescription GetInputDescription()
		{
			VertexInputDescription description;

			//we will have just 1 vertex buffer binding, with a per-vertex rate
			VkVertexInputBindingDescription mainBinding = {};
			mainBinding.binding = 0;
			mainBinding.stride = sizeof(Vertex);
			mainBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			description.Bindings.push_back(mainBinding);

			//Position will be stored at Location 0
			VkVertexInputAttributeDescription positionAttribute = {};
			positionAttribute.binding = 0;
			positionAttribute.location = 0;
			positionAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
			positionAttribute.offset = offsetof(Vertex, Position);

			//Normal will be stored at Location 1
			VkVertexInputAttributeDescription normalAttribute = {};
			normalAttribute.binding = 0;
			normalAttribute.location = 1;
			normalAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
			normalAttribute.offset = offsetof(Vertex, Normal);

			//Color will be stored at Location 2
			VkVertexInputAttributeDescription texCoordAttribute = {};
			texCoordAttribute.binding = 0;
			texCoordAttribute.location = 2;
			texCoordAttribute.format = VK_FORMAT_R32G32_SFLOAT;
			texCoordAttribute.offset = offsetof(Vertex, TexCoords);

			description.Attributes.push_back(positionAttribute);
			description.Attributes.push_back(normalAttribute);
			description.Attributes.push_back(texCoordAttribute);
			return description;
		}
	};

	

	// ALLOCATIONS
	struct AllocatedBuffer
	{
		VkBuffer Buffer;
		VmaAllocation Allocation;
	};

	struct AllocatedImage
	{
		VkImage Image;
		VmaAllocation Allocation;
	};

	//TODO: move to its own class
	struct Mesh
	{
		std::vector<Vertex> m_Vertices;
		AllocatedBuffer m_VertexBuffer;
	};

	// PUSH CONSTANT
	struct MeshConstant
	{
		glm::mat4 transform;
	};
}
