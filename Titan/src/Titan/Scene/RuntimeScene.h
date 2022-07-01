#pragma once
#include "Entity.h"
namespace Titan
{
	class RuntimeScene
	{
		friend class Entity;
	public:
		void OnStart();
		void OnUpdate();
		void OnRender();

	private:
		Snowflake::Registry m_Registry;
	};
}