#include "TNpch.h"
#include "Input.h"
#include "Titan/Application.h"
#include "Titan/Platform/WindowsWindow.h"
namespace Titan
{
	bool Input::Key(KeyCode key, InputMode inputMode)
	{
		auto* window = (GLFWwindow*)Application::GetWindow().GetNativeWindow();
		auto state = glfwGetKey(window, (int32_t)key);
		switch (inputMode)
		{
		case Titan::InputMode::Down:
			return state == GLFW_PRESS;
			break;
		case Titan::InputMode::Pressed:
			return state == GLFW_PRESS;
			break;
		case Titan::InputMode::Up:
			return state == GLFW_RELEASE;
			break;
		default:
			break;
		}
	}
	bool Input::MouseButton(MouseCode key, InputMode inputMode)
	{
		auto* window = (GLFWwindow*)Application::GetWindow().GetNativeWindow();
		auto state = glfwGetMouseButton(window, (int32_t)key);
		switch (inputMode)
		{
		case Titan::InputMode::Down:
			return state == GLFW_REPEAT;
			break;
		case Titan::InputMode::Pressed:
			return state == GLFW_PRESS;
			break;
		case Titan::InputMode::Up:
			return state == GLFW_RELEASE;
			break;
		default:
			break;
		}
	}
}