#pragma once
#include "Buffers.h"
#include "Titan/Core/Core.h"
#include "Titan/Rendering/VulkanAPI/GraphicsContext.h"
namespace Titan
{
	template<class T>
	class VertexBuffer
	{
	public:
		VertexBuffer(std::vector<T>& vertices);

		void Bind(Ref<CommandBuffer> cmdBuffer);

		static Ref<VertexBuffer> Create(std::vector<T>& vertices);
	private:
		AllocatedBuffer m_VertexBuffer;
	};

	template <class T>
	VertexBuffer<T>::VertexBuffer(std::vector<T>& vertices)
	{
		const auto& allocator = GraphicsContext::GetAllocator();
		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = vertices.size() * sizeof(T);
		bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;


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
		memcpy(data, vertices.data(), vertices.size() * sizeof(T));
		vmaUnmapMemory(allocator, m_VertexBuffer.Allocation);
	}

	template <class T>
	void VertexBuffer<T>::Bind(Ref<CommandBuffer> cmdBuffer)
	{
		VkDeviceSize offset = 0;
		vkCmdBindVertexBuffers(cmdBuffer->GetHandle(), 0, 1, &m_VertexBuffer.Buffer, &offset);
	}

	template <class T>
	Ref<VertexBuffer<T>> VertexBuffer<T>::Create(std::vector<T>& vertices)
	{
		return CreateRef<VertexBuffer<T>>(vertices);
	}
}
