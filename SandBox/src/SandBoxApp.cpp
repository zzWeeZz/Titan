#include "Titan.h"


class TestLayer : public Titan::Layer
{
public:
	void OnAttach() override
	{
		Titan::GLTFImporter::Import("Models/Cube.bin");
	}
	void OnUpdate() override
	{
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