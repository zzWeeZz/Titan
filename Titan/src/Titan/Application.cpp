#include "Application.h"

#include <iostream>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

#include "Core/Log.h"
#include "Events/ApplicationEvent.h"
#include "Rendering/RenderAPI.h"
#include "Rendering/VulkanAPI/GraphicsContext.h"

Titan::Application::Application() : m_Running(true)
{
	s_Window = Ref<Window>(Window::Create());
	s_Window->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));
	RenderAPI::Initialize(API::Vulkan);
}

void Titan::Application::Run()
{
	while (m_Running)
	{
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		for (auto& layer : m_LayerStack)
		{
			layer->OnUpdate();
		}
		s_Window->OnUpdate();
		ImGui::Render();
		RenderAPI::Begin();
	}
	RenderAPI::Shutdown();
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
