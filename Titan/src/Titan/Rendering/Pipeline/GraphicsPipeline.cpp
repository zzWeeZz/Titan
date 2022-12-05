#include "TNpch.h"
#include "GraphicsPipeline.h"
#include "Titan/Rendering/GraphicsContext.h"
#include <Titan/Core/TitanFormats.h>
#include "Titan/Utils/FilesystemUtils.h"
#include "Titan/Utils/TitanAllocator.h"
#include "Titan/Application.h"
#include "Titan/Rendering/Libraries/ShaderLibrary.h"
#include <Titan/Rendering/Vertices.h>
namespace Titan
{
	GraphicsPipeline::GraphicsPipeline(const GraphicsPipelineInfo& info)
	{
		auto vertShaderModule = CreateShaderModule(ShaderLibrary::Get(info.vsPath).spvBinary);
		auto fragShaderModule = CreateShaderModule(ShaderLibrary::Get(info.psPath).spvBinary);

		VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = vertShaderModule;
		vertShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = fragShaderModule;
		fragShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

		std::vector<VkDynamicState> dynamicStates = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};

		VkPipelineDynamicStateCreateInfo dynamicState{};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
		dynamicState.pDynamicStates = dynamicStates.data();

		auto vertexInfo = Vertex::GetBindingDesc();

		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(vertexInfo.Bindings.size());
		vertexInputInfo.pVertexBindingDescriptions = vertexInfo.Bindings.data(); // Optional
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexInfo.Attributes.size());
		vertexInputInfo.pVertexAttributeDescriptions = vertexInfo.Attributes.data(); // Optional

		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = FormatToVkFormat(info.topology);
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)Application::GetWindow().GetWidth();
		viewport.height = (float)Application::GetWindow().GetHeight();
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = GraphicsContext::GetSwapchain().GetExtent();

		VkPipelineViewportStateCreateInfo viewportState{};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.pViewports = &viewport;
		viewportState.scissorCount = 1;
		viewportState.pScissors = &scissor;

		VkPipelineRasterizationStateCreateInfo rasterizer{};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;
		rasterizer.depthBiasConstantFactor = 0.0f; // Optional
		rasterizer.depthBiasClamp = 0.0f; // Optional
		rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

		VkPipelineMultisampleStateCreateInfo multisampling{};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampling.minSampleShading = 1.0f; // Optional
		multisampling.pSampleMask = nullptr; // Optional
		multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
		multisampling.alphaToOneEnable = VK_FALSE; // Optional

		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

		VkPipelineColorBlendStateCreateInfo colorBlending{};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f; // Optional
		colorBlending.blendConstants[1] = 0.0f; // Optional
		colorBlending.blendConstants[2] = 0.0f; // Optional
		colorBlending.blendConstants[3] = 0.0f; // Optional
		
		VkPipelineDepthStencilStateCreateInfo depthStencil{};
		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencil.depthTestEnable = VK_TRUE;
		depthStencil.depthWriteEnable = VK_TRUE;
		depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
		depthStencil.depthBoundsTestEnable = VK_FALSE;
		depthStencil.minDepthBounds = 0.0f; // Optional
		depthStencil.maxDepthBounds = 1.0f; // Optional
		depthStencil.stencilTestEnable = VK_FALSE;
		depthStencil.front = {}; // Optional
		depthStencil.back = {}; // Optional

		auto& vShader = ShaderLibrary::Get(info.vsPath);
		auto& fShader = ShaderLibrary::Get(info.psPath);
		
		std::vector<VkDescriptorSetLayoutBinding> bindings = vShader.layoutBindings;
		bindings.insert(bindings.end(), fShader.layoutBindings.begin(), fShader.layoutBindings.end());

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();

		TN_VK_CHECK(vkCreateDescriptorSetLayout(GraphicsContext::GetDevice().GetHandle(), &layoutInfo, nullptr, &m_DescriptorSetLayout));
		TitanAllocator::QueueDeletion([&]() {vkDestroyDescriptorSetLayout(GraphicsContext::GetDevice().GetHandle(), m_DescriptorSetLayout, nullptr); });

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1; // Optional
		pipelineLayoutInfo.pSetLayouts = &m_DescriptorSetLayout; // Optional
		pipelineLayoutInfo.pushConstantRangeCount = 1; // Optional
		pipelineLayoutInfo.pPushConstantRanges = &vShader.pushConstants; // Optional

		TN_VK_CHECK(vkCreatePipelineLayout(GraphicsContext::GetDevice().GetHandle(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout));
		TitanAllocator::QueueDeletion([&]() { vkDestroyPipelineLayout(GraphicsContext::GetDevice().GetHandle(), m_PipelineLayout, nullptr); });


		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = &dynamicState;
		pipelineInfo.pDepthStencilState = &depthStencil;

		pipelineInfo.layout = m_PipelineLayout;
		std::vector<VkFormat> formats(info.imageFormats.size());
		VkFormat depthFormat = VK_FORMAT_UNDEFINED;
		for (size_t i = 0; i < formats.size(); ++i)
		{
			if (!IsDepth(info.imageFormats[i]))
			{
				formats[i] = FormatToVkFormat(info.imageFormats[i]);
			}
			else
			{
				depthFormat = FormatToVkFormat(info.imageFormats[i]);
			}
		}
		const VkPipelineRenderingCreateInfoKHR pipeline_rendering_create_info
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR,
			.colorAttachmentCount = static_cast<uint32_t>(depthFormat == VK_FORMAT_UNDEFINED ? formats.size() : (formats.size() - 1)),
			.pColorAttachmentFormats = formats.data(),
			.depthAttachmentFormat = depthFormat
		};

		pipelineInfo.pNext = &pipeline_rendering_create_info;
		pipelineInfo.renderPass = nullptr;

		TN_VK_CHECK(vkCreateGraphicsPipelines(GraphicsContext::GetDevice().GetHandle(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_Pipeline));
		TitanAllocator::QueueDeletion([&]() {vkDestroyPipeline(GraphicsContext::GetDevice().GetHandle(), m_Pipeline, nullptr); });

		vkDestroyShaderModule(GraphicsContext::GetDevice().GetHandle(), vertShaderModule, nullptr);
		vkDestroyShaderModule(GraphicsContext::GetDevice().GetHandle(), fragShaderModule, nullptr);
	}
	
	void GraphicsPipeline::Bind(VkCommandBuffer& cmd)
	{
		vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline);
	}

	Ref<GraphicsPipeline> GraphicsPipeline::Create(const GraphicsPipelineInfo& info)
	{
		return CreateRef<GraphicsPipeline>(info);
	}

	VkShaderModule GraphicsPipeline::CreateShaderModule(std::vector<uint32_t> assembly)
	{
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = assembly.size() * 4;
		createInfo.pCode = assembly.data();

		VkShaderModule shaderModule;
		TN_VK_CHECK(vkCreateShaderModule(GraphicsContext::GetDevice().GetHandle(), &createInfo, nullptr, &shaderModule));
		return shaderModule;
	}
}