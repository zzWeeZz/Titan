#pragma once
#include "Titan/Rendering/Buffers/VertexBuffer.h"
#include "Titan/Rendering/Buffers/IndexBuffer.h"
namespace Titan
{

	struct VertexPackage
	{
		Ref<VertexBuffer> vertexBuffer;
		Ref<IndexBuffer> indexBuffer;
	};
}