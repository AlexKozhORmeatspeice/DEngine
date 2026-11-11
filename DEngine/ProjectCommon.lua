-- Shared by every solution script. Include this before any project.

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
IncludeDir["ImGuizmo"] = "DEngine/vendor/ImGuizmo"
IncludeDir["tracy"] = "DEngine/vendor/tracy/public"
IncludeDir["enkiTS"] = "DEngine/vendor/enkiTS/src"

function EnableTracy()
    filter "configurations:Debug"
        defines { "TRACY_ENABLE", "TRACY_ON_DEMAND" }

    filter "configurations:Release"
        defines { "TRACY_ENABLE", "TRACY_ON_DEMAND" }

    filter {}
end

function IncludeEngineDependencies()
    group "Dependencies"
        include "DEngine/vendor/GLFW"
        include "DEngine/vendor/GLAD"
        include "DEngine/vendor/imgui"
        include "DEngine/vendor/yaml-cpp"
        include "DEngine/vendor/tracy"
        include "DEngine/vendor/enkiTS"
    group ""
    filter {}
end
