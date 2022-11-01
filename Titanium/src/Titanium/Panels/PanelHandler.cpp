#include "PanelHandler.h"
namespace Titan
{
	void PanelHandler::SendCallback()
	{
		for (auto& panel : m_Panels)
		{
			panel.second->OnImGuiDraw();
		}
	}
}