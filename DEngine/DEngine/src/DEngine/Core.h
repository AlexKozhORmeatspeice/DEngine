#pragma once

#include "memory"

#ifdef D_PLATFORM_WINDOWS
    #ifdef D_BUILD_DLL
        #define D_API __declspec(dllexport)
    #elif defined(D_IMPORT_DLL)
        #define D_API __declspec(dllimport)
    #else
        #define D_API  // Static library or no DLL import/export
    #endif
#else
    #error DEngine only supports Windows!
#endif

#ifdef D_DEBUG
	#define D_ENABLE_ASSERTS
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

#define DMESH_FILE_EXT	".dmesh"
#define DMAT_FILE_EXT	".dmat"
#define DACTOR_FILE_EXT	".dactor"

namespace DEngine
{
    template<typename T>
    using Scope = std::unique_ptr<T>;

    template<typename T>
    using Ref = std::shared_ptr<T>;

	template<typename T, typename... Args>
	Scope<T> CreateScope(Args&&... args) {
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T, typename... Args>
	Ref<T> CreateRef(Args&&... args) {
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}
