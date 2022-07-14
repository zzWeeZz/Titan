#include "Model.h"



namespace Titan
{
	Model::Model(const std::filesystem::path& modelPath)
	{
		m_Mesh = Mesh::Create(modelPath);
		m_Position = { 0,0,0 };
		m_Rotation = {1,0,0,0};
		m_Scale = { 1, 1, 1 };

		GetMatrix();
	}

	glm::mat4& Model::GetMatrix()
	{
		//m_Transform = glm::mat4(1.0f);
		m_Transform = glm::translate(glm::mat4(1.f), m_Position);
		m_Transform *= glm::mat4_cast(m_Rotation);
		m_Transform *= glm::scale(glm::mat4(1.0f), m_Scale);
		return m_Transform;
	}

	Ref<Model> Model::Create(const std::filesystem::path& modelPath)
	{
		return CreateRef<Model>(modelPath);
	}
}
