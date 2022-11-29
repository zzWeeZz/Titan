#pragma once
#include <Titan/Rendering/Vertices.h>
#include <Titan/Rendering/Buffers/VertexPacket.h>
namespace Titan
{
	class Submesh
	{

	public:
		Submesh(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);
		
		inline VertexPackage& GetVertexPackage() { return m_VertexPackage; }
		inline const TitanID& GetID() { return m_ID; }
	private:
		VertexPackage m_VertexPackage;
		TitanID m_ID; // TODO: this should be some form of structure.
	};
}