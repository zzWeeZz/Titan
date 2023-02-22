#include "TNpch.h"
#include "Shader.h"
#include <SPIRV-Reflect/spirv_reflect.h>

namespace Titan
{
	ShaderType Titan::GetShaderTypeFromFile(const std::filesystem::path& path)
	{
		auto filename = path.stem().string();
		auto shaderValidater = filename.substr(filename.size() - 2, 2);
		if (shaderValidater == "fs" || shaderValidater == "ps")
		{
			return ShaderType::Fragment;
		}
		else if (shaderValidater == "cs")
		{
			return ShaderType::Compute;
		}
		else if (shaderValidater == "ms")
		{
			return ShaderType::Mesh;
		}
		else if (shaderValidater == "ts")
		{
			return ShaderType::Task;
		}
		return ShaderType::Count;
	}
	void Shader::Reflect()
	{
		SpvReflectShaderModule spvModule;
		auto reflectResult = spvReflectCreateShaderModule(m_Spirv.size() * 4, reinterpret_cast<void*>(m_Spirv.data()), &spvModule);
		TN_CORE_ASSERT(reflectResult == SPV_REFLECT_RESULT_SUCCESS, "Could not reflect shader.");

		switch (spvModule.shader_stage)
		{
		case SPV_REFLECT_SHADER_STAGE_COMPUTE_BIT:
		{
			m_Stage = VK_SHADER_STAGE_COMPUTE_BIT;
			break;
		}
		case SPV_REFLECT_SHADER_STAGE_TASK_BIT_NV:
		{
			m_Stage = VK_SHADER_STAGE_TASK_BIT_NV;
			break;
		}
		case SPV_REFLECT_SHADER_STAGE_MESH_BIT_NV:
		{
			m_Stage = VK_SHADER_STAGE_MESH_BIT_NV;
			break;
		}

		case SPV_REFLECT_SHADER_STAGE_VERTEX_BIT:
		{
			m_Stage = VK_SHADER_STAGE_VERTEX_BIT;
			break;
		}
		case SPV_REFLECT_SHADER_STAGE_FRAGMENT_BIT:
		{
			m_Stage = VK_SHADER_STAGE_FRAGMENT_BIT;
			break;
		}
		}

		if (spvModule.push_constant_block_count > 0 && spvModule.push_constant_blocks != nullptr)
		{
			m_PushConstantRange = { .stageFlags = m_Stage,.offset = spvModule.push_constant_blocks->offset, .size = spvModule.push_constant_blocks->size };
		}

		uint32_t spvBindingCount = 0;

		spvReflectEnumerateDescriptorBindings(&spvModule, &spvBindingCount, nullptr);

		std::vector<SpvReflectDescriptorBinding*> spvBindings(spvBindingCount);
		spvReflectEnumerateDescriptorBindings(&spvModule, &spvBindingCount, spvBindings.data());

		m_Layouts.reserve(spvBindingCount);
		for (uint32_t layoutBindingIndex = 0; layoutBindingIndex < spvBindingCount; ++layoutBindingIndex)
		{
			VkDescriptorType descType{};
			switch (spvBindings[layoutBindingIndex]->descriptor_type)
			{
			case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER:
				descType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
				break;
			case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
				descType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				break;

			case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE:
				descType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
				break;
			case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
				descType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				break;
			default:
				break;
			}
			VkDescriptorSetLayoutBinding descriptorSetLayoutBinding{};
			descriptorSetLayoutBinding.binding = spvBindings[layoutBindingIndex]->binding;
			descriptorSetLayoutBinding.descriptorCount = 1;
			descriptorSetLayoutBinding.descriptorType = descType;
			descriptorSetLayoutBinding.stageFlags = m_Stage;
			descriptorSetLayoutBinding.pImmutableSamplers = nullptr;

			m_Layouts[spvBindings[layoutBindingIndex]->set].push_back(descriptorSetLayoutBinding);
		}
		spvReflectDestroyShaderModule(&spvModule);
	}
}