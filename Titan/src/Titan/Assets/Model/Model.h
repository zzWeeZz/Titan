#pragma once
#include "Snowflake/Snowflake.hpp"
#include "Titan/Core/Core.h"
#include "Titan/Scene/Components.h"
namespace Titan
{
	COMPONENT(ModelComponent)
	{
		REGISTER_COMPONENT("{564BFFF1-275D-48B8-B94B-32E3A9B138A9}"_guid);
		TitanID modelHandle;
	};

	inline void ModelSystem(ModelComponent& mdl, TransformComponent& tf)
	{

	}
}