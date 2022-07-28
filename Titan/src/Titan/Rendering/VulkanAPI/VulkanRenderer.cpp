#include "VulkanRenderer.h"

#include "GraphicsContext.h"
#include "Titan/Application.h"

#include <glm/gtx/transform.hpp>

#include "imgui.h"
#include "imgui_impl_vulkan.h"
#include "Buffers/Buffer.h"
#include "Mesh/Mesh.h"
#include "Titan/Assets/Models/Model.h"


namespace Titan
{
	static Scope<RenderData> s_Data = CreateScope<RenderData>();
	void VulkanRenderer::Initialize()
	{
		GraphicsContext::CreateCommandBuffer(m_CommandBuffer);
		{
			RenderPassCreateInfo info{};
			info.width = 1280;
			info.height = 720;
			info.haveDepth = false;
			info.isSwapchain = false;
			info.DepthView = GraphicsContext::GetSwapChain().GetDepthView();
			m_RenderPass = RenderPass::Create(info);
		}
		m_FragShader = Shader::Create("Shaders/triangle.frag.spv", ShaderType::Fragment);
		m_VertShader = Shader::Create("Shaders/triangle.vert.spv", ShaderType::Vertex);

		m_PushConstant = PushConstant<MeshConstant>::Create();
		m_Texture = Texture::Create("Texture/Titan.png");
		InitializeDescriptors();
		CreateTrianglePipeline();
	}

	void VulkanRenderer::SubmitMesh(Ref<Model> mesh)
	{
		s_Data->models.emplace_back(mesh.get());
	}

	void VulkanRenderer::DebugImGuiStuff()
	{
		ImGui::Begin("viewPort");
		/*const auto descriptor = ImGui_ImplVulkan_AddTexture(m_Sampler, m_RenderPass->GetFrameBuffer()->GetImageView(GraphicsContext::GetSwapChain().GetImageCount()), VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL);

		ImGui::Image(descriptor, { 1280, 720 });*/
		ImGui::End();
	}

	void VulkanRenderer::Begin()
	{
		static int time = 0;
		time++;
		GraphicsContext::GetSwapChain().WaitOnFences();
		vkResetDescriptorPool(GraphicsContext::GetDevice(), m_ImGuiDescriptorPool, 0);
		m_CommandBuffer->Reset();
		m_CommandBuffer->Bind();

		VkClearValue clearValue;
		clearValue.color = { {0.3f, 0.3f, 0.3f, 1.f} };

		VkClearValue DepthValue;
		DepthValue.depthStencil.depth = 1.0f;
		std::vector<VkClearValue> clears = { clearValue, DepthValue };
		VkRenderingAttachmentInfo colorAttachmentinfo{};
		colorAttachmentinfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
		colorAttachmentinfo.imageView = GraphicsContext::GetSwapChain().GetViews()[GraphicsContext::GetSwapChain().GetImageCount()];
		colorAttachmentinfo.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
		colorAttachmentinfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachmentinfo.storeOp = VK_ATTACHMENT_STORE_OP_NONE;
		colorAttachmentinfo.clearValue = clearValue;

		VkRenderingInfo rendingInfo{};
		rendingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
		rendingInfo.renderArea.extent = { 1280, 720 };
		rendingInfo.renderArea.offset = { 0, 0 };
		rendingInfo.layerCount = 1;
		rendingInfo.colorAttachmentCount = 1;
		rendingInfo.pColorAttachments = &colorAttachmentinfo;

		vkCmdBeginRendering(m_CommandBuffer->GetHandle(), &rendingInfo);

		glm::vec4 camPos = { 0.f, 0.f, -10.f , 1 };
		glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(camPos));
		glm::mat4 projection = glm::perspective(glm::radians(70.f), 1700.f / 900.f, 0.1f, 200.f);
		projection[1][1] *= -1;

		CameraUniform camData{};
		camData.pos = camPos;
		camData.proj = projection;
		camData.view = view;

		void* data;
		vmaMapMemory(GraphicsContext::GetAllocator(), s_Data->cameraBuffer->GetAllocation(), &data);
		memcpy(data, &camData, sizeof(CameraUniform));
		vmaUnmapMemory(GraphicsContext::GetAllocator(), s_Data->cameraBuffer->GetAllocation());

		auto& Pushdata = m_PushConstant->GetData();
		m_TrianglePipeline->Bind(m_CommandBuffer);
		vkCmdBindDescriptorSets(m_CommandBuffer->GetHandle(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_TrianglePipeline->GetPipelineLayout(), 0, 1, &m_DescriptorSet, 0, nullptr);
		for (auto model : s_Data->models)
		{
			Pushdata.transform = model->GetMatrix();
			m_PushConstant->PushToGpu(m_CommandBuffer, m_TrianglePipeline, VK_SHADER_STAGE_VERTEX_BIT);
			VkDeviceSize offset = 0;
			model->m_Mesh->m_VertexArray->GetVertexBuffer()->Bind(m_CommandBuffer);
			model->m_Mesh->m_VertexArray->GetIndexBuffer()->Bind(m_CommandBuffer);
			vkCmdDrawIndexed(m_CommandBuffer->GetHandle(), static_cast<uint32_t>(model->m_Mesh->m_VertexArray->GetIndexArray().size()), 1, 0, 0, 0);
		}

		//ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), m_CommandBuffer->GetHandle());

		vkCmdEndRendering(m_CommandBuffer->GetHandle());

		TN_VK_CHECK(vkEndCommandBuffer(m_CommandBuffer->GetHandle()));
		std::vector<Ref<CommandBuffer>> commandBuffers;
		commandBuffers.push_back(m_CommandBuffer);


		GraphicsContext::GetSwapChain().Submit(commandBuffers);


		GraphicsContext::GetSwapChain().Present();

		s_Data->models.clear();
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

		VertexInputDescription vertexDescription = Vertex::GetInputDescription();
		builder.m_VertexInputState.pVertexAttributeDescriptions = vertexDescription.Attributes.data();
		builder.m_VertexInputState.pVertexBindingDescriptions = vertexDescription.Bindings.data();

		builder.m_VertexInputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexDescription.Attributes.size());
		builder.m_VertexInputState.vertexBindingDescriptionCount = static_cast<uint32_t>(vertexDescription.Bindings.size());

		{
			VkPipelineRasterizationStateCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			info.pNext = nullptr;
			info.depthClampEnable = VK_FALSE;
			info.rasterizerDiscardEnable = VK_FALSE;

			info.polygonMode = VK_POLYGON_MODE_FILL;
			info.lineWidth = 1.0f;

			info.cullMode = VK_CULL_MODE_FRONT_BIT;
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
		VkPipelineDepthStencilStateCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		info.pNext = nullptr;

		info.depthTestEnable = VK_TRUE;
		info.depthWriteEnable = VK_TRUE;
		info.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
		info.depthBoundsTestEnable = VK_FALSE;
		info.minDepthBounds = 0.0f; // Optional
		info.maxDepthBounds = 1.0f; // Optional
		info.stencilTestEnable = VK_FALSE;
		builder.m_DepthStencilState = info;

		builder.m_DescriptorSetLayouts.push_back(m_SetLayout);

		m_TrianglePipeline = Pipeline::Create(builder, m_RenderPass);
	}

	void VulkanRenderer::InitializeDescriptors()
	{
		std::vector<VkDescriptorPoolSize> sizes;
		sizes = {
			{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 10},
			{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 10}
		};

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;

		poolInfo.flags = 0;
		poolInfo.maxSets = 10;
		poolInfo.poolSizeCount = static_cast<uint32_t>(sizes.size());
		poolInfo.pPoolSizes = sizes.data();

		vkCreateDescriptorPool(GraphicsContext::GetDevice(), &poolInfo, nullptr, &m_DescriptorPool);

		VkDescriptorSetLayoutBinding camBufferBinding{};
		camBufferBinding.binding = 0;
		camBufferBinding.descriptorCount = 1;
		camBufferBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		camBufferBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

		VkDescriptorSetLayoutBinding textureBind{};
		textureBind.binding = 1;
		textureBind.descriptorCount = 1;
		textureBind.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		textureBind.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		std::vector<VkDescriptorSetLayoutBinding> binding = { camBufferBinding, textureBind };

		VkDescriptorSetLayoutCreateInfo setInfo{};
		setInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		setInfo.pNext = nullptr;

		setInfo.bindingCount = binding.size();
		setInfo.flags = 0;
		setInfo.pBindings = binding.data();


		vkCreateDescriptorSetLayout(GraphicsContext::GetDevice(), &setInfo, nullptr, &m_SetLayout);

		GlobalDeletionQueue.PushFunction([&]
			{
				vkDestroyDescriptorSetLayout(GraphicsContext::GetDevice(), m_SetLayout, nullptr);
				vkDestroyDescriptorPool(GraphicsContext::GetDevice(), m_DescriptorPool, nullptr);
			});
		s_Data->cameraBuffer = Buffer::Create(sizeof(CameraUniform), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
		GlobalDeletionQueue.PushFunction([&]
			{
				s_Data->cameraBuffer->Destroy();
			});
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.pNext = nullptr;
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;

		allocInfo.descriptorPool = m_DescriptorPool;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &m_SetLayout;

		vkAllocateDescriptorSets(GraphicsContext::GetDevice(), &allocInfo, &m_DescriptorSet);

		VkSamplerCreateInfo samplerInfo = {};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.pNext = nullptr;

		samplerInfo.magFilter = VK_FILTER_NEAREST;
		samplerInfo.minFilter = VK_FILTER_NEAREST;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

		vkCreateSampler(GraphicsContext::GetDevice(), &samplerInfo, nullptr, &m_Sampler);

		VkDescriptorImageInfo imageBufferInfo;
		imageBufferInfo.sampler = m_Sampler;
		imageBufferInfo.imageView = m_Texture->GetView();
		imageBufferInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;



		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = s_Data->cameraBuffer->GetBuffer();
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(CameraUniform);

		VkWriteDescriptorSet setWrite{};
		setWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		setWrite.pNext = nullptr;

		setWrite.dstBinding = 0;
		setWrite.dstSet = m_DescriptorSet;

		setWrite.descriptorCount = 1;
		setWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		setWrite.pBufferInfo = &bufferInfo;
		vkUpdateDescriptorSets(GraphicsContext::GetDevice(), 1, &setWrite, 0, nullptr);

		VkWriteDescriptorSet setTexWrite{};
		setTexWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		setTexWrite.pNext = nullptr;
		setTexWrite.dstBinding = 1;
		setTexWrite.dstSet = m_DescriptorSet;
		setTexWrite.descriptorCount = 1;
		setTexWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		setTexWrite.pImageInfo = &imageBufferInfo;

		vkUpdateDescriptorSets(GraphicsContext::GetDevice(), 1, &setTexWrite, 0, nullptr);
		std::vector<VkDescriptorPoolSize> imguiSizes;
		imguiSizes =
		{
			{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000}
		};

		VkDescriptorPoolCreateInfo imGuipoolInfo{};
		imGuipoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;

		imGuipoolInfo.flags = 0;
		imGuipoolInfo.maxSets = 1000;
		imGuipoolInfo.poolSizeCount = static_cast<uint32_t>(imguiSizes.size());
		imGuipoolInfo.pPoolSizes = imguiSizes.data();

		vkCreateDescriptorPool(GraphicsContext::GetDevice(), &poolInfo, nullptr, &m_ImGuiDescriptorPool);
	}
}
