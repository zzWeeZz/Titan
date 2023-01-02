#include "TNpch.h"
#include "BindRegistry.h"

namespace Titan
{
	void BindRegistry::Register(const TitanID& id, uint32_t bindIndex)
	{
		m_BindMap[GetStringOfID(id)] = bindIndex;
		m_LookUpMap[bindIndex] = GetStringOfID(id);
	}

	bool BindRegistry::Exists(const TitanID& id, uint32_t& atIndex)
	{
		if (!m_BindMap.contains(GetStringOfID(id)))
		{
			return false;
		}
		atIndex = m_BindMap[GetStringOfID(id)];
		return true;
	}

	bool BindRegistry::Exists(const TitanID& id)
	{
		if (!m_BindMap.contains(GetStringOfID(id)))
		{
			return false;
		}
		return true;
	}
	
	const uint32_t BindRegistry::Fetch(const TitanID& id)
	{
		if (!m_BindMap.contains(GetStringOfID(id)))
		{
			TN_CORE_ERROR("Trying to fetch with id [{}] and it did not exist!", GetStringOfID(id));
			return ~0;
		}
		return m_BindMap[GetStringOfID(id)];
	}
	void BindRegistry::Reset()
	{
		m_BindMap.clear();
		m_LookUpMap.clear();
	}
}