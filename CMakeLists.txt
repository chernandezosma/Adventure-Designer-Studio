cmake_minimum_required(VERSION 3.14...3.31)

set(ADSProject Adventure_Designer_Studio)

project(${ADSProject} CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(IMGUI_DIR "lib/imgui")

# ----------------------------------------------------------
# --- Config build environments
# ----------------------------------------------------------
# By default CMake use only one, but we can force to use three ones
set(AVAILABLE_BUILD_TYPES Debug Release Test)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the build type." FORCE)
endif ()

set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${AVAILABLE_BUILD_TYPES})

message(STATUS "Selected build type: ${CMAKE_BUILD_TYPE}")

# ----------------------------------------------------------
# --- Compilation flags configuration
# ----------------------------------------------------------
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS_DEBUG "-g3 -O0 -Wall -Wextra -DDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
    set(CMAKE_CXX_FLAGS_TEST "-g3 -O0 -Wall -Wextra -DTEST_MODE")
elseif (MSVC)
    set(CMAKE_CXX_FLAGS_DEBUG "/Zi /Od /DDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "/O2 /DNDEBUG")
    set(CMAKE_CXX_FLAGS_TEST "/Zi /Od /DTEST_MODE")
    # Suppress MSVC warnings about unsafe functions
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif ()

# ----------------------------------------------------------
# --- Copy resources (translations)
# ----------------------------------------------------------
if (NOT EXISTS "${CMAKE_BINARY_DIR}/translations")
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/translations/core")
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/translations/game")
endif ()
file(COPY "public/translations/" DESTINATION "${CMAKE_BINARY_DIR}/translations")

if (NOT EXISTS "${CMAKE_BINARY_DIR}/public")
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/public/assets")
endif ()
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/public/" DESTINATION "${CMAKE_BINARY_DIR}/public")

# ----------------------------------------------------------
# --- External dependencies
# ----------------------------------------------------------

# OpenGL
find_package(OpenGL REQUIRED)

# SDL2 - Robust cross-platform finding
find_package(SDL2 CONFIG QUIET)

if(SDL2_FOUND)
    message(STATUS "Found SDL2 via CONFIG mode")
else()
    message(STATUS "SDL2 CONFIG not found, trying manual search...")
    
    # For Windows: Set hint to vcpkg if available
    if(WIN32)
        if(DEFINED ENV{VCPKG_ROOT})
            set(SDL2_DIR "$ENV{VCPKG_ROOT}/installed/x64-windows")
        endif()
    endif()
    
    # Manual search for SDL2
    find_path(SDL2_INCLUDE_DIR SDL.h
        HINTS ${SDL2_DIR}
        PATH_SUFFIXES include/SDL2 include SDL2
    )
    
    find_library(SDL2_LIBRARY
        NAMES SDL2
        HINTS ${SDL2_DIR}
        PATH_SUFFIXES lib lib/x64
    )
    
    if(SDL2_INCLUDE_DIR AND SDL2_LIBRARY)
        set(SDL2_FOUND TRUE)
        message(STATUS "Found SDL2: ${SDL2_LIBRARY}")
        
        # Create SDL2::SDL2 target
        if(NOT TARGET SDL2::SDL2)
            add_library(SDL2::SDL2 UNKNOWN IMPORTED)
            set_target_properties(SDL2::SDL2 PROPERTIES
                IMPORTED_LOCATION "${SDL2_LIBRARY}"
                INTERFACE_INCLUDE_DIRECTORIES "${SDL2_INCLUDE_DIR}"
            )
            
            # Windows needs SDL2main
            if(WIN32)
                find_library(SDL2_MAIN_LIBRARY
                    NAMES SDL2main
                    HINTS ${SDL2_DIR}
                    PATH_SUFFIXES lib lib/x64
                )
                if(SDL2_MAIN_LIBRARY)
                    set_target_properties(SDL2::SDL2 PROPERTIES
                        INTERFACE_LINK_LIBRARIES "${SDL2_MAIN_LIBRARY}"
                    )
                endif()
            endif()
        endif()
    else()
        message(FATAL_ERROR 
            "SDL2 not found!\n"
            "Please install SDL2:\n"
            "  Windows (vcpkg): vcpkg install sdl2:x64-windows\n"
            "  Linux:           sudo apt-get install libsdl2-dev\n"
            "  macOS:           brew install sdl2\n"
            "\n"
            "If using vcpkg, make sure to:\n"
            "  1. Run: vcpkg integrate install\n"
            "  2. Or use: -DCMAKE_TOOLCHAIN_FILE=[vcpkg-root]/scripts/buildsystems/vcpkg.cmake"
        )
    endif()
endif()

# nlohmann_json
add_subdirectory(lib/nlohmann)

# ImGui library
add_library(imgui
        lib/imgui/imgui.cpp
        lib/imgui/imgui_draw.cpp
        lib/imgui/imgui_tables.cpp
        lib/imgui/imgui_widgets.cpp
        lib/imgui/backends/imgui_impl_sdl2.cpp
        lib/imgui/backends/imgui_impl_sdlrenderer2.cpp
)

target_include_directories(imgui PUBLIC
        ${IMGUI_DIR}
        ${IMGUI_DIR}/backends
)

target_link_libraries(imgui PUBLIC SDL2::SDL2)

# ----------------------------------------------------------
# --- Project directories
# ----------------------------------------------------------
include_directories(
        src/classes
        src/exceptions
        src/constants
        src/utils
        src/include
        ${IMGUI_DIR}
        ${IMGUI_DIR}/backends
)

# ----------------------------------------------------------
# --- Header and source files
# ----------------------------------------------------------
set(Headers
        src/exceptions/path_not_exist_exception.h
        src/exceptions/file_not_found_exception.h
        src/exceptions/file_not_open_exception.h
        src/constants/languages.h
        src/constants/SystemConst.h
        src/classes/i18n/i18n.h
        src/classes/env/env.h
        src/include/adsString.h
        src/include/i18nUtils.h
)

set(Sources
        main.cpp
        src/classes/i18n/i18n.cpp
        src/classes/env/env.cpp
        src/classes/env/env.h
        src/include/adsString.h
        src/include/adsString.cpp
        src/include/i18nUtils.h
        src/include/i18nUtils.cpp
)

# ----------------------------------------------------------
# --- Main executable
# ----------------------------------------------------------
add_executable(${ADSProject} ${Sources} ${Headers})

# Link libraries using modern CMake targets
target_link_libraries(${ADSProject} PRIVATE
        nlohmann_json::nlohmann_json
        imgui
        SDL2::SDL2
        OpenGL::GL
)

# ----------------------------------------------------------
# --- Platform-specific configurations
# ----------------------------------------------------------

# Windows: Copy SDL2.dll to output directory
if (WIN32)
    # Get SDL2 DLL location
    if (TARGET SDL2::SDL2)
        get_target_property(SDL2_DLL_PATH SDL2::SDL2 IMPORTED_LOCATION)
        if (NOT SDL2_DLL_PATH)
            # Try different configuration
            get_target_property(SDL2_DLL_PATH SDL2::SDL2 IMPORTED_LOCATION_RELEASE)
        endif()
        if (NOT SDL2_DLL_PATH)
            get_target_property(SDL2_DLL_PATH SDL2::SDL2 IMPORTED_LOCATION_DEBUG)
        endif()
        
        if (SDL2_DLL_PATH)
            add_custom_command(TARGET ${ADSProject} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${SDL2_DLL_PATH}"
                $<TARGET_FILE_DIR:${ADSProject}>
                COMMENT "Copying SDL2.dll to output directory"
            )
        endif()
    endif()
    
    # Set Windows subsystem to CONSOLE (or WINDOWS for GUI-only app)
    set_target_properties(${ADSProject} PROPERTIES
        WIN32_EXECUTABLE FALSE  # Set to TRUE if you want a GUI-only app without console
    )
endif()

# macOS: Set bundle properties if needed
if (APPLE)
    set_target_properties(${ADSProject} PROPERTIES
        MACOSX_BUNDLE FALSE  # Set to TRUE if you want a .app bundle
    )
endif()

# Linux: Link additional libraries if needed
if (UNIX AND NOT APPLE)
    target_link_libraries(${ADSProject} PRIVATE
        ${CMAKE_DL_LIBS}  # Dynamic loading library
        pthread           # Threading library
    )
endif()

# ----------------------------------------------------------
# --- Test config (test mode)
# ----------------------------------------------------------
include(CTest)

if (CMAKE_BUILD_TYPE STREQUAL "Test")
    message(STATUS "TEST mode enabled – including GoogleTest")
    add_subdirectory(lib/googletest)
    add_subdirectory(tests)
    enable_testing()
endif ()

# ----------------------------------------------------------
# --- Useful information
# ----------------------------------------------------------
message(STATUS "==========================================")
message(STATUS "Platform: ${CMAKE_SYSTEM_NAME}")
message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "SDL2 Found: ${SDL2_FOUND}")
message(STATUS "OpenGL Found: ${OPENGL_FOUND}")
message(STATUS "==========================================")
