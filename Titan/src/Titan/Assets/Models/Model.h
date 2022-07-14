#pragma once
#include <filesystem>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "Titan/Assets/Asset.h"
#include "Titan/Core/Core.h"
#include "Titan/Rendering/VulkanAPI/Mesh/Mesh.h"


namespace Titan
{
	class Model : public Asset
	{
		friend class VulkanRenderer;
	public:
		Model(const std::filesystem::path& modelPath);
		inline glm::mat4& GetMatrix();
		inline void SetPosition(const glm::vec3& position) { m_Position = position; }
		inline void SetRotation(const glm::vec3& rotation) { m_Rotation = rotation; }
		inline void SetRotation(const glm::quat& rotation) { m_Rotation = rotation; }
		inline void SetScale(const glm::vec3& scale) { m_Scale = scale; }
		inline glm::vec3& GetPosition() { return m_Position; }
		inline glm::quat& GetQuaternion() { return m_Rotation; }
		inline glm::vec3& GetScale() { return m_Scale; }
		static Ref<Model> Create(const std::filesystem::path& modelPath);
	private:
		glm::vec3 m_Position;
		glm::quat m_Rotation;
		glm::vec3 m_Scale;
		glm::mat4 m_Transform;
		Ref<Mesh> m_Mesh;
	};
}
