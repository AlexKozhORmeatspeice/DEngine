cmake_minimum_required(VERSION 3.1)

project(ImGui)


# On Unix-like systems, shared libraries can use the soname system.
set(GLFW_LIB_NAME imgui)

include_directories(${PROJECT_SOURCE_DIR}/)

add_library(imgui STATIC ${PROJECT_SOURCE_DIR}/imgui.cpp
                  ${PROJECT_SOURCE_DIR}/imgui_draw.cpp
                  ${PROJECT_SOURCE_DIR}/imgui_widgets.cpp
                  ${PROJECT_SOURCE_DIR}/imgui_demo.cpp)

set_target_properties(imgui PROPERTIES
                      OUTPUT_NAME imgui
                      POSITION_INDEPENDENT_CODE ON
                      FOLDER "imgui")