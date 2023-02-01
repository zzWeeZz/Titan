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
		std::vector<VkShaderStageFlagBits> stages;

		FindStages(info, stages);
		TN_CORE_ASSERT(stages.empty() == false, "Could not create graphics pipeline!");

		std::vector<Shader> shaders;
		GetShaderObjects(stages, shaders, info);


		std::vector<VkPipelineShaderStageCreateInfo> pipelineStageInfos;
		CreatePipelineStages(stages, pipelineStageInfos, shaders, info);

		std::vector<VkDynamicState> dynamicStates =
		{
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};

		VkPipelineDynamicStateCreateInfo dynamicState{};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
		dynamicState.pDynamicStates = dynamicStates.data();

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
		rasterizer.cullMode = VK_CULL_MODE_NONE;
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

		std::vector<VkDescriptorSetLayout> descriptorlayouts;

		CombineAndCreateDescriptorlayouts(shaders, descriptorlayouts);

		std::vector<VkPushConstantRange> pushConstants;

		for (auto& shader : shaders)
		{
			if (pushConstants.empty())
			{
				pushConstants.push_back(shader.pushConstants);
			}
			else
			{
				for (auto& pc : pushConstants)
				{
					if (shader.pushConstants.size == 0)
					{
						continue;
					}
					if (shader.pushConstants.size == pc.size)
					{
						pc.stageFlags |= shader.pushConstants.stageFlags;
					}
					else
					{
						pushConstants.push_back(shader.pushConstants);
					}
				}
			}
		}

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		if (!descriptorlayouts.empty())
		{
			pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorlayouts.size()); // Optional
			pipelineLayoutInfo.pSetLayouts = descriptorlayouts.data(); // Optional
		}
		if (!pushConstants.empty())
		{
			pipelineLayoutInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstants.size()); // Optional
			pipelineLayoutInfo.pPushConstantRanges = pushConstants.data(); // Optional
		}
		m_PipelineLayout = nullptr;
		TN_VK_CHECK(vkCreatePipelineLayout(GraphicsContext::GetDevice().GetHandle(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout));
		TitanAllocator::QueueDeletion([&]() { vkDestroyPipelineLayout(GraphicsContext::GetDevice().GetHandle(), m_PipelineLayout, nullptr); });


		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = static_cast<uint32_t>(pipelineStageInfos.size());
		pipelineInfo.pStages = pipelineStageInfos.data();
		pipelineInfo.pVertexInputState = nullptr;
		pipelineInfo.pInputAssemblyState = nullptr;
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
		const VkPipelineRenderingCreateInfoKHR pipelineRenderingCreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR,
			.colorAttachmentCount = static_cast<uint32_t>(depthFormat == VK_FORMAT_UNDEFINED ? formats.size() : (formats.size() - 1)),
			.pColorAttachmentFormats = formats.data(),
			.depthAttachmentFormat = depthFormat,
		};

		pipelineInfo.pNext = &pipelineRenderingCreateInfo;
		pipelineInfo.renderPass = nullptr;

		TN_VK_CHECK(vkCreateGraphicsPipelines(GraphicsContext::GetDevice().GetHandle(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_Pipeline));
		TitanAllocator::QueueDeletion([&]() {vkDestroyPipeline(GraphicsContext::GetDevice().GetHandle(), m_Pipeline, nullptr); });

		for (auto& pipelineStage : pipelineStageInfos)
		{
			vkDestroyShaderModule(GraphicsContext::GetDevice().GetHandle(), pipelineStage.module, nullptr);
		}
	}

	void GraphicsPipeline::CombineAndCreateDescriptorlayouts(std::vector<Titan::Shader>& shaders, std::vector<VkDescriptorSetLayout>& descriptorlayouts)
	{
		std::unordered_map<uint32_t, std::vector<VkDescriptorSetLayoutBinding>> bindingMap;
		for (auto& shader : shaders)
		{
			auto& layout = shader.layouts;
			for (auto& set : layout)
			{
				auto& setBindings = bindingMap[set.first];
				setBindings.insert(setBindings.end(), set.second.begin(), set.second.end());
			}
		}
		std::vector<std::pair<uint32_t, std::vector<VkDescriptorSetLayoutBinding>>> sortedBindMap;

		for (auto& binding : bindingMap)
		{
			auto& sortBinding = sortedBindMap.emplace_back();
			sortBinding.first = binding.first;
			sortBinding.second = binding.second;
		}

		std::sort(sortedBindMap.begin(), sortedBindMap.end(), [](std::pair<uint32_t, std::vector<VkDescriptorSetLayoutBinding>>& first, std::pair<uint32_t, std::vector<VkDescriptorSetLayoutBinding>>& second) -> bool
			{
				return first.first < second.first;
			});

		for (auto& binding : sortedBindMap)
		{
			auto& bindings = binding.second;
			VkDescriptorSetLayoutCreateInfo layoutInfo{};
			layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
			layoutInfo.pBindings = bindings.data();
			auto& descriptorLayout = descriptorlayouts.emplace_back();
			TN_VK_CHECK(vkCreateDescriptorSetLayout(GraphicsContext::GetDevice().GetHandle(), &layoutInfo, nullptr, &descriptorLayout));
			TitanAllocator::QueueDeletion([=]() {vkDestroyDescriptorSetLayout(GraphicsContext::GetDevice().GetHandle(), descriptorLayout, nullptr); });
		}
	}

	void GraphicsPipeline::GetShaderObjects(std::vector<VkShaderStageFlagBits>& stages, std::vector<Shader>& shaders, const Titan::GraphicsPipelineInfo& info)
	{
		for (auto& stage : stages)
		{
			switch (stage)
			{
			case VK_SHADER_STAGE_TASK_BIT_NV:
				shaders.emplace_back(ShaderLibrary::Get(info.tsPath));
				break;
			case VK_SHADER_STAGE_MESH_BIT_NV:
				shaders.emplace_back(ShaderLibrary::Get(info.msPath));
				break;
			case VK_SHADER_STAGE_FRAGMENT_BIT:
				shaders.emplace_back(ShaderLibrary::Get(info.fsPath));
				break;
			default:
				TN_CORE_ASSERT(false, "Shader stage is not supported");
				break;
			}
		}
	}

	void GraphicsPipeline::CreatePipelineStages(std::vector<VkShaderStageFlagBits>& stages, std::vector<VkPipelineShaderStageCreateInfo>& pipelineStageInfos, std::vector<Shader>& shaders, const Titan::GraphicsPipelineInfo& info)
	{
		info;
		TN_CORE_ASSERT(stages.size() == shaders.size(), "Shader stages and shader objects are not the same size!");

		for (size_t i = 0; i < stages.size(); ++i)
		{
			auto& stage = stages[i];
			auto& shader = shaders[i];

			VkPipelineShaderStageCreateInfo& shaderStageInfo = pipelineStageInfos.emplace_back();
			shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			shaderStageInfo.stage = stage;
			shaderStageInfo.pName = "main";
			shaderStageInfo.module = CreateShaderModule(shader.spvBinary);
		}
	}

	void GraphicsPipeline::FindStages(const Titan::GraphicsPipelineInfo& info, std::vector<VkShaderStageFlagBits>& stages)
	{
		if (!info.tsPath.empty())
		{
			stages.push_back(VK_SHADER_STAGE_TASK_BIT_NV);
		}
		if (!info.msPath.empty())
		{
			stages.push_back(VK_SHADER_STAGE_MESH_BIT_NV);
		}
		if (!info.fsPath.empty())
		{
			stages.push_back(VK_SHADER_STAGE_FRAGMENT_BIT);
		}
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