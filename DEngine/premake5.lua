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

IncludeDir = {}
IncludeDir["GLFW"] = "DEngine/vendor/GLFW/include"
IncludeDir["GLAD"] = "DEngine/vendor/GLAD/include"
IncludeDir["ImGUI"] = "DEngine/vendor/imgui"
IncludeDir["glm"] = "DEngine/vendor/glm"

include "DEngine/vendor/GLFW"
include "DEngine/vendor/GLAD"
include "DEngine/vendor/imgui"

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
        "%{prj.name}/src/**.cpp",
        "%{prj.name}/vendor/glm/glm/**.hpp",
        "%{prj.name}/vendor/glm/glm/**.inl"
    }

    includedirs
    {
        "%{prj.name}/src",
        "%{prj.name}/vendor/spdlog/include",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.GLAD}",
        "%{IncludeDir.ImGUI}",
        "%{IncludeDir.glm}"
    }

    links
    {
        "GLFW",
        "opengl32.lib",
        "dwmapi.lib",
        "GLAD",
        "ImGui"
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
            "_CRT_NONSTDC_NO_WARNINGS",
            "GLFW_INCLUDE_NONE"
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
        "DEngine/src",
        "DEngine/vendor",
        "%{IncludeDir.glm}"
    }

    links
    {
        "DEngine"
    }
    
    filter "system:windows"
        cppdialect "C++17"
        staticruntime "Off"
        systemversion "latest"

        defines
        {
            "D_PLATFORM_WINDOWS",
            "_CRT_SECURE_NO_WARNINGS",
            "_CRT_NONSTDC_NO_WARNINGS"
        }

    filter "configurations:Debug"
        defines "D_DEBUG"
        buildoptions "/MDd"
        symbols "On"

    filter "configurations:Release"
        defines "D_RELEASE"
        buildoptions "/MD"
        optimize "On"
        
    filter "configurations:Dist"
        defines "D_DIST"
        buildoptions "/MD"
        optimize "On"