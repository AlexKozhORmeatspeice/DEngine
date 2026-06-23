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
