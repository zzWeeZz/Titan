#pragma once
#include "Buffers.h"
#include "glm/ext/scalar_uint_sized.hpp"
#include "Titan/Core/Core.h"
#include "Titan/Rendering/VulkanAPI/CommandBuffer.h"
#include "Titan/Rendering/VulkanAPI/GraphicsContext.h"

namespace Titan
{
	class IndexBuffer
	{
	public:
		IndexBuffer(std::vector<uint16_t>& indices);

		void Bind(Ref<CommandBuffer> cmdBuffer);

		static Ref<IndexBuffer> Create(std::vector<uint16_t>& vertices);
	private:
		AllocatedBuffer m_VertexBuffer{};
	};

	inline IndexBuffer::IndexBuffer(std::vector<uint16_t>& indices)
	{
		const auto& allocator = GraphicsContext::GetAllocator();
		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = indices.size() * sizeof(uint16_t);
		bufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;


		VmaAllocationCreateInfo vmaallocInfo = {};
		vmaallocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

		//allocate the buffer
		TN_VK_CHECK(vmaCreateBuffer(allocator, &bufferInfo, &vmaallocInfo,
			&m_VertexBuffer.Buffer,
			&m_VertexBuffer.Allocation,
			nullptr));

		GlobalDeletionQueue.PushFunction([=]()
			{
				vmaDestroyBuffer(allocator, m_VertexBuffer.Buffer, m_VertexBuffer.Allocation);
			});

		void* data;

		vmaMapMemory(allocator, m_VertexBuffer.Allocation, &data);
		memcpy(data, indices.data(), indices.size() * sizeof(uint16_t));
		vmaUnmapMemory(allocator, m_VertexBuffer.Allocation);
	}

	inline void IndexBuffer::Bind(Ref<CommandBuffer> cmdBuffer)
	{
		VkDeviceSize offset = 0;
		vkCmdBindIndexBuffer(cmdBuffer->GetHandle(), m_VertexBuffer.Buffer, offset, VK_INDEX_TYPE_UINT16);
	}

	inline Ref<IndexBuffer> IndexBuffer::Create(std::vector<uint16_t>& vertices)
	{
		return CreateRef<IndexBuffer>(vertices);
	}
}
