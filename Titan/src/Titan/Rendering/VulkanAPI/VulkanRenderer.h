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
	class Buffer;
	class Model;
	class Mesh;

	struct RenderData
	{
		std::vector<Model*> models;

		Ref<Buffer> cameraBuffer;
	};
	class VulkanRenderer
	{
	public:
		static void Initialize();
		static void SubmitMesh(Ref<Model> mesh);
		static void Begin();
		static Ref<RenderPass> GetRenderPass() { return m_RenderPass; }
		static void Shutdown();
	private:
		static void CreateTrianglePipeline();
		static void InitializeDescriptors();
		inline static VkDescriptorSetLayout m_SetLayout;
		inline static VkDescriptorPool m_DescriptorPool;
		inline static VkDescriptorSet m_DescriptorSet;
		inline static Ref<CommandBuffer> m_CommandBuffer;
		inline static Ref<RenderPass> m_RenderPass;
		inline static Ref<Pipeline> m_TrianglePipeline;
		inline static Ref<Shader> m_FragShader;
		inline static Ref<Shader> m_VertShader;
		inline static Ref<PushConstant<MeshConstant>> m_PushConstant;
	};
}
