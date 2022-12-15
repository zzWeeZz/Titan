#pragma once
#include <unordered_map>

#include "Titan/Core/TitanID.h"

namespace Titan
{
	class BindRegistry
	{
	public:
		void Register(const TitanID& id, uint32_t bindIndex);
		bool Exists(const TitanID& id, uint32_t& atIndex);
		const uint32_t Fetch(const TitanID& id);
		void Reset();
	private:

		std::unordered_map<std::string, uint32_t> m_BindMap;
		std::unordered_map<uint32_t, std::string> m_LookUpMap;
	};
}