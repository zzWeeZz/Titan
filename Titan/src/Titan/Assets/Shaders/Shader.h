#pragma once
#include "Titan/Assets/Resource.h"
namespace Titan
{
	enum class ShaderType
	{
		Fragment = 1,
		Compute,
		Task = 26,
		Mesh = 27,
		Count
	};

	ShaderType GetShaderTypeFromFile(const std::filesystem::path& path);

	class Shader : public Resource
	{
		friend class GLSLImporter;
		friend class HLSLImporter;
	public:
		void Reflect();
		TN_FORCE_INLINE std::vector<uint32_t>& GetSpirv() { return m_Spirv; }
		TN_FORCE_INLINE ShaderType GetType() { return m_ShaderType; };
		TN_FORCE_INLINE VkShaderStageFlags GetVkStage() { return m_Stage; }
		TN_FORCE_INLINE std::unordered_map<uint32_t, std::vector<VkDescriptorSetLayoutBinding>>& GetLayoutBindings() { return m_Layouts; }
		TN_FORCE_INLINE VkPushConstantRange& GetPushContantRange() { return m_PushConstantRange; }
	private:
		std::vector<uint32_t> m_Spirv;
		ShaderType m_ShaderType = ShaderType::Count;
		VkShaderStageFlags m_Stage = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
		std::unordered_map<uint32_t, std::vector<VkDescriptorSetLayoutBinding>> m_Layouts;
		VkPushConstantRange m_PushConstantRange;
	};
}