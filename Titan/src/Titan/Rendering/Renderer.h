#pragma once

#include "glm/glm.hpp"
#include <vulkan/vulkan.h>
#include "Titan/Rendering/Framebuffer.h"
namespace Titan
{
	class Submesh;
	struct CameraCmd
	{
		glm::mat4 view;
		glm::mat4 proj;
	};

	struct MeshCmd
	{
		Submesh* submesh;
		glm::mat4 transform;
		TitanID textureId;
	};

	struct LightCmd
	{
		glm::vec4 direction;
		glm::vec4 color;
	};

	class Renderer
	{
	public:
		static void SetDebugLayer(const uint32_t& layer);

		static void Submit(const CameraCmd& cameraCmd);
		static void Submit(const MeshCmd& meshCmd);
		static void Submit(const LightCmd& lightCmd);
		static Ref<Framebuffer> GetMainFramebuffer();
		static void Initialize();
		static void NewFrame();
		static void Begin();
		static void Shutdown();
	private:
		static void CombineMeshlets(VkCommandBuffer& cmd);
		static void CreateMeshletBuffer();
	};
}
