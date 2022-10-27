#pragma once

#include "glm/glm.hpp"
#include <vulkan/vulkan.h>
#include "Titan/Rendering/Buffers/VertexPacket.h"
namespace Titan
{
	struct CameraCmd
	{
		glm::mat4 view;
		glm::mat4 proj;
	};

	struct MeshCmd
	{
		VertexPackage package;
		glm::mat4 transform;
	};

	class Renderer
	{
	public:
		static void Submit(const CameraCmd& cameraCmd);
		static void Submit(const MeshCmd& meshCmd);
		static VkDescriptorSet& AllocateDescriptorSet(VkDescriptorSetLayout& layout);
		static void Initialize();
		static void Begin();
		static void Shutdown();
	private:
		static void FreeExternalDescriptorSets();
	};
}
