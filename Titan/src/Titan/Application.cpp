#include "TNpch.h"
#include "Application.h"

#include <iostream>

#include "Titan/Rendering/GraphicsContext.h"
#include "Core/Log.h"
#include "Events/ApplicationEvent.h"
#include "Titan/Rendering/Renderer.h"

Titan::Application::Application() : m_Running(true)
{
	s_Window = Ref<Window>(Window::Create());
	s_Window->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));
	GraphicsContextInfo info{ };
	info.width = s_Window->GetWidth();
	info.height = s_Window->GetHeight();
	GraphicsContext::Initialize(info);
	Renderer::Initialize();
}

void Titan::Application::Run()
{
	while (m_Running)
	{
		for (const auto& layer : m_LayerStack)
		{
			layer->OnUpdate();
		}
		s_Window->OnUpdate();
		Renderer::Begin();
		Renderer::End();

		Renderer::DrawCommands();
	}
	
	Renderer::Shutdown();
}

void Titan::Application::OnEvent(Event& e)
{
	if (e.GetEventType() == EventType::WindowClose)
	{
		m_Running = false;
	}
	for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
	{
		(*--it)->OnEvent(e);
		if (e.Handled)
			break;
	}
}

void Titan::Application::PushLayer(Layer* layer)
{
	m_LayerStack.Push(layer);
}

void Titan::Application::PushOverlay(Layer* overlay)
{
	m_LayerStack.PushOverlay(overlay);
}
