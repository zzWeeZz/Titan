#pragma once
#include "Titan/Window.h"
#include "GLFW/glfw3.h"

namespace Titan
{
	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowCreateInfo& props);

		void OnUpdate() override;

		inline uint32_t GetWidth() const override { return m_WindowInfo.Width; }
		inline uint32_t GetHeight() const override{ return m_WindowInfo.Height; }
		inline void SetVSync(bool enabled) override { m_WindowInfo.VSync = enabled; }
		inline bool IsVSync() const override {return m_WindowInfo.VSync;}

		void SetEventCallback(const EventCallbackFn& callback) override { m_WindowInfo.EventCallback = callback; }
	private:
		virtual void Initialize(const WindowCreateInfo& createInfo);
		virtual void Shutdown();

		GLFWwindow* m_Window;

		struct WindowInfo
		{
			std::string Title = "Titan Engine";
			uint32_t Width = 1280;
			uint32_t Height = 720;
			bool VSync = false;

			EventCallbackFn EventCallback;
		} m_WindowInfo;
	};
}

