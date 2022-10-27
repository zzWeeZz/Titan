#pragma once
#include <functional>
#include "Snowflake/Snowflake.hpp"
#include <unordered_map>
#define REGISTER_PROPERTIES(GUID, propVec) \
	Titan::PropertyRegistry::Add(GUID, (propVec))
namespace Titan
{
	enum class VaribleType
	{
		Bool,
		Float,
		Vec2,
		Vec3,
		Vec4,
		Quat,
		Str,
	};

	class PropertyRegistry
	{
	public:
		typedef std::vector<VaribleType> PropertyList;
		typedef std::unordered_map<SnowID, PropertyList> FactoryMap;

		static void Add(const SnowID& name, PropertyList func)
		{
			auto& map = GetFactoryMap();
			if (map.find(name) != map.end())
			{
				return;
			}
			map[name] = func;
		}

		static PropertyList GetPropertyList(SnowID& id)
		{
			auto map = GetFactoryMap();
			return map[id];
		}
	private:
		static FactoryMap& GetFactoryMap()
		{
			static FactoryMap map;
			return map;
		}
	};
}