-- shaderc is built by CMake. This project hosts that build in the solution.
-- DEngine links build/libshaderc/<Config>/shaderc_combined.lib.
-- The generator instance is Community so the static CRT matches the engine's MSVC.
-- BuildTools ships a newer toolset, and a lib from that toolset does not link.

project "shaderc"
    kind "Makefile"
    language "C++"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "libshaderc/include/shaderc/**.h",
        "libshaderc/include/shaderc/**.hpp",
        "CMakeLists.txt"
    }

    local configure = 'cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_GENERATOR_INSTANCE="C:/Program Files/Microsoft Visual Studio/2022/Community" -DSHADERC_SKIP_TESTS=ON -DSHADERC_SKIP_EXAMPLES=ON -DSHADERC_SKIP_EXECUTABLES=ON -DSHADERC_SKIP_COPYRIGHT_CHECK=ON -DSHADERC_SKIP_INSTALL=ON -DSHADERC_ENABLE_SHARED_CRT=OFF'

    filter "configurations:Debug"
        targetdir "build/libshaderc/Debug"
        targetname "shaderc_combined.lib"
        buildcommands {
            configure .. " && cmake --build build --config Debug --target shaderc_combined -- /m"
        }
        rebuildcommands {
            configure .. " && cmake --build build --config Debug --target shaderc_combined --clean-first -- /m"
        }
        cleancommands { "cmake --build build --config Debug --target clean" }

    filter "configurations:Release"
        targetdir "build/libshaderc/Release"
        targetname "shaderc_combined.lib"
        buildcommands {
            configure .. " && cmake --build build --config Release --target shaderc_combined -- /m"
        }
        rebuildcommands {
            configure .. " && cmake --build build --config Release --target shaderc_combined --clean-first -- /m"
        }
        cleancommands { "cmake --build build --config Release --target clean" }

    filter "configurations:Dist"
        targetdir "build/libshaderc/Release"
        targetname "shaderc_combined.lib"
        buildcommands {
            configure .. " && cmake --build build --config Release --target shaderc_combined -- /m"
        }
        rebuildcommands {
            configure .. " && cmake --build build --config Release --target shaderc_combined --clean-first -- /m"
        }
        cleancommands { "cmake --build build --config Release --target clean" }

    filter {}
