#include "TNpch.h"
#include "DescriptorAllocator.h"

#include "Titan/Rendering/GraphicsContext.h"
namespace Titan
{
	void DescriptorAllocator::Initialize()
	{
	}
	bool DescriptorAllocator::Allocate(VkDescriptorSet* set, VkDescriptorSetLayout layout)
	{
		auto device = GraphicsContext::GetDevice();
		
		if (m_CurrentPool == VK_NULL_HANDLE)
		{
			m_CurrentPool = GrabPool();
			m_UsedPools.push_back(m_CurrentPool);
		}

		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.pNext = nullptr;

		allocInfo.pSetLayouts = &layout;
		allocInfo.descriptorPool = m_CurrentPool;
		allocInfo.descriptorSetCount = 1;

		auto allocResult = vkAllocateDescriptorSets(device.GetHandle(), &allocInfo, set);

		bool needReAlloc = false;

		switch (allocResult)
		{
		case VK_SUCCESS:
			return true;
			break;
		case VK_ERROR_FRAGMENTED_POOL:
		case VK_ERROR_OUT_OF_POOL_MEMORY:
			needReAlloc = true;
			break;
		default:
			TN_CORE_ERROR("(DescriptorAllocator::Allocate) Some thing went really wrong when allocating descriptor set!");
			return false;
		}

		if (needReAlloc)
		{
			TN_CORE_INFO("(DescriptorAllocator::Allocate) Need to reallocate descriptor set.");
			m_CurrentPool = GrabPool();
			m_UsedPools.push_back(m_CurrentPool);

			allocResult = vkAllocateDescriptorSets(device.GetHandle(), &allocInfo, set);

			if (allocResult == VK_SUCCESS)
			{
				TN_CORE_INFO("(DescriptorAllocator::Allocate) Reallocation was a success.");
				return true;
			}
		}
		TN_CORE_ERROR("(DescriptorAllocator::Allocate) Some thing went really wrong when allocating descriptor set!");
		return false;
	}
	void DescriptorAllocator::ResetPools()
	{
		auto device = GraphicsContext::GetDevice();
		for (auto pool : m_UsedPools)
		{
			vkResetDescriptorPool(device.GetHandle(), pool, 0);
			m_FreePools.push_back(pool);
		}

		m_UsedPools.clear();

		m_CurrentPool = VK_NULL_HANDLE;
	}
	void DescriptorAllocator::Shutdown()
	{
		auto device = GraphicsContext::GetDevice();
		for (auto pool : m_FreePools)
		{
			vkDestroyDescriptorPool(device.GetHandle(), pool, nullptr);
		}
		for (auto pool : m_UsedPools)
		{
			vkDestroyDescriptorPool(device.GetHandle(), pool, nullptr);
		}
	}
	VkDescriptorPool DescriptorAllocator::GrabPool()
	{
		if (!m_FreePools.empty())
		{
			VkDescriptorPool pool = m_FreePools.back();
			m_FreePools.pop_back();
			return pool;
		}
		else
		{
			return CreatePool(1000, 0);
		}
		return VkDescriptorPool();
	}
	VkDescriptorPool DescriptorAllocator::CreatePool(int32_t count, VkDescriptorPoolCreateFlags flags)
	{
		std::vector<VkDescriptorPoolSize> sizes;
		sizes.reserve(m_DescriptorSizes.sizes.size());
		for (auto sz : m_DescriptorSizes.sizes)
		{
			sizes.emplace_back(sz.first, static_cast<uint32_t>(sz.second * static_cast<float>(count)));
		}

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.flags = flags;
		poolInfo.maxSets = count;
		poolInfo.poolSizeCount = static_cast<uint32_t>(sizes.size());
		poolInfo.pPoolSizes = sizes.data();

		VkDescriptorPool descPool;
		auto device = GraphicsContext::GetDevice();
		vkCreateDescriptorPool(device.GetHandle(), &poolInfo, nullptr, &descPool);

		return descPool;
	}
}