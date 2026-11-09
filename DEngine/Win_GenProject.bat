@echo off
setlocal enabledelayedexpansion
cd /d "%~dp0"

echo ========================================
echo   DEngine Project Generator
echo ========================================
echo.

set "PREMAKE=vendor\Premake\premake5.exe"
if not exist "%PREMAKE%" goto :no_premake

set "VSINSTALLER=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer"
if not exist "%VSINSTALLER%\vswhere.exe" goto :no_vswhere

set "VS_PATH="
set "VS_YEAR="
pushd "%VSINSTALLER%"
for /f "usebackq delims=" %%i in (`vswhere.exe -latest -prerelease -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VS_PATH=%%i"
for /f "usebackq delims=" %%i in (`vswhere.exe -latest -prerelease -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property catalog_productLineVersion`) do set "VS_YEAR=%%i"
if not defined VS_PATH (
    for /f "usebackq delims=" %%i in (`vswhere.exe -latest -prerelease -property installationPath`) do set "VS_PATH=%%i"
    for /f "usebackq delims=" %%i in (`vswhere.exe -latest -prerelease -property catalog_productLineVersion`) do set "VS_YEAR=%%i"
)
popd

if not defined VS_PATH goto :no_vs

set "VS_ACTION="
if "%VS_YEAR%"=="2026" set "VS_ACTION=vs2026"
if "%VS_YEAR%"=="2022" set "VS_ACTION=vs2022"
if "%VS_YEAR%"=="2019" set "VS_ACTION=vs2019"
if "%VS_YEAR%"=="2017" set "VS_ACTION=vs2017"
if not defined VS_ACTION goto :no_action

echo [INFO] Latest Visual Studio: %VS_YEAR%
echo [INFO] Path: %VS_PATH%
echo [INFO] Premake action: %VS_ACTION%
echo.
echo [INFO] Generating DEngine.sln...
"%PREMAKE%" --file=premake5.lua %VS_ACTION%
if errorlevel 1 goto :premake_failed

set "MSBUILD=%VS_PATH%\MSBuild\Current\Bin\MSBuild.exe"
if not exist "%MSBUILD%" goto :no_msbuild

echo.
echo [INFO] Building DEngine.sln Debug x64 with Visual Studio %VS_YEAR%...
"%MSBUILD%" DEngine.sln /t:Build /p:Configuration=Debug /p:Platform=x64 /m /v:minimal
if errorlevel 1 goto :build_failed

echo.
echo ========================================
echo   DEngine.sln generated and built
echo ========================================
echo.
pause
exit /b 0

:no_premake
echo [ERROR] Premake not found: %PREMAKE%
pause
exit /b 1

:no_vswhere
echo [ERROR] vswhere not found in:
echo   %VSINSTALLER%
pause
exit /b 1

:no_vs
echo [ERROR] Visual Studio was not found.
echo Install Visual Studio with the "Desktop development with C++" workload.
pause
exit /b 1

:no_action
echo [ERROR] No Premake generator for Visual Studio %VS_YEAR%.
echo Install is: %VS_PATH%
pause
exit /b 1

:premake_failed
echo.
echo [ERROR] Premake generation failed.
pause
exit /b 1

:no_msbuild
echo [ERROR] MSBuild not found: %MSBUILD%
pause
exit /b 1

:build_failed
echo.
echo [ERROR] Build failed.
pause
exit /b 1
