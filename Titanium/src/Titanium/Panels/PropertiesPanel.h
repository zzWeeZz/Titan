#pragma once
#include <Titan/Scene/Entity.h>
#include "Titanium/Panels/Panel.h"
namespace Titan
{
	class PropertiesPanel : public Panel
	{
	public:
		void Inspect(Entity entity);
		// Inherited via Panel
		virtual void OnImGuiDraw() override;
	private:
		Entity m_InspectedItem;

		
	};
}