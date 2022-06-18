#pragma once
#include "Core.h"
#include "LayerStack.h"
#include "Titan/Window.h"
#include <sstream>
namespace Titan
{
	class Application
	{
	public:
		Application();

		void Run();
		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		virtual ~Application(){}
	protected:
		Scope<Window> m_Window;
		bool m_Running;
		LayerStack m_LayerStack;
	};

	Application* CreateApplication();
}
