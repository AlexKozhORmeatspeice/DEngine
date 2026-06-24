#pragma once

#ifdef D_PLATFORM_WINDOWS
    #ifdef D_BUILD_DLL
        #define D_API __declspec(dllexport)
    #else
        #define D_API __declspec(dllimport)
    #endif
#else
    #error DEngine only support Windows!
#endif

#ifdef D_ENABLE_ASSERTS
	#define D_ASSERT(x, ...) { if(!(x)) { D_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define D_CORE_ASSERT(x, ...) { if(!(x)) { D_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define D_ASSERT(x, ...)
	#define D_CORE_ASSERT(x, ...)
#endif

#define BIT(X) (1 << X)
#define BIND_EVENT_FN(X) std::bind(&X, this, std::placeholders::_1)
