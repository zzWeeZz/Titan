#pragma once
#include "Titan/Utils/KeyCode.h"
#include "Titan/Utils/MouseCode.h"
namespace Titan
{
	enum class InputMode
	{
		Down,
		Pressed,
		Up
	};

	class Input
	{
	public:
		static bool Key(KeyCode key, InputMode inputMode);
		static bool MouseButton(MouseCode key, InputMode inputMode);
	};
}