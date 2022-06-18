#include "Application.h"

#include <iostream>

#include "Core/Log.h"
#include "Events/ApplicationEvent.h"

Titan::Application::Application()
{
	m_Window = Scope<Window>(Window::Create());
	m_Window->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));
	m_Running = true;
}

void Titan::Application::Run()
{
	while (m_Running)
	{
		m_Window->OnUpdate();
	}
}

void Titan::Application::OnEvent(Event& e)
{
	TITAN_CORE_INFO("{0}", e);
}
