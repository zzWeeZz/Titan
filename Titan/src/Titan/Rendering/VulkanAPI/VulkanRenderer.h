#pragma once
#include <vulkan/vulkan.h>

#include "CommandBuffer.h"
#include "Pipeline.h"
#include "RenderPass.h"
#include "Buffers/PushConstant.h"
#include "ShaderClass/Shader.h"
#include "Buffers/Buffers.h"
#include "Titan/Assets/Camera/Camera.h"
#include "Titan/Assets/Textures/Texture.h"

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
		static void DebugImGuiStuff();
		static void AssignCameraEntity(Snowflake::Entity entity) { m_ActiveCamera = entity; }
		static void Begin();
		static Ref<RenderPass> GetRenderPass() { return m_RenderPass; }
		static VkDescriptorPool GetDescriptorPool() { return m_ImGuiDescriptorPool; }
		static void Shutdown();
	private:
		static void CreateTrianglePipeline();
		static void InitializeDescriptors();
		inline static Ref<Texture> m_Texture;
		inline static Snowflake::Entity m_ActiveCamera;
		inline static std::vector<VkDescriptorPool> m_ImguiStuff;
		inline static VkSampler m_Sampler;
		inline static VkDescriptorSetLayout m_SetLayout;
		inline static VkDescriptorSetLayout m_TextureSetLayout;
		inline static VkDescriptorPool m_DescriptorPool;
		inline static VkDescriptorPool m_ImGuiDescriptorPool;
		inline static VkDescriptorSet m_DescriptorSet;
		inline static Ref<CommandBuffer> m_CommandBuffer;
		inline static Ref<CommandBuffer> m_ImGuiCmd;
		inline static Ref<RenderPass> m_RenderPass;
		inline static Ref<Pipeline> m_TrianglePipeline;
		inline static Ref<Shader> m_FragShader;
		inline static Ref<Shader> m_VertShader;
		inline static Ref<PushConstant<MeshConstant>> m_PushConstant;
	};
}
