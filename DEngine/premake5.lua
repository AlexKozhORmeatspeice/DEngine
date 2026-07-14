workspace "DEngine"
    architecture "x64"
    startproject "DEditor"
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
IncludeDir["stb_image"] = "DEngine/vendor/stb_image"
IncludeDir["entt"] = "DEngine/vendor/entt/include"
IncludeDir["yaml"] = "DEngine/vendor/yaml-cpp/include"
IncludeDir["assimp"] = "DEngine/vendor/assimp/include"

group "Dependencies"
    include "DEngine/vendor/GLFW"
    include "DEngine/vendor/GLAD"
    include "DEngine/vendor/imgui"
    include "DEngine/vendor/yaml-cpp"
group ""

project "DEngine"
    location "DEngine"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"
    
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "dpch.h"
    pchsource "DEngine/src/dpch.cpp"

    files 
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",
        "%{prj.name}/vendor/stb_image/**.cpp",
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
        "%{IncludeDir.glm}",
        "%{IncludeDir.stb_image}",
        "%{IncludeDir.entt}",
        "%{IncludeDir.yaml}",
        "%{IncludeDir.assimp}"
    }

    libdirs
    {
        "%{prj.name}/vendor/assimp/lib"
    }

    links
    {
        "GLFW",
        "GLAD",
        "ImGui",
        "yaml-cpp",
        "assimp-vc143-mt"
    }
    
    filter "system:windows"
        systemversion "latest"

        links
        {
            "opengl32.lib",
            "dwmapi.lib",
            "gdi32.lib",
            "user32.lib",
            "winmm.lib",
            "advapi32.lib"
        }

        defines
        {
            "D_PLATFORM_WINDOWS",
            "GLFW_INCLUDE_NONE",
            "_CRT_SECURE_NO_WARNINGS",
            "_CRT_NONSTDC_NO_WARNINGS",
            "YAML_CPP_STATIC_DEFINE"
        }

    filter "configurations:Debug"
        defines "D_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "D_RELEASE"
        runtime "Release"
        optimize "on"
        
    filter "configurations:Dist"
        defines "D_DIST"
        runtime "Release"
        optimize "on"

project "Sandbox"
    location "Sandbox"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"
    
    targetdir ("bin/" ..outputdir.. "/%{prj.name}")
    objdir ("bin-int/" ..outputdir.. "/%{prj.name}")

    files 
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs
    {
        "%{prj.name}/src",
        "DEngine/vendor/spdlog/include",
        "DEngine/src",
        "DEngine/vendor",
        "%{IncludeDir.glm}",
        "%{IncludeDir.entt}"
    }

    links
    {
        "DEngine"
    }
    
    filter "system:windows"
        systemversion "latest"

        defines
        {
            "D_PLATFORM_WINDOWS",
            "_CRT_SECURE_NO_WARNINGS",
            "_CRT_NONSTDC_NO_WARNINGS"
        }

        postbuildcommands
        {
            "{COPY} %{wks.location}/DEngine/vendor/assimp/dll/assimp-vc143-mt.dll %{cfg.targetdir}"
        }

    filter "configurations:Debug"
        defines "D_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "D_RELEASE"
        runtime "Release"
        optimize "on"
        
    filter "configurations:Dist"
        defines "D_DIST"
        runtime "Release"
        optimize "on"

project "DEditor"
    location "DEditor"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"
    
    targetdir ("bin/" ..outputdir.. "/%{prj.name}")
    objdir ("bin-int/" ..outputdir.. "/%{prj.name}")

    files 
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs
    {
        "%{prj.name}/src",
        "DEngine/vendor/spdlog/include",
        "DEngine/src",
        "DEngine/vendor",
        "%{IncludeDir.glm}",
        "%{IncludeDir.entt}"
    }

    links
    {
        "DEngine"
    }
    
    filter "system:windows"
        systemversion "latest"

        defines
        {
            "D_PLATFORM_WINDOWS",
            "_CRT_SECURE_NO_WARNINGS",
            "_CRT_NONSTDC_NO_WARNINGS"
        }

        postbuildcommands
        {
            "{COPY} %{wks.location}/DEngine/vendor/assimp/dll/assimp-vc143-mt.dll %{cfg.targetdir}"
        }

    filter "configurations:Debug"
        defines "D_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "D_RELEASE"
        runtime "Release"
        optimize "on"
        
    filter "configurations:Dist"
        defines "D_DIST"
        runtime "Release"
        optimize "on"