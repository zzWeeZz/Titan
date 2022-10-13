#pragma once
#include "TNpch.h"
#include <Titan/Assets/Asset.h>
namespace Titan
{
	class ResourceRegistry
	{
	public:
		static void Initialize();
		template<typename T>
		static Ref<T> GetItem(TitanID id);
	private:
		inline static std::unordered_map<TitanID, Ref<Asset>> s_Registry;
	};
}