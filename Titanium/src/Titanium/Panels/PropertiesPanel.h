#pragma once
#include <Titan/Scene/Entity.h>
namespace Titan
{
	class PropertiesPanel
	{
	public:
		void Inspect(Entity entity);
		void ImGuiBeginRender();
	private:
		Entity m_InspectedItem;
	};
}