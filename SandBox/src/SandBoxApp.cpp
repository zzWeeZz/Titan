#include "Titan.h"


class TestLayer : public Titan::Layer
{
public:
	Titan::Mesh mesh;
	void OnAttach() override
	{
		std::vector<Titan::Vertex> vex;
		Titan::AssetImporter::ImportModel("Models/Cube.gltf", vex);
		mesh.m_Vertices = vex;

		Titan::GraphicsContext::UploadMesh(mesh);
	}
	void OnUpdate() override
	{
		Titan::VulkanRenderer::SubmitMesh(mesh);
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