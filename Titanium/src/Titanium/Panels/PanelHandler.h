#pragma once
#include <map>
#include <string>
#include <Titanium/Panels/Panel.h>
#include "Titan/Core/Core.h"
namespace Titan
{
	class PanelHandler
	{
	public:
		PanelHandler() = default;
		template<class T>
		void Add(const std::string& key, Ref<T> panel);
		template<class T>
		Ptr<T> Get(const std::string& key);
		void SendCallback();
	private:
		std::map<std::string, Ref<Panel>> m_Panels;
	};
	template<class T>
	inline void PanelHandler::Add(const std::string& key, Ref<T> panel)
	{
		m_Panels[key] = std::reinterpret_pointer_cast<Panel>(panel);
	}
	template<class T>
	inline Ptr<T> PanelHandler::Get(const std::string& key)
	{
		if (!m_Panels.contains(key))
		{
			m_Panels[key] = nullptr;
		}
		return std::reinterpret_pointer_cast<T>(m_Panels[key]);
	}
}