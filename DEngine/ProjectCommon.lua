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

function IncludeEngineDependencies()
    group "Dependencies"
        include "DEngine/vendor/GLFW"
        include "DEngine/vendor/GLAD"
        include "DEngine/vendor/imgui"
        include "DEngine/vendor/yaml-cpp"
    group ""
    filter {}
end
