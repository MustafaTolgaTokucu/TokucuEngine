#pragma once

#ifdef TKC_PLATFORM_WINDOWS
	#ifdef TKC_BUILD_DLL
		#define TOKUCU_API __declspec(dllexport)
	#elif defined(TOKUCU_USE_DLL)
		#define TOKUCU_API __declspec(dllimport)
	#else
		#define TOKUCU_API
	#endif // DEBUG
#else 
	#error Tokucu only supports Windows!
#endif // DEBUG

#ifdef TKC_ENABLE_ASSERTS
	#define TKC_ASSERT(x,...) {if(!(X)){TKC_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak();} }
	#define TKC_CORE_ASSERT(x, ...) { if (!(X)) { TKC_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else 
	#define TKC_ASSERT(x,...)
	#define TKC_CORE_ASSERT(x,...)
 
#endif // TKC_ENABLE_ASSERTS


#define BIT(x) (1<<x)

#define TKC_BIND_EVENT_FN(fn) std::bind(&fn , this, std::placeholders::_1)

