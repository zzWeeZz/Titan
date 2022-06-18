#pragma once
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "Titan/Core.h"

namespace Titan
{
	class Log
	{
	public:
		Log() = default;
		static void Initialize();

		inline static Ref<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static Ref<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	private:
		static Ref<spdlog::logger> s_CoreLogger;
		static Ref<spdlog::logger> s_ClientLogger;
	};
}

#define TITAN_CORE_TRACE(...) ::Titan::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define TITAN_CORE_INFO(...) ::Titan::Log::GetCoreLogger()->info(__VA_ARGS__)
#define TITAN_CORE_WARN(...) ::Titan::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define TITAN_CORE_ERROR(...) ::Titan::Log::GetCoreLogger()->error(__VA_ARGS__)

#define TITAN_TRACE(...) ::Titan::Log::GetClientLogger()->trace(__VA_ARGS__)
#define TITAN_INFO(...) ::Titan::Log::GetClientLogger()->info(__VA_ARGS__)
#define TITAN_WARN(...) ::Titan::Log::GetClientLogger()->warn(__VA_ARGS__)
#define TITAN_ERROR(...) ::Titan::Log::GetClientLogger()->error(__VA_ARGS__)
