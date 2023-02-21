#pragma once
#include "TNpch.h"
#include <Titan/Assets/Resource.h>
namespace Titan
{
	constexpr TitanID InvalidID = TitanID();
	class ResourceRegistry
	{
	public:
		static void Initialize();
		template<typename T>
		static Ref<T> GetItem(TitanID& id);
		static void Dump();
		static void Load();
	private:
		inline static std::unordered_map<TitanID, Ref<Resource>> s_Registry;
		inline static std::unordered_map<TitanID, std::filesystem::path> s_CachedRegistry;
	};
	template<typename T>
	inline Ref<T> ResourceRegistry::GetItem(TitanID& id)
	{
		if (id == InvalidID)
		{
			id = CreateID();
		}
		const auto strId = (id);
		if (s_Registry.contains(strId))
		{
			return std::reinterpret_pointer_cast<T>(s_Registry[strId]);
		}

		s_Registry[strId] = std::reinterpret_pointer_cast<Resource>(T::Create());
		if (s_CachedRegistry.contains(strId))
		{
			s_Registry[strId]->Initialize(s_CachedRegistry[strId]);
		}
		return std::reinterpret_pointer_cast<T>(s_Registry[strId]);
	}
}