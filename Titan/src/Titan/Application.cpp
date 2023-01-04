#include "TNpch.h"
#include "Application.h"

#include <iostream>

#include "Titan/Rendering/GraphicsContext.h"
#include "Core/Log.h"
#include "Events/ApplicationEvent.h"
#include "Titan/Rendering/Renderer.h"
#include "Titan/Assets/ResourceRegistry.h"
#include <Optick/src/optick.h>
#include "Titan/ImGui/TitanImGui.h"
#include "Titan/Utils/TitanAllocator.h"
#include "Titan/Utils/ThreadPool.h"
#include "Titan/Utils/Chrono.h"

static void ThreadCallback(std::string message, Titan::Severity severity)
{
	TN_CORE_INFO(message);
}

Titan::Application::Application() : m_Running(true)
{
	s_Window = Ref<Window>(Window::Create());
	s_Window->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));
	ThreadPool::Initialize(ThreadCallback);
	GraphicsContextInfo info{ };
	info.width = s_Window->GetWidth();
	info.height = s_Window->GetHeight();
	GraphicsContext::Initialize(info);
	TitanAllocator::Initialize();
	Renderer::Initialize();
	TitanImGui::Initialize();
	ResourceRegistry::Initialize();
}

void Titan::Application::Run()
{
	while (m_Running)
	{
		Chrono::UpdateTimeData();
		TitanImGui::Begin();
		OPTICK_FRAME("MainThread");
		Renderer::WaitOnNewFrame();
		for (const auto& layer : m_LayerStack)
		{
			layer->OnUpdate();
		}
		s_Window->OnUpdate();
		Renderer::Begin();
	}
	GraphicsContext::WaitOnAllFences();

	ResourceRegistry::Dump();
	
	GraphicsContext::GetSwapchain().CleanUp();
	Renderer::Shutdown();
	TitanAllocator::Flush();
	TitanAllocator::Shutdown();
	GraphicsContext::Shutdown();
	ThreadPool::Shutdown();
}

void Titan::Application::OnEvent(Event& e)
{
	if (e.GetEventType() == EventType::WindowClose)
	{
		m_Running = false;
	}
	if (e.GetEventType() == EventType::WindowResize)
	{
		auto& resize = *reinterpret_cast<WindowResizeEvent*>(&e);
		GraphicsContext::GetSwapchain().Resize(resize.GetWidth(), resize.GetHeight());
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
