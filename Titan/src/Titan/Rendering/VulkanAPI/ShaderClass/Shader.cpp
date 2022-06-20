#include "Shader.h"
#include <vulkan/vulkan.h>
#include <fstream>

#include "Titan/Rendering/VulkanAPI/GraphicsContext.h"


namespace Titan
{
	Shader::Shader(const std::string& filepath, ShaderType type)
	{
		m_Type = type;
		std::ifstream file(filepath, std::ios::ate | std::ios::binary);

		if (!file.is_open())
		{
			TN_CORE_WARN("Couldn't open shader: {0}", filepath);
			return;
		}

		size_t fileSize = file.tellg();
		std::vector<uint32_t> buffer(fileSize / sizeof(uint32_t));

		file.seekg(0);
		file.read(reinterpret_cast<char*>(buffer.data()), fileSize);

		file.close();

		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.pNext = nullptr;

		createInfo.codeSize = buffer.size() * sizeof(uint32_t);
		createInfo.pCode = buffer.data();

		if (vkCreateShaderModule(GraphicsContext::GetDevice(), &createInfo, nullptr, &m_ShaderModule) != VK_SUCCESS)
		{
			TN_CORE_WARN("Couldn't create Shader: {0}", filepath);
			return;
		}
		TN_CORE_TRACE("Successful shadercreation of: {0}", filepath);
		GlobalDeletionQueue.PushFunction([=]
			{
				vkDestroyShaderModule(GraphicsContext::GetDevice(), m_ShaderModule, nullptr);
			});
	}

	void Shader::Bind()
	{
	}

	Ref<Shader> Shader::Create(const std::string& filepath, ShaderType type)
	{
		return CreateRef<Shader>(filepath, type);
	}

	VkPipelineShaderStageCreateInfo Shader::GetCreateInfo()
	{
		VkShaderStageFlagBits stage;
		switch (m_Type)
		{
		case ShaderType::Vertex:
			stage = VK_SHADER_STAGE_VERTEX_BIT;
			break;
		case ShaderType::Fragment:
			stage = VK_SHADER_STAGE_FRAGMENT_BIT;
			break;
		case ShaderType::Geometry:
			stage = VK_SHADER_STAGE_GEOMETRY_BIT;
			break;
		case ShaderType::Compute:
			stage = VK_SHADER_STAGE_COMPUTE_BIT;
			break;
		case ShaderType::TessellationControl:
			stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
			break;
		case ShaderType::TessellationEvaluation:
			stage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
			break;
		default:;
		}

		VkPipelineShaderStageCreateInfo info{};
		info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		info.pNext = nullptr;

		info.stage = stage;
		info.module = m_ShaderModule;
		info.pName = "main";
		return info;
	
		}
}
