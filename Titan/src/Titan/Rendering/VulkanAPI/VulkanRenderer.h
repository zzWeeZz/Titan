#pragma once
#include <vulkan/vulkan.h>
#include "CommandBuffer.h"
#include "RenderPass.h"
#include "ShaderClass/Shader.h"

namespace Titan
{
	class VulkanRenderer
	{
	public:
		static void Initialize();
		static void Begin();
		static void Shutdown();
	private:
		inline static Ref<CommandBuffer> m_CommandBuffer;
		inline static Ref<RenderPass> m_RenderPass;

		inline static Ref<Shader> m_Shader;
	};
}
