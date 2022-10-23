#pragma once
#include <memory>
#include "Log.h"
#include "TitanMemory.h"
#include "comdef.h"
#include "vulkan/vulkan.h"
#include "TitanID.h"
#include "PlatformDetection.h"

constexpr uint32_t g_FramesInFlight = 3;
template<typename T>
using PreFrameInFlight = std::array<T, g_FramesInFlight>;

#define TN_BIND_FUNC(X) std::bind(&X, this, std::placeholders::_1)

#define TN_CORE_TRACE(...) ::Titan::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define TN_CORE_INFO(...) ::Titan::Log::GetCoreLogger()->info(__VA_ARGS__)
#define TN_CORE_WARN(...) ::Titan::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define TN_CORE_ERROR(...) ::Titan::Log::GetCoreLogger()->error(__VA_ARGS__)

#define TN_TRACE(...) ::Titan::Log::GetClientLogger()->trace(__VA_ARGS__)
#define TN_INFO(...) ::Titan::Log::GetClientLogger()->info(__VA_ARGS__)
#define TN_WARN(...) ::Titan::Log::GetClientLogger()->warn(__VA_ARGS__)
#define TN_ERROR(...) ::Titan::Log::GetClientLogger()->error(__VA_ARGS__)

#define TN_CORE_ASSERT(X, ...) if(!(X)) { TN_CORE_ERROR(__VA_ARGS__); TN_CORE_ERROR("Assertion failed: {0}", #X); assert(false); }

inline std::string errorString(VkResult errorCode)
{
	switch (errorCode)
	{
#define STR(r) case VK_ ##r: return #r
		STR(NOT_READY);
		STR(TIMEOUT);
		STR(EVENT_SET);
		STR(EVENT_RESET);
		STR(INCOMPLETE);
		STR(ERROR_OUT_OF_HOST_MEMORY);
		STR(ERROR_OUT_OF_DEVICE_MEMORY);
		STR(ERROR_INITIALIZATION_FAILED);
		STR(ERROR_DEVICE_LOST);
		STR(ERROR_MEMORY_MAP_FAILED);
		STR(ERROR_LAYER_NOT_PRESENT);
		STR(ERROR_EXTENSION_NOT_PRESENT);
		STR(ERROR_FEATURE_NOT_PRESENT);
		STR(ERROR_INCOMPATIBLE_DRIVER);
		STR(ERROR_TOO_MANY_OBJECTS);
		STR(ERROR_FORMAT_NOT_SUPPORTED);
		STR(ERROR_SURFACE_LOST_KHR);
		STR(ERROR_NATIVE_WINDOW_IN_USE_KHR);
		STR(SUBOPTIMAL_KHR);
		STR(ERROR_OUT_OF_DATE_KHR);
		STR(ERROR_INCOMPATIBLE_DISPLAY_KHR);
		STR(ERROR_VALIDATION_FAILED_EXT);
		STR(ERROR_INVALID_SHADER_NV);
#undef STR
	default:
		return "UNKNOWN_ERROR";
	}
}

#define TN_VK_CHECK(X) do { VkResult error = X; if(error) { TN_CORE_ERROR("Fatal : VkResult is {0} in {1} at line {2}", errorString(error).c_str(), __FILE__, __LINE__); exit(1); } } while(0)

#define TN_DX_CHECK(X) {HRESULT hr = X; if(FAILED(hr)){ TN_CORE_ASSERT(false,std::system_category().message(hr).c_str());}}

#define TN_SAFE_RELEASE(X) if( (X) ){ (X).Reset(); (X) = nullptr;}

#define BIT(x) (1 << x)