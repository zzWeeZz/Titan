#pragma once

#include "glm/glm.hpp"
#include <vulkan/vulkan.h>
#include "Titan/Rendering/Buffers/VertexPacket.h"
#include "Titan/Rendering/Framebuffer.h"
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
		TitanID textureId;
	};

	class Renderer
	{
	public:
		static void Submit(const CameraCmd& cameraCmd);
		static void Submit(const MeshCmd& meshCmd);
		static Ref<Framebuffer> GetMainFramebuffer();
		static void Initialize();
		static void Begin();
		static void Shutdown();
	private:
		static void FreeExternalDescriptorSets();
	};
}
