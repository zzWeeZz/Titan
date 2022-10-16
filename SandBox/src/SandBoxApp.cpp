#include "Titan.h"
#include "Titanium/Editorlayer/EditorLayer.h"


class TestLayer : public Titan::Layer
{
public:
	void OnAttach() override
	{
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
		//m_LayerStack.Push(new Titan::ImGuiLayer());
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