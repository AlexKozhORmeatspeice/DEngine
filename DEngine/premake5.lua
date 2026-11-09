-- Generates DEngine.sln: engine, editor, and Sandbox player.
workspace "DEngine"
    architecture "x64"
    startproject "DEditor"
    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

include "ProjectCommon.lua"
IncludeEngineDependencies()
include "ProjectDEngine.lua"
include "ProjectDEditor.lua"
include "ProjectSandbox.lua"
