#include "imgui.h"
#include "Titan.h"
#include "Titanium/Editorlayer/EditorLayer.h"


class TestLayer : public Titan::Layer
{
public:
	Titan::Ref<Titan::Mesh> mesh;
	Titan::Ref<Titan::Model> mesh2;
	Titan::Ref<Titan::Model> mesh3;
	void OnAttach() override
	{
		mesh = Titan::Mesh::Create("Models/Cube.gltf");
		mesh2 = Titan::Model::Create("Models/Cube.gltf");
		mesh3 = Titan::Model::Create("Models/box01.glb");
		mesh2->SetPosition({ 5, 0, 0 });
	}
	void OnUpdate() override
	{
		static float rotValue = 0;
		rotValue += 0.05;
		mesh3->SetRotation({ rotValue / 10, rotValue/20, rotValue / 30 });
		mesh2->SetRotation({ rotValue / 10, 0,0 });
		static float value = 0;
		ImGui::DragFloat("TestValue", &value);
		mesh2->SetPosition({ value, 0, 0 });
		Titan::VulkanRenderer::SubmitMesh(mesh2);
		Titan::VulkanRenderer::SubmitMesh(mesh3);

	}
	void OnDetach() override
	{
	}
	void OnEvent(Titan::Event& e) override
	{
	}
};
class SandBox : public Titan::Application
{
public:
	SandBox()
	{
		m_LayerStack.Push(new TestLayer());
		m_LayerStack.Push(new Titan::ImGuiLayer());
		m_LayerStack.Push(new Titan::EditorLayer());
	}

	~SandBox()
	{

	}
private:
};

Titan::Application* Titan::CreateApplication()
{
	return new SandBox();
}