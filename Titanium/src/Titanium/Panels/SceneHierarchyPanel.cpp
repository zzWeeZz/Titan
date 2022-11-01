#include "SceneHierarchyPanel.h"
#include "ImGui/imgui.h"
#include "Titan/Scene/Components.h"

namespace Titan
{
	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context)
	{
		m_Context = context;
	}

	void SceneHierarchyPanel::SetContext(const Ref<Scene>& context)
	{
		m_Context = context;
	}

	void SceneHierarchyPanel::OnImGuiDraw()
	{
		ImGui::Begin("Hierarchy");

		ImGui::Text("Scene: NAME");
		ImGui::BeginChild("hirarchyPanel");
		m_Context->m_Registry.ForEach([&](Snowflake::Entity ent)
			{
				Entity entity = { ent, m_Context.get() };
				DrawEntityNode(entity);

			});
		ImGui::EndChild();

		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
		{
			m_SelectedEntity = {};
		}
		bool destroy = false;
		if (ImGui::BeginPopupContextWindow("one", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
		{
			if (ImGui::MenuItem("Create Empty Entity"))
			{
				m_Context->CreateEntity();
			}
			ImGui::EndPopup();
		}

		ImGui::End();
	}

	void SceneHierarchyPanel::EntitySelectedCallback(std::function<void(Entity& selectedEntity)>&& callback)
	{
		callback(m_SelectedEntity);
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		auto& tag = entity.GetComponent<TagComponent>();
		ImGuiTreeNodeFlags flags = ((m_SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		bool isOpened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity.GetId(), flags, tag.tag.c_str());
		if (ImGui::IsItemClicked())
		{
			m_SelectedEntity = entity;
		}

		bool deleted = false;
		if (ImGui::BeginPopupContextWindow(std::to_string(entity.GetId()).c_str(), ImGuiPopupFlags_MouseButtonRight))
		{
			if (ImGui::MenuItem("destroy Entity"))
			{
				deleted = true;
			}
			ImGui::EndPopup();
		}
		if (isOpened)
		{
			ImGui::TreePop();
		}
		if (deleted)
		{
			m_Context->DestroyEntity(m_SelectedEntity);
			if (m_SelectedEntity == entity)
			{
				m_SelectedEntity = {};
			}
		}
	}
	
}
