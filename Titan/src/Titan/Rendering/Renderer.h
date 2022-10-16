#pragma once

#include <d3d12.h>
#include "glm/glm.hpp"
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

		static void Initialize();
		static void Begin();
		static void End();
		// this will draw command buffers; DO NOT USE THIS TO RENDER SOMETHING, This is called internal. 
		static void DrawCommands();
		static void Shutdown();
	private:
		static void CopyResource(ID3D12Resource* dest, ID3D12Resource* source);
		static void InitializePipelines();
	};
}
