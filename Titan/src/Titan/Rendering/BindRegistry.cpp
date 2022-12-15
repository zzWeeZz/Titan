#include "TNpch.h"
#include "BindRegistry.h"

namespace Titan
{
	void BindRegistry::Register(const TitanID& id, uint32_t bindIndex)
	{
		m_BindMap[id] = bindIndex;
		m_LookUpMap[bindIndex] = id;
	}
	bool BindRegistry::Exists(const TitanID& id, uint32_t& atIndex)
	{
		if (!m_BindMap.contains(id))
		{
			return false;
		}
		atIndex = m_BindMap[id];
		return true;
	}
	const uint32_t BindRegistry::Fetch(const TitanID& id)
	{
		if (!m_BindMap.contains(id))
		{
			TN_CORE_ERROR("Trying to fetch with id [{}] and it did not exist!", GetStringOfID(id));
			return ~0;
		}
		return m_BindMap[id];
	}
	void BindRegistry::Reset()
	{
		m_BindMap.clear();
		m_LookUpMap.clear();
	}
}