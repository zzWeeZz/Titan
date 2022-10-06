#include "TNpch.h"
#include "WindowsWindow.h"

#include "Titan/Core/Core.h"
#include "Titan/Core/Log.h"
#include "Titan/Events/ApplicationEvent.h"

namespace Titan
{
	static bool s_Initialized = false;
	Window* Window::Create(const WindowCreateInfo& props)
	{
		return new WindowsWindow(props);
	}

	WindowsWindow::WindowsWindow(const WindowCreateInfo& props)
	{
		Initialize(props);
	}

	void WindowsWindow::OnUpdate()
	{
		glfwPollEvents();
		glfwSwapBuffers(m_Window);
	}

	void WindowsWindow::Initialize(const WindowCreateInfo& createInfo)
	{
		m_WindowInfo.Title = createInfo.Title;
		m_WindowInfo.Width = createInfo.Width;
		m_WindowInfo.Height = createInfo.Height;
		m_WindowInfo.VSync = createInfo.VSync;

		TN_CORE_INFO("Creating window {0}x{1}", m_WindowInfo.Width, m_WindowInfo.Height);

		if (!s_Initialized)
		{
			int success = glfwInit();
			TN_CORE_ASSERT(success, "Could not initialize GLFW!");
			s_Initialized = true;
		}
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		m_Window = glfwCreateWindow(m_WindowInfo.Width, m_WindowInfo.Height, m_WindowInfo.Title.c_str(), nullptr, nullptr);
		glfwMakeContextCurrent(m_Window);
		glfwSetWindowUserPointer(m_Window, &m_WindowInfo);
		SetVSync(true);

		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
			{
				WindowInfo* info = static_cast<WindowInfo*>(glfwGetWindowUserPointer(window));
				info->Width = width;
				info->Height = height;

				WindowResizeEvent event(width, height);
				info->EventCallback(event);
			});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
			{
				WindowInfo* info = static_cast<WindowInfo*>(glfwGetWindowUserPointer(window));
				WindowCloseEvent event;
				info->EventCallback(event);
			});

	}

	void WindowsWindow::Shutdown()
	{
		glfwDestroyWindow(m_Window);
	}
}
