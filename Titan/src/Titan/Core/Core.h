#pragma once
#include <memory>
#include "Log.h"
#include "TitanMemory.h"


#define TN_CORE_TRACE(...) ::Titan::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define TN_CORE_INFO(...) ::Titan::Log::GetCoreLogger()->info(__VA_ARGS__)
#define TN_CORE_WARN(...) ::Titan::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define TN_CORE_ERROR(...) ::Titan::Log::GetCoreLogger()->error(__VA_ARGS__)

#define TN_TRACE(...) ::Titan::Log::GetClientLogger()->trace(__VA_ARGS__)
#define TN_INFO(...) ::Titan::Log::GetClientLogger()->info(__VA_ARGS__)
#define TN_WARN(...) ::Titan::Log::GetClientLogger()->warn(__VA_ARGS__)
#define TN_ERROR(...) ::Titan::Log::GetClientLogger()->error(__VA_ARGS__)

#define TN_CORE_ASSERT(X, ...) if(!(X)) { TN_CORE_ERROR(__VA_ARGS__); TN_CORE_ERROR("Assertion failed: {0}", #X); exit(1); }

#define TN_DX_CHECK(X) {HRESULT hr = X; if(FAILED(hr)){ assert(false);}}

#define TN_SAFE_RELEASE(X) if( (X) ){ (X)->Release(); (X) = nullptr;}

#define BIT(x) (1 << x)

