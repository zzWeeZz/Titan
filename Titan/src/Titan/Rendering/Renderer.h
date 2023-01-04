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
		bool dirty = false;
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
		static void WaitOnNewFrame();
		static void Begin();
		static void Shutdown();
	private:
		static bool ValidateBindlessBuffers();
		static bool ValidateBindlessTextures();
		static void CombineMeshlets(VkCommandBuffer& cmd);
		static void CombineVertexBuffers(VkCommandBuffer& cmd);
		static void CombineTriangleBuffers(VkCommandBuffer& cmd);
		static void CombineVertexIndexBuffers(VkCommandBuffer& cmd);
		static void UpdateMeshBuffer();
		static void CreateMeshletBuffer();
		static void CreateVertexBuffer();
		static void CreateTriangleBuffer();
		static void CreateVertexIndexBuffer();
		static void CreateMeshBuffer();
	};
}
