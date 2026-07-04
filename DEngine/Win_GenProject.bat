@echo off
setlocal enabledelayedexpansion

echo ========================================
echo   DEngine Project Generator
echo ========================================
echo.

:: ============================================
:: 1. ПРОВЕРКА НАЛИЧИЯ PREMAKE
:: ============================================
if not exist "vendor\bin\premake\premake5.exe" (
    echo [ERROR] Premake not found!
    echo.
    echo Expected path: vendor\bin\premake\premake5.exe
    echo.
    PAUSE
    exit /b 1
)

:: ============================================
:: 2. ОПРЕДЕЛЕНИЕ ВЕРСИИ VISUAL STUDIO
:: ============================================
set VS_VERSION=

:: Проверяем Visual Studio 2022
if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\devenv.exe" (
    set VS_VERSION=vs2022
    echo [INFO] Found Visual Studio 2022 (Community)
    goto :found_vs
)

if exist "C:\Program Files\Microsoft Visual Studio\2022\Professional\Common7\IDE\devenv.exe" (
    set VS_VERSION=vs2022
    echo [INFO] Found Visual Studio 2022 (Professional)
    goto :found_vs
)

if exist "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\Common7\IDE\devenv.exe" (
    set VS_VERSION=vs2022
    echo [INFO] Found Visual Studio 2022 (Enterprise)
    goto :found_vs
)

:: Проверяем Visual Studio 2019
if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\IDE\devenv.exe" (
    set VS_VERSION=vs2019
    echo [INFO] Found Visual Studio 2019 (Community)
    goto :found_vs
)

if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\Common7\IDE\devenv.exe" (
    set VS_VERSION=vs2019
    echo [INFO] Found Visual Studio 2019 (Professional)
    goto :found_vs
)

if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\Common7\IDE\devenv.exe" (
    set VS_VERSION=vs2019
    echo [INFO] Found Visual Studio 2019 (Enterprise)
    goto :found_vs
)

:: Проверяем Visual Studio 2017
if exist "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\Common7\IDE\devenv.exe" (
    set VS_VERSION=vs2017
    echo [INFO] Found Visual Studio 2017 (Community)
    goto :found_vs
)

if exist "C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\Common7\IDE\devenv.exe" (
    set VS_VERSION=vs2017
    echo [INFO] Found Visual Studio 2017 (Professional)
    goto :found_vs
)

if exist "C:\Program Files (x86)\Microsoft Visual Studio\2017\Enterprise\Common7\IDE\devenv.exe" (
    set VS_VERSION=vs2017
    echo [INFO] Found Visual Studio 2017 (Enterprise)
    goto :found_vs
)

:: Проверяем Visual Studio через vswhere (самый надежный способ)
where /q vswhere
if !ERRORLEVEL! EQU 0 (
    for /f "usebackq tokens=*" %%i in (`vswhere -latest -property installationPath`) do (
        set VS_PATH=%%i
    )
    
    if not "!VS_PATH!"=="" (
        echo [INFO] Found Visual Studio via vswhere: !VS_PATH!
        
        :: Определяем версию по пути
        echo !VS_PATH! | findstr /i "2022" >nul
        if !ERRORLEVEL! EQU 0 (
            set VS_VERSION=vs2022
            goto :found_vs
        )
        
        echo !VS_PATH! | findstr /i "2019" >nul
        if !ERRORLEVEL! EQU 0 (
            set VS_VERSION=vs2019
            goto :found_vs
        )
        
        echo !VS_PATH! | findstr /i "2017" >nul
        if !ERRORLEVEL! EQU 0 (
            set VS_VERSION=vs2017
            goto :found_vs
        )
    )
)

:: Если ничего не найдено
echo [ERROR] Visual Studio not found!
echo.
echo Please install one of the following:
echo   - Visual Studio 2022 (recommended)
echo   - Visual Studio 2019
echo   - Visual Studio 2017
echo.
echo Make sure to install the "Desktop development with C++" workload.
echo.
echo Download: https://visualstudio.microsoft.com/downloads/
echo.
PAUSE
exit /b 1

:found_vs
echo [INFO] Using generator: !VS_VERSION!
echo.

:: ============================================
:: 3. ЗАПУСК PREMAKE
:: ============================================
echo [INFO] Generating project files...
call vendor\bin\premake\premake5.exe !VS_VERSION!

:: ============================================
:: 4. ПРОВЕРКА РЕЗУЛЬТАТА
:: ============================================
IF !ERRORLEVEL! NEQ 0 (
    echo.
    echo [ERROR] Premake generation failed!
    echo.
    PAUSE
    exit /b !ERRORLEVEL!
)

echo.
echo ========================================
echo   Project generated successfully!
echo ========================================
echo.
echo Open the solution file:
echo   DEngine.sln
echo.
PAUSE