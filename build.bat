@echo off
setlocal

REM Adventure Designer Studio - Build Script for Windows
REM This script builds the project using vcpkg for dependency management

REM Configuration
set BUILD_TYPE=%1
if "%BUILD_TYPE%"=="" set BUILD_TYPE=Release

if not defined VCPKG_ROOT (
    set VCPKG_ROOT=C:\vcpkg
)

echo =========================================
echo Building Adventure Designer Studio
echo Build Type: %BUILD_TYPE%
echo vcpkg Root: %VCPKG_ROOT%
echo =========================================

REM Check if vcpkg exists
if not exist "%VCPKG_ROOT%\vcpkg.exe" (
    echo ERROR: vcpkg not found at %VCPKG_ROOT%
    echo Please install vcpkg first:
    echo   git clone https://github.com/microsoft/vcpkg.git C:\vcpkg
    echo   cd C:\vcpkg
    echo   .\bootstrap-vcpkg.bat
    echo.
    echo Or set VCPKG_ROOT environment variable to your vcpkg installation:
    echo   setx VCPKG_ROOT "C:\path\to\vcpkg"
    exit /b 1
)

REM Create build directory
if not exist build mkdir build
cd build

echo.
echo Configuring with CMake...
cmake .. ^
    -DCMAKE_BUILD_TYPE=%BUILD_TYPE% ^
    -DCMAKE_TOOLCHAIN_FILE="%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake" ^
    -DVCPKG_INSTALLED_DIR="%cd%\vcpkg_installed"

if errorlevel 1 (
    echo ERROR: CMake configuration failed
    exit /b 1
)

echo.
echo Building project...
cmake --build . --config %BUILD_TYPE% -j

if errorlevel 1 (
    echo ERROR: Build failed
    exit /b 1
)

echo.
echo =========================================
echo Build complete!
echo =========================================
echo Executable: build\%BUILD_TYPE%\Adventure_Designer_Studio.exe
echo.
echo To run the application:
echo   cd build\%BUILD_TYPE% ^&^& Adventure_Designer_Studio.exe
echo.
