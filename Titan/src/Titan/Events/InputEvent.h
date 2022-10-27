#pragma once
#include <sstream>

#include "Titan/Events/Event.h"

namespace Titan
{
	class MouseMoveEvent : public Event
	{
	public:
		MouseMoveEvent(double x, double y)
		{
			m_X = x;
			m_Y = y;
		}

		std::pair<double, double> GetMousePos() { return { m_X, m_Y }; }

		EVENT_CLASS_TYPE(MouseMoved)
			EVENT_CLASS_CATEGORY(EventCategoryMouse)
	private:
		double m_X;
		double m_Y;
	};
}