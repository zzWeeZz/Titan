#pragma once
#include <filesystem>

#include "Titan/Core/TitanMemory.h"
#include "Titan/Rendering/VulkanAPI/Buffers/VertexArray.h"

namespace Titan
{
	class Mesh
	{
		friend class VulkanRenderer;
	public:
		Mesh(const std::filesystem::path& filePath);
		static Ref<Mesh> Create(const std::filesystem::path& filePath);
	private:
		Ref<VertexArray> m_VertexArray;
	};
}
