#pragma once
#include <map>
#include <any>
#include <string>

#ifndef TN_PROFILE_NO_OPTICK
#include "Optick/src/optick.h"

#ifndef TN_CONFIG_DIST
#define TN_PROFILE_FRAME(...) OPTICK_FRAME(__VA_ARGS__)
#define TN_PROFILE_SCOPE(NAME) OPTICK_EVENT_DYNAMIC(NAME)
#define TN_PROFILE_FUNCTION(...) OPTICK_EVENT(__VA_ARGS__)
#define TN_PROFILE_THREAD(NAME) OPTICK_THREAD(NAME)
#define TN_PROFILE_CONTEXT(...) OPTICK_GPU_CONTEXT(__VA_ARGS__)
#define TN_PROFILE_GPU_EVENT(NAME) OPTICK_GPU_EVENT(NAME)
#else
#define TN_PROFILE_FRAME(...)
#define TN_PROFILE_SCOPE(NAME)
#define TN_PROFILE_FUNCTION(...)
#define TN_PROFILE_THREAD(NAME)
#define TN_PROFILE_CONTEXT(...)
#define TN_PROFILE_GPU_EVENT(NAME)
#endif

#endif
namespace Titan
{
	class Profiler
	{
	public:

		class Scope
		{
		public:
			Scope(std::string_view key);
			~Scope();
		private:
			std::string_view m_Key;
			float m_Time;
		};

		template<typename T>
		static void PofileDataAdd(std::string_view key, const T& addValue);
		template<typename T>
		static void PofileDataSub(std::string_view key, const T& subValue);
		template<typename T>
		static void PofileDataSet(std::string_view key, const T& setValue);
		template<typename T>
		static const T PofileDataGet(std::string_view key);
	private:
		template<typename T>
		static T* GetDataFromAny(std::string_view key);

		static inline std::map<std::string_view, std::any> s_ProfilingData;
	};
	template<typename T>
	inline void Profiler::PofileDataAdd(std::string_view key, const T& addValue)
	{
		*GetDataFromAny<T>(key) += addValue;
	}
	template<typename T>
	inline void Profiler::PofileDataSub(std::string_view key, const T& subValue)
	{
		*GetDataFromAny<T>(key) -= subValue;
	}
	template<typename T>
	inline void Profiler::PofileDataSet(std::string_view key, const T& setValue)
	{
		*GetDataFromAny<T>(key) = setValue;
	}
	template<typename T>
	inline const T Profiler::PofileDataGet(std::string_view key)
	{
		return *GetDataFromAny<T>(key);
	}
	template<typename T>
	inline T* Profiler::GetDataFromAny(std::string_view key)
	{
		if (!s_ProfilingData.contains(key))
		{
			s_ProfilingData[key] = std::make_any<T>();
		}

		return std::any_cast<T>(&s_ProfilingData[key]);
	}
}