#include "Titan.h"


class TestLayer : public Titan::Layer
{
public:
	Titan::Ref<Titan::Mesh> mesh;
	Titan::Ref<Titan::Mesh> mesh2;
	Titan::Ref<Titan::Mesh> mesh3;
	void OnAttach() override
	{
		mesh = Titan::Mesh::Create("Models/Cube.gltf");
		mesh2 = Titan::Mesh::Create("Models/Cube.gltf");
		mesh3 = Titan::Mesh::Create("Models/box01.glb");
	}
	void OnUpdate() override
	{
		Titan::VulkanRenderer::SubmitMesh(mesh);
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