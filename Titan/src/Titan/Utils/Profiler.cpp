#include "TNpch.h"
#include "Profiler.h"

#include "Titan/Utils/Chrono.h"
namespace Titan
{
	Profiler::Scope::Scope(std::string_view key)
	{
		m_Key = key;
		m_Time = Chrono::TotalTime();
	}
	Profiler::Scope::~Scope()
	{
		m_Time = Chrono::TotalTime() - m_Time;
		Profiler::PofileDataSet(m_Key, m_Time);
	}
}
