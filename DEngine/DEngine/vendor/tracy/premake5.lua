project "Tracy"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"
    warnings "off"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "public/TracyClient.cpp"
    }

    includedirs
    {
        "public"
    }

    defines
    {
        "_CRT_SECURE_NO_WARNINGS"
    }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "Full"
        editandcontinue "Off"
        defines { "TRACY_ENABLE", "TRACY_ON_DEMAND" }

    filter "configurations:Release"
        runtime "Release"
        optimize "on"
        defines { "TRACY_ENABLE", "TRACY_ON_DEMAND" }

    filter "configurations:Dist"
        runtime "Release"
        optimize "on"

    filter {}
