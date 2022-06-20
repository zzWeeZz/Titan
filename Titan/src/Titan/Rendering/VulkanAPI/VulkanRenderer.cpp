#include "VulkanRenderer.h"

#include "GraphicsContext.h"
#include "Titan/Application.h"

namespace Titan
{
	void VulkanRenderer::Initialize()
	{
		GraphicsContext::CreateCommandBuffer(m_CommandBuffer);

		{
			RenderPassCreateInfo info{};
			info.width = 1280;
			info.height = 720;
			info.haveDepth = false;
			m_RenderPass = RenderPass::Create(info);
		}
		m_FragShader = Shader::Create("Shaders/triangle.frag.spv", ShaderType::Fragment);
		m_VertShader = Shader::Create("Shaders/triangle.vert.spv", ShaderType::Vertex);
		CreateTrianglePipeline();
	}

	void VulkanRenderer::Begin()
	{
		GraphicsContext::GetSwapChain().WaitOnFences();
		m_CommandBuffer->Reset();
		m_CommandBuffer->Bind();
		VkClearValue clearValue;
		clearValue.color = { {0.3f, 0.3f, 0.3f, 1.f} };

		VkRenderPassBeginInfo rpInfo{};
		rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		rpInfo.renderPass = m_RenderPass->GetRenderPass();
		rpInfo.framebuffer = m_RenderPass->GetFrameBuffer()->GetFrameBuffer(GraphicsContext::GetSwapChain().GetImageCount());
		rpInfo.renderArea.offset = { 0, 0 };
		rpInfo.renderArea.extent = { 1280, 720 };
		rpInfo.clearValueCount = 1;
		rpInfo.pClearValues = &clearValue;
		vkCmdBeginRenderPass(m_CommandBuffer->GetHandle(), &rpInfo, VK_SUBPASS_CONTENTS_INLINE);

		m_TrianglePipeline->Bind(m_CommandBuffer);
		vkCmdDraw(m_CommandBuffer->GetHandle(), 3, 1, 0, 0);

		vkCmdEndRenderPass(m_CommandBuffer->GetHandle());
		TN_VK_CHECK(vkEndCommandBuffer(m_CommandBuffer->GetHandle()));

		std::vector<Ref<CommandBuffer>> commandBuffers;
		commandBuffers.push_back(m_CommandBuffer);
		GraphicsContext::GetSwapChain().Submit(commandBuffers);

		GraphicsContext::GetSwapChain().Present();
	}

	void VulkanRenderer::Shutdown()
	{
		GraphicsContext::ShutDown();
	}

	void VulkanRenderer::CreateTrianglePipeline()
	{
		PipelineBuilder builder;
		{
			VkPipelineVertexInputStateCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			info.pNext = nullptr;

			info.vertexBindingDescriptionCount = 0;
			info.vertexAttributeDescriptionCount = 0;
			builder.m_VertexInputState = info;
		}
		{
			VkPipelineInputAssemblyStateCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
			info.pNext = nullptr;
			info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			info.primitiveRestartEnable = VK_FALSE;
			builder.m_InputAssemblyState = info;
		}

		builder.m_Viewport.x = 0.f;
		builder.m_Viewport.y = 0.f;
		builder.m_Viewport.width = Application::GetWindow().GetWidth();
		builder.m_Viewport.height = Application::GetWindow().GetHeight();
		builder.m_Viewport.minDepth = 0.0f;
		builder.m_Viewport.maxDepth = 1.0f;

		builder.m_Scissor.offset = { 0, 0 };
		builder.m_Scissor.extent = { Application::GetWindow().GetWidth(), Application::GetWindow().GetHeight() };

		{
			VkPipelineRasterizationStateCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			info.pNext = nullptr;
			info.depthClampEnable = VK_FALSE;
			info.rasterizerDiscardEnable = VK_FALSE;

			info.polygonMode = VK_POLYGON_MODE_FILL;
			info.lineWidth = 1.0f;

			info.cullMode = VK_CULL_MODE_NONE;
			info.frontFace = VK_FRONT_FACE_CLOCKWISE;

			info.depthBiasEnable = VK_FALSE;
			info.depthBiasConstantFactor = 0.0f;
			info.depthBiasClamp = 0.0f;
			info.depthBiasSlopeFactor = 0.0f;

			builder.m_Rasterizer = info;
		}
		builder.m_ShaderStages.push_back(m_FragShader);
		builder.m_ShaderStages.push_back(m_VertShader);
		{
			VkPipelineMultisampleStateCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
			info.pNext = nullptr;
			info.sampleShadingEnable = VK_FALSE;
			info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
			info.minSampleShading = 1.0f;
			info.pSampleMask = nullptr;
			info.alphaToCoverageEnable = VK_FALSE;
			info.alphaToOneEnable = VK_FALSE;
			builder.m_Multisampling = info;
		}
		{
			VkPipelineColorBlendAttachmentState info{};
			info.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
			info.blendEnable = VK_FALSE;
			builder.m_ColorBlendAttachmentState = info;
		}

		m_TrianglePipeline = Pipeline::Create(builder, m_RenderPass);
	}
}
