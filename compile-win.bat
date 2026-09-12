@echo off
setlocal EnableDelayedExpansion
rem ===========================================================================
rem  Copyright (c) 2026 Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
rem
rem  compile-win.bat - one-shot build & delivery script for Adventure Designer
rem                    Studio on Windows 10/11 (x64).
rem
rem  It takes a near-clean machine to a ready-to-run artifact:
rem    1. detects CPU count and the Visual Studio C++ toolset (via vswhere)
rem    2. checks / installs CMake, Git and Ninja through winget
rem    3. finds an existing vcpkg or clones + bootstraps one into %USERPROFILE%\vcpkg
rem    4. initialises the IconFontCppHeaders submodule
rem    5. configures and builds through the vcpkg toolchain (Ninja + MSVC env)
rem    6. stages the .exe + its DLLs + public\ + .env + a launcher into .\dist
rem
rem  Portability note: CMakeLists.txt does NOT force -march=native for MSVC, so
rem  the Windows binary is a normal x64 build. It still needs the Microsoft
rem  Visual C++ Redistributable on the target machine (or a static-CRT triplet).
rem
rem  Usage:
rem    compile-win.bat [BUILD_TYPE] [--no-install] [--clean] [--no-package]
rem
rem      BUILD_TYPE     Release (default) | Debug | Test
rem      --no-install   only check prerequisites; never call winget. Abort on gap.
rem      --clean        remove the build directory before configuring.
rem      --no-package   configure + build only; skip the dist\ staging step.
rem      -h, --help     show this help and exit.
rem ===========================================================================

set "REPO_ROOT=%~dp0"
if "%REPO_ROOT:~-1%"=="\" set "REPO_ROOT=%REPO_ROOT:~0,-1%"
set "BUILD_DIR=%REPO_ROOT%\build"
set "DIST_DIR=%REPO_ROOT%\dist"
set "BIN_NAME=Adventure_Designer_Studio"
set "VCPKG_REPO=https://github.com/microsoft/vcpkg.git"

rem --- defaults ---
set "BUILD_TYPE=Release"
set "DO_INSTALL=1"
set "DO_CLEAN=0"
set "DO_PACKAGE=1"

rem --- parse args ---
:parse
if "%~1"=="" goto parsed
if /I "%~1"=="Release"      set "BUILD_TYPE=Release"    & shift & goto parse
if /I "%~1"=="Debug"        set "BUILD_TYPE=Debug"      & shift & goto parse
if /I "%~1"=="Test"         set "BUILD_TYPE=Test"       & shift & goto parse
if /I "%~1"=="--no-install" set "DO_INSTALL=0"          & shift & goto parse
if /I "%~1"=="--clean"      set "DO_CLEAN=1"            & shift & goto parse
if /I "%~1"=="--no-package" set "DO_PACKAGE=0"          & shift & goto parse
if /I "%~1"=="-h"           goto help
if /I "%~1"=="--help"       goto help
echo ERROR: Unknown argument: %~1  (try --help)
exit /b 2
:parsed

set "START_TS=%TIME%"

echo.
echo ==^> Detecting environment
echo     Host       : %COMPUTERNAME%  (%PROCESSOR_ARCHITECTURE%)
echo     CPUs       : %NUMBER_OF_PROCESSORS%
echo     Build type : %BUILD_TYPE%

rem =========================================================================
rem  winget helper
rem =========================================================================
set "HAVE_WINGET=0"
where winget >nul 2>&1 && set "HAVE_WINGET=1"

rem =========================================================================
rem  Visual Studio C++ toolset (vswhere)
rem =========================================================================
echo.
echo ==^> Checking Visual Studio C++ toolset
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
set "VS_PATH="
if exist "%VSWHERE%" (
    for /f "usebackq delims=" %%i in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath 2^>nul`) do set "VS_PATH=%%i"
)
if not defined VS_PATH (
    echo     ^^! No Visual Studio C++ toolset ^(MSVC^) was found.
    if "%DO_INSTALL%"=="1" if "%HAVE_WINGET%"=="1" (
        echo     ^^! Installing Visual Studio 2022 Build Tools with the C++ workload.
        echo     ^^! This is a large download and may take a while.
        choice /C YN /M "    Proceed with 'winget install Microsoft.VisualStudio.2022.BuildTools'"
        if errorlevel 2 goto need_vs
        winget install --id Microsoft.VisualStudio.2022.BuildTools -e --accept-source-agreements --accept-package-agreements --override "--quiet --wait --add Microsoft.VisualStudio.Workload.NativeDesktop --includeRecommended"
        for /f "usebackq delims=" %%i in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath 2^>nul`) do set "VS_PATH=%%i"
    )
)
if not defined VS_PATH goto need_vs
echo     * MSVC toolset: !VS_PATH!

set "VCVARS=!VS_PATH!\VC\Auxiliary\Build\vcvars64.bat"
if not exist "!VCVARS!" (
    echo ERROR: vcvars64.bat not found under !VS_PATH!.
    exit /b 1
)

rem =========================================================================
rem  CMake / Git / Ninja
rem =========================================================================
echo.
echo ==^> Checking build toolchain

call :need_tool cmake "Kitware.CMake" CMake || exit /b 1
call :need_tool git   "Git.Git"       Git   || exit /b 1

set "USE_NINJA=1"
where ninja >nul 2>&1
if errorlevel 1 (
    if "%DO_INSTALL%"=="1" if "%HAVE_WINGET%"=="1" (
        echo     ^^! Installing Ninja ^(optional^) via winget
        winget install --id Ninja-build.Ninja -e --accept-source-agreements --accept-package-agreements >nul 2>&1
    )
    where ninja >nul 2>&1 || set "USE_NINJA=0"
)
if "%USE_NINJA%"=="1" (
    for /f "delims=" %%v in ('ninja --version 2^>nul') do echo     * Ninja %%v
) else (
    echo     ^^! Ninja not available - falling back to the Visual Studio generator.
)

rem winget may have installed to a spot not yet on this shell's PATH.
where cmake >nul 2>&1 || set "PATH=%ProgramFiles%\CMake\bin;%PATH%"
where cmake >nul 2>&1 || (echo ERROR: cmake still not on PATH - open a new terminal and re-run. & exit /b 1)

rem =========================================================================
rem  vcpkg
rem =========================================================================
echo.
echo ==^> Locating vcpkg
set "VCPKG_DIR="
if defined VCPKG_ROOT if exist "%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake" set "VCPKG_DIR=%VCPKG_ROOT%"

if not defined VCPKG_DIR (
    for %%D in ("%USERPROFILE%\vcpkg" "C:\vcpkg" "C:\src\vcpkg" "%USERPROFILE%\source\repos\vcpkg") do (
        if not defined VCPKG_DIR if exist "%%~D\scripts\buildsystems\vcpkg.cmake" set "VCPKG_DIR=%%~D"
    )
)

if not defined VCPKG_DIR (
    set "VCPKG_DIR=%USERPROFILE%\vcpkg"
    echo.
    echo ==^> Cloning vcpkg into !VCPKG_DIR!
    if "%DO_INSTALL%"=="0" (
        echo ERROR: vcpkg not found and --no-install was given.
        echo        git clone %VCPKG_REPO% "!VCPKG_DIR!" ^&^& "!VCPKG_DIR!\bootstrap-vcpkg.bat"
        exit /b 1
    )
    if exist "!VCPKG_DIR!\.git" (
        git -C "!VCPKG_DIR!" pull --ff-only
    ) else (
        git clone %VCPKG_REPO% "!VCPKG_DIR!" || (echo ERROR: git clone of vcpkg failed. & exit /b 1)
    )
    call "!VCPKG_DIR!\bootstrap-vcpkg.bat" || (echo ERROR: vcpkg bootstrap failed. & exit /b 1)
)
if not exist "!VCPKG_DIR!\scripts\buildsystems\vcpkg.cmake" (
    echo ERROR: vcpkg toolchain file missing under !VCPKG_DIR!.
    exit /b 1
)
set "VCPKG_ROOT=!VCPKG_DIR!"
echo     * vcpkg: !VCPKG_DIR!
echo       ^(persist it with:  setx VCPKG_ROOT "!VCPKG_DIR!"  - new terminals only^)

rem =========================================================================
rem  Submodules
rem =========================================================================
echo.
echo ==^> Initialising submodules
if exist "%REPO_ROOT%\.git" (
    git -C "%REPO_ROOT%" submodule update --init --recursive
) else (
    if exist "%REPO_ROOT%\lib\IconFontCppHeaders\IconsFontAwesome6.h" (
        echo     ^^! Not a git checkout; assuming vendored lib\IconFontCppHeaders is present.
    ) else (
        echo ERROR: Not a git checkout and lib\IconFontCppHeaders is missing.
        echo        Clone the repo with git instead of downloading a zip.
        exit /b 1
    )
)

rem =========================================================================
rem  Enter the MSVC environment (needed for the Ninja generator)
rem =========================================================================
echo.
echo ==^> Loading MSVC environment
call "!VCVARS!" >nul || (echo ERROR: vcvars64.bat failed. & exit /b 1)

rem =========================================================================
rem  Configure
rem =========================================================================
echo.
echo ==^> Configuring (%BUILD_TYPE%)
if "%DO_CLEAN%"=="1" if exist "%BUILD_DIR%" (
    echo     Removing %BUILD_DIR%
    rmdir /s /q "%BUILD_DIR%"
)
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"

set "GEN_ARGS="
if "%USE_NINJA%"=="1" set "GEN_ARGS=-G Ninja"

cmake -S "%REPO_ROOT%" -B "%BUILD_DIR%" %GEN_ARGS% ^
    -DCMAKE_BUILD_TYPE=%BUILD_TYPE% ^
    -DCMAKE_TOOLCHAIN_FILE="!VCPKG_DIR!\scripts\buildsystems\vcpkg.cmake" ^
    -DVCPKG_INSTALLED_DIR="%BUILD_DIR%\vcpkg_installed" ^
    -DVCPKG_OVERLAY_PORTS="%REPO_ROOT%\vcpkg-overlays" ^
    -DVCPKG_APPLOCAL_DEPS=ON
if errorlevel 1 (echo ERROR: CMake configuration failed. & exit /b 1)

rem =========================================================================
rem  Build
rem =========================================================================
echo.
echo ==^> Building (first run downloads + compiles all vcpkg deps - 10-20 min)
cmake --build "%BUILD_DIR%" --config %BUILD_TYPE% -j %NUMBER_OF_PROCESSORS%
if errorlevel 1 (echo ERROR: Build failed. & exit /b 1)

rem =========================================================================
rem  Locate the produced binary
rem =========================================================================
set "BIN_PATH="
for %%P in (
    "%BUILD_DIR%\%BIN_NAME%.exe"
    "%BUILD_DIR%\%BUILD_TYPE%\%BIN_NAME%.exe"
) do if not defined BIN_PATH if exist "%%~P" set "BIN_PATH=%%~P"
if not defined BIN_PATH (
    for /f "delims=" %%F in ('dir /s /b "%BUILD_DIR%\%BIN_NAME%.exe" 2^>nul') do if not defined BIN_PATH set "BIN_PATH=%%F"
)
if not defined BIN_PATH (
    echo ERROR: Build finished but %BIN_NAME%.exe was not found under %BUILD_DIR%.
    exit /b 1
)
echo     * Binary: !BIN_PATH!
for %%D in ("!BIN_PATH!") do set "BIN_DIR=%%~dpD"

if "%DO_PACKAGE%"=="0" (
    echo.
    echo ==^> Done ^(--no-package: skipped staging^)
    echo     Run it with:  cd /d "!BIN_DIR!" ^&^& %BIN_NAME%.exe
    goto done
)

rem =========================================================================
rem  Stage into dist\
rem =========================================================================
echo.
echo ==^> Staging runnable artifact into %DIST_DIR%
if exist "%DIST_DIR%" rmdir /s /q "%DIST_DIR%"
mkdir "%DIST_DIR%"

copy /y "!BIN_PATH!" "%DIST_DIR%\%BIN_NAME%.exe" >nul

rem DLLs that vcpkg's applocal-deps step dropped next to the exe (SDL3.dll, ...)
copy /y "!BIN_DIR!*.dll" "%DIST_DIR%\" >nul 2>&1

rem assets + config
xcopy /e /i /q /y "%REPO_ROOT%\public" "%DIST_DIR%\public" >nul
if exist "%REPO_ROOT%\.env" (
    copy /y "%REPO_ROOT%\.env" "%DIST_DIR%\.env" >nul
) else if exist "%REPO_ROOT%\.env.example" (
    copy /y "%REPO_ROOT%\.env.example" "%DIST_DIR%\.env" >nul
    echo     ^^! .env not found - copied .env.example as .env ^(review before shipping^)
) else (
    echo     ^^! no .env or .env.example found; the app may complain at startup
)

rem launcher - keeps public\ and .env on the working directory
> "%DIST_DIR%\run.bat" echo @echo off
>> "%DIST_DIR%\run.bat" echo cd /d "%%~dp0"
>> "%DIST_DIR%\run.bat" echo start "" "%%~dp0%BIN_NAME%.exe" %%*

rem =========================================================================
rem  Verify
rem =========================================================================
echo.
echo ==^> Verifying artifact
if exist "%DIST_DIR%\%BIN_NAME%.exe" (echo     * %BIN_NAME%.exe staged) else (echo     ^^! exe missing in dist)
if exist "%DIST_DIR%\SDL3.dll" (echo     * SDL3.dll staged) else (echo     ^^! SDL3.dll not next to the exe - the app will not start. Check VCPKG_APPLOCAL_DEPS.)
if exist "%DIST_DIR%\public\translations\core\en_US.json" (echo     * assets staged) else (echo     ^^! public\translations missing - check public\ contents)
dir /b "%DIST_DIR%\*.dll" 2>nul | find /c /v "" >"%TEMP%\_ads_dllcount.txt"
set /p DLLCOUNT=<"%TEMP%\_ads_dllcount.txt"
del "%TEMP%\_ads_dllcount.txt" >nul 2>&1
echo     * %DLLCOUNT% DLL^(s^) bundled

:done
echo.
echo ==^> Build complete
echo     Build type : %BUILD_TYPE%
echo     vcpkg root : !VCPKG_DIR!
echo     Artifact   : %DIST_DIR%
echo.
echo     Run it:
echo       "%DIST_DIR%\run.bat"
echo.
echo     Ship it (PowerShell):
echo       Compress-Archive -Path "%DIST_DIR%\*" -DestinationPath ADS-windows-x64.zip
echo.
echo     Reminder: the target machine needs the Microsoft Visual C++ Redistributable
echo     (x64) installed, unless you rebuild with a static-CRT vcpkg triplet.
endlocal
exit /b 0

rem =========================================================================
rem  Subroutines
rem =========================================================================
:need_tool
rem %1 = exe name, %2 = winget id, %3 = human label
where %~1 >nul 2>&1 && (echo     * %~3 present & exit /b 0)
if "%DO_INSTALL%"=="0" (
    echo ERROR: %~3 not found and --no-install was given.  winget install --id %~2 -e
    exit /b 1
)
if "%HAVE_WINGET%"=="0" (
    echo ERROR: %~3 not found and winget is unavailable.
    echo        Install %~3 manually ^(https://cmake.org/download/ , https://git-scm.com/download/win^) and re-run.
    exit /b 1
)
echo     ^^! Installing %~3 via winget (winget install --id %~2 -e)
winget install --id %~2 -e --accept-source-agreements --accept-package-agreements
where %~1 >nul 2>&1 && exit /b 0
rem refresh PATH from the registry for this session
for /f "usebackq tokens=2,*" %%A in (`reg query "HKLM\SYSTEM\CurrentControlSet\Control\Session Manager\Environment" /v Path 2^>nul`) do set "PATH=%%B;%PATH%"
where %~1 >nul 2>&1 && exit /b 0
echo ERROR: %~3 installed but not visible on PATH. Open a new terminal and re-run.
exit /b 1

:need_vs
echo.
echo ERROR: The Visual Studio C++ toolset is required.
echo   Install "Visual Studio 2022 Build Tools" and select the
echo   "Desktop development with C++" workload:
echo     https://visualstudio.microsoft.com/visual-cpp-build-tools/
echo   or, with winget:
echo     winget install --id Microsoft.VisualStudio.2022.BuildTools -e --override ^
echo       "--add Microsoft.VisualStudio.Workload.NativeDesktop --includeRecommended"
echo   Then re-run this script.
exit /b 1

:help
rem print the header comment block
for /f "tokens=1,* delims=]" %%a in ('type "%~f0" ^| findstr /n "^rem"') do (
    set "line=%%b"
    setlocal EnableDelayedExpansion
    if "!line:~0,4!"=="rem " (echo !line:~4!) else (echo.)
    endlocal
)
exit /b 0
