#pragma once
#include "Core.h"
#include "Titan/Window.h"
namespace Titan
{
	class Application
	{
	public:
		Application();

		void Run();
		void OnEvent(Event& e);
		virtual ~Application() = default;
	private:
		Scope<Window> m_Window;
		bool m_Running;
	};

	Application* CreateApplication();
}
