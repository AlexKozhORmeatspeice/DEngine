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

project "DEngine"
	location "DEngine"
	kind "SharedLib"
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
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include"
	}
	
	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"D_PLATFORM_WINDOWS",
			"D_BUILD_DLL"
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
		defines "D_Dist"
		optimize "On"

project "SandBox"
	location "SandBox"
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
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"D_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "D_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "D_RELEASE"
		optimize "On"
		
	filter "configurations:Dist"
		defines "D_Dist"
		optimize "On"
