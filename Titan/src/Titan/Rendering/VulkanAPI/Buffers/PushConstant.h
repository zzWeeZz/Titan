#pragma once
#include <vulkan/vulkan.h>

#include "Titan/Core/Core.h"
#include "Titan/Rendering/VulkanAPI/Pipeline.h"
#include "Titan/Rendering/VulkanAPI/ShaderClass/Shader.h"

namespace Titan
{
	template<typename T>
	class PushConstant
	{
	public:
		PushConstant();
		inline T& GetData() { return m_Data; }

		void PushToGpu(Ref<CommandBuffer> commandBuffer, Ref<Pipeline> pipeline, VkShaderStageFlagBits type);

		static Ref<PushConstant> Create();
	private:
		T m_Data;
	};

	template <typename T>
	PushConstant<T>::PushConstant() : m_Data(T())
	{
	}

	template <typename T>
	void PushConstant<T>::PushToGpu(Ref<CommandBuffer> commandBuffer, Ref<Pipeline> pipeline, VkShaderStageFlagBits type)
	{
		vkCmdPushConstants(commandBuffer->GetHandle(), pipeline->GetPipelineLayout(), type, 0, sizeof(T), &m_Data);
	}

	template <typename T>
	Ref<PushConstant<T>> PushConstant<T>::Create()
	{
		return CreateRef<PushConstant<T>>();
	}
}
