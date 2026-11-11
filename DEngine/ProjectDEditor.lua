project "DEditor"
    location "DEditor"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",
    }

    includedirs
    {
        "%{prj.name}/src",
        "DEngine/vendor/spdlog/include",
        "DEngine/src",
        "DEngine/vendor",
        "%{IncludeDir.glm}",
        "%{IncludeDir.entt}",
        "%{IncludeDir.yaml}",
        "%{IncludeDir.ImGuizmo}",
        "%{IncludeDir.tracy}",
        "%{IncludeDir.enkiTS}",
    }

    links
    {
        "DEngine",
        "enkiTS"
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
            "mkdir \"%{cfg.targetdir}/assets\" 2>nul",
            "mkdir \"%{cfg.targetdir}/resources\" 2>nul",

            "{COPY} %{wks.location}/DEngine/vendor/assimp/dll/assimp-vc143-mt.dll %{cfg.targetdir}",
            "{COPY} %{wks.location}/DEditor/assets %{cfg.targetdir}/assets",
            "{COPY} %{wks.location}/DEditor/resources %{cfg.targetdir}/resources"
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
