#pragma once
#include "Titanium/Panels/Panel.h"
namespace Titan
{
	class RenderingStatsPanel : public Panel
	{
	public:
		// Inherited via Panel
		virtual void OnImGuiDraw() override;
	};
}
