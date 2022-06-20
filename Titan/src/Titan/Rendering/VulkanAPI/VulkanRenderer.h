#pragma once
#include <vulkan/vulkan.h>
#include "CommandBuffer.h"
#include "Pipeline.h"
#include "RenderPass.h"
#include "Buffers/PushConstant.h"
#include "ShaderClass/Shader.h"
#include "Buffers/Buffers.h"

namespace Titan
{
	
	class VulkanRenderer
	{
	public:
		static void Initialize();
		static void Begin();
		static void Shutdown();
	private:
		static void CreateTrianglePipeline();

		inline static Ref<CommandBuffer> m_CommandBuffer;
		inline static Ref<RenderPass> m_RenderPass;
		inline static Ref<Pipeline> m_TrianglePipeline;
		inline static Ref<Shader> m_FragShader;
		inline static Ref<Shader> m_VertShader;
		inline static Ref<PushConstant<MeshConstant>> m_PushConstant;
	};
}
