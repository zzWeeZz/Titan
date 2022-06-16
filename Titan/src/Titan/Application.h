#pragma once
#include "Core.h"

namespace Titan
{
	class Application
	{
	public:
		Application() = default;

		void Run();
		virtual ~Application() = default;
	};

	Application* CreateApplication();
}
