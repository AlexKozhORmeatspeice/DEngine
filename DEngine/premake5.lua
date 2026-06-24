workspace "DEngine"
    architecture "x64"
    startproject "Sandbox"
    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- INCLUDE dirs relative to root folder (solution dir)
IncludeDir = {}
IncludeDir["GLFW"] = "DEngine/vendor/GLFW/include"

include "DEngine/vendor/GLFW"

project "DEngine"
    location "DEngine"
    kind "SharedLib"
    language "C++"
    
    targetdir ("bin/" ..outputdir.. "/%{prj.name}")
    objdir ("bin-int/" ..outputdir.. "/%{prj.name}")

    pchheader "dpch.h"
    pchsource "DEngine/src/dpch.cpp"

    files 
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs
    {
        "%{prj.name}/src",
        "%{prj.name}/vendor/spdlog/include",
        "%{IncludeDir.GLFW}"
    }

    links
    {
        "GLFW",
        "opengl32.lib",
        "dwmapi.lib"
    }
    
    filter "system:windows"
        cppdialect "C++17"
        staticruntime "Off"  -- ИЗМЕНЕНО: используем динамическую CRT
        systemversion "latest"
        
        -- Добавляем недостающие библиотеки
        links
        {
            "opengl32.lib",
            "dwmapi.lib",
            "gdi32.lib",      -- Добавляем GDI
            "user32.lib",     -- Добавляем User32
            "winmm.lib",      -- Добавляем Windows Multimedia
            "advapi32.lib"    -- Добавляем Advanced API
        }

        defines
        {
            "D_PLATFORM_WINDOWS",
            "D_BUILD_DLL",
            "_CRT_SECURE_NO_WARNINGS",
            "_CRT_NONSTDC_NO_WARNINGS"
        }

        postbuildcommands
        {
            ("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
        }

    filter "configurations:Debug"
        defines "D_DEBUG"
        symbols "On"

    filter "configurations:Release"
        defines "D_RELEASE"
        optimize "On"
        
    filter "configurations:Dist"
        defines "D_DIST"
        optimize "On"

project "Sandbox"
    location "Sandbox"
    kind "ConsoleApp"
    language "C++"
    
    targetdir ("bin/" ..outputdir.. "/%{prj.name}")
    objdir ("bin-int/" ..outputdir.. "/%{prj.name}")

    files 
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs
    {
        "DEngine/vendor/spdlog/include",
        "DEngine/src"
    }

    links
    {
        "DEngine"
    }
    
    filter "system:windows"
        cppdialect "C++17"
        staticruntime "Off"  -- ИЗМЕНЕНО: синхронизируем с DEngine
        systemversion "latest"

        defines
        {
            "D_PLATFORM_WINDOWS",
            "_CRT_SECURE_NO_WARNINGS",
            "_CRT_NONSTDC_NO_WARNINGS"
        }

    filter "configurations:Debug"
        defines "D_DEBUG"
        symbols "On"

    filter "configurations:Release"
        defines "D_RELEASE"
        optimize "On"
        
    filter "configurations:Dist"
        defines "D_DIST"
        optimize "On"