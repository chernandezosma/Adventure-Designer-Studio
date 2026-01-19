cmake_minimum_required(VERSION 3.14...3.31)

set(ADSProject Adventure_Designer_Studio)
set(HEADER_GUARD_PREFIX "ADS" CACHE STRING "Prefix for header guards")

project(${ADSProject} CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

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

add_compile_definitions(
        PROJECT_ROOT="${CMAKE_SOURCE_DIR}"
        PROJECT_BUILD_ROOT="${CMAKE_BINARY_DIR}"
)


# ----------------------------------------------------------
# --- Compilation flags configuration
# ----------------------------------------------------------
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
    set(CMAKE_CXX_FLAGS_DEBUG "-g3 -O0 -Wall -Wextra -DDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
    set(CMAKE_CXX_FLAGS_TEST "-g3 -O0 -Wall -Wextra -DTEST_MODE")   ### NUEVO ###
elseif (MSVC)
    set(CMAKE_CXX_FLAGS_DEBUG "/Zi /Od /DEBUG:FULL /DDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "/O2 /DNDEBUG")
    set(CMAKE_CXX_FLAGS_TEST "/Zi /Od /DEBUG:FULL /DTEST_MODE")     ### NUEVO ###
    # Prevent Windows min/max macros from conflicting with std::min/std::max and boost
    add_compile_definitions(NOMINMAX WIN32_LEAN_AND_MEAN)
endif ()

# ----------------------------------------------------------
# --- Copy public resources
# ----------------------------------------------------------
message(STATUS "Copying the content of public folder: ${CMAKE_BUILD_TYPE}")
file(GLOB_RECURSE PUBLIC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/public/*")
add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/public/.stamp
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/public
        ${CMAKE_CURRENT_BINARY_DIR}/public
        COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_CURRENT_BINARY_DIR}/public/.stamp
        DEPENDS ${PUBLIC_FILES}
        COMMENT "Copying public folder to build directory"
)

#add_custom_target(copy_public_folder ALL
#        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/public/.stamp
#)
add_custom_target(copy_public_folder ALL
        COMMAND ${CMAKE_COMMAND} -E echo "Cleaning and copying public folder..."
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/public
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/public
        ${CMAKE_CURRENT_BINARY_DIR}/public
        COMMENT "Copying public folder to build directory"
)

# ----------------------------------------------------------
# --- External dependencies (managed by vcpkg)
# ----------------------------------------------------------
find_package(OpenGL REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS uuid headers)

# ----------------------------------------------------------
# --- Project directories
# ----------------------------------------------------------
include_directories(
        src
        src/classes
        src/exceptions
        src/constants
        src/utils
        src/include
        lib/IconFontCppHeaders
)

# ----------------------------------------------------------
# --- Header and source files
# ----------------------------------------------------------
set(Headers
        src/exceptions/filesystem/path_not_exist_exception.h
        src/exceptions/filesystem/file_not_found_exception.h
        src/exceptions/filesystem/file_not_open_exception.h
        src/exceptions/json/json_parse_exception.h
        src/constants/languages.h
        src/constants/System.h
        src/classes/i18n/i18n.h
        src/classes/env/env.h
        src/include/adsString.h
        src/include/i18nUtils.h
)

set(Sources
        main.cpp
        src/app.cpp
        src/app.h
        src/classes/i18n/i18n.cpp
        src/classes/env/env.cpp
        src/classes/env/env.h
        src/include/adsString.h
        src/include/adsString.cpp
        src/include/i18nUtils.h
        src/include/i18nUtils.cpp
        src/include/im_gui_tools.cpp
        src/include/im_gui_tools.h
        src/classes/Logger/logger.cpp
        src/classes/Logger/logger.h
        src/classes/UI/UI.cpp
        src/classes/UI/UI.h
        src/classes/UI/Window.cpp
        src/classes/UI/Window.h
        src/classes/UI/fonts.cpp
        src/classes/UI/fonts.h
        src/classes/IDE/IDERenderer.cpp
        src/classes/IDE/IDERenderer.h
        src/classes/IDE/LayoutManager.cpp
        src/classes/IDE/LayoutManager.h
        src/classes/IDE/navigation/MenuBarRenderer.cpp
        src/classes/IDE/navigation/MenuBarRenderer.h
        src/classes/IDE/navigation/ToolBarRenderer.cpp
        src/classes/IDE/navigation/ToolBarRenderer.h
        src/classes/IDE/panels/BasePanel.cpp
        src/classes/IDE/panels/BasePanel.h
        src/classes/IDE/panels/StatusBarPanel.cpp
        src/classes/IDE/panels/StatusBarPanel.h
        src/classes/IDE/panels/EntitiesPanel.cpp
        src/classes/IDE/panels/EntitiesPanel.h
        src/classes/IDE/panels/PropertiesPanel.cpp
        src/classes/IDE/panels/PropertiesPanel.h
        src/classes/IDE/panels/InspectorPanel.cpp
        src/classes/IDE/panels/InspectorPanel.h
        src/classes/IDE/panels/WorkingAreaPanel.cpp
        src/classes/IDE/panels/WorkingAreaPanel.h
        src/classes/IDE/IDEBase.cpp
        src/classes/IDE/IDEBase.h
        src/classes/IDE/themes/Theme.cpp
        src/classes/IDE/themes/Theme.h
        src/classes/IDE/themes/DarkTheme.cpp
        src/classes/IDE/themes/DarkTheme.h
        src/classes/IDE/themes/LightTheme.cpp
        src/classes/IDE/themes/LightTheme.h
        src/classes/IDE/navigation/NavigationService.cpp
        src/classes/IDE/navigation/NavigationService.h
        src/classes/IDE/navigation/NavigationConstants.h
)

# ----------------------------------------------------------
# --- Main executable
# ----------------------------------------------------------
add_executable(${ADSProject} ${Sources} ${Headers})
add_dependencies(${ADSProject} copy_public_folder)  # Add this line

target_link_libraries(${ADSProject} PRIVATE
        nlohmann_json::nlohmann_json
        imgui::imgui
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
        $<IF:$<TARGET_EXISTS:SDL2::SDL2main>,SDL2::SDL2main,SDL2::SDL2main-static>
        spdlog::spdlog
        fmt::fmt
        Boost::uuid
        Boost::headers
)

# ----------------------------------------------------------
# --- Test config (test mode)
# ----------------------------------------------------------
include(CTest)

if (CMAKE_BUILD_TYPE STREQUAL "Test")    ### NUEVO ###
    message(STATUS "Modo TEST activado — incluyendo GoogleTest")
    find_package(GTest CONFIG REQUIRED)
    add_subdirectory(tests)
    enable_testing()
endif ()

# ----------------------------------------------------------
# --- Useful information
# ----------------------------------------------------------
message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
