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
        "%{prj.name}/vendor/glm/glm/**.inl",
        "%{prj.name}/vendor/ImGuizmo/ImGuizmo.h",
        "%{prj.name}/vendor/ImGuizmo/ImGuizmo.cpp"
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
        "%{IncludeDir.assimp}",
        "%{IncludeDir.ImGuizmo}",
        "%{IncludeDir.tracy}",
        "%{IncludeDir.enkiTS}",
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
        "assimp-vc143-mt",
        "Tracy",
        "enkiTS"
    }

    filter "files:vendor/ImGuizmo/**.cpp"
        flags {"NoPCH"}

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

EnableTracy()
