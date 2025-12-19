cmake_minimum_required(VERSION 3.14...3.31)

set(ADSProject Adventure_Designer_Studio)
set(HEADER_GUARD_PREFIX "ADS" CACHE STRING "Prefix for header guards")

project(${ADSProject} CXX)

set(CMAKE_CXX_STANDARD 23)
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

add_custom_target(copy_public_folder ALL
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/public/.stamp
)
#
#if (NOT EXISTS "${CMAKE_BINARY_DIR}/public/translations")
#    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/public/translations/core")
#    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/public/translations/game")
#endif ()
#
#
#if (NOT EXISTS "${CMAKE_BINARY_DIR}/public")
#    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/public")
#endif ()
#
#if (NOT EXISTS "${CMAKE_BINARY_DIR}/public/assets")
#    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/public/assets")
#endif ()
#
#if (NOT EXISTS "${CMAKE_BINARY_DIR}/public/fonts")
#    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/public/fonts")
#endif ()
#
#file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/public/assets" DESTINATION "${CMAKE_BINARY_DIR}/public/assets")
#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/public/translations/" DESTINATION "${CMAKE_BINARY_DIR}/public/translations")
#file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/public/fonts" DESTINATION "${CMAKE_BINARY_DIR}/public/fonts")

# ----------------------------------------------------------
# --- External dependencies
# ----------------------------------------------------------
find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)
add_subdirectory(lib/nlohmann_json)
add_subdirectory(lib/spdlog)
add_subdirectory(lib/uuid_v4)

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
        ${SDL2_INCLUDE_DIRS}
)

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
        ${SDL2_INCLUDE_DIRS}
        lib/IconFontCppHeaders
)

# ----------------------------------------------------------
# --- Header and source files
# ----------------------------------------------------------
set(Headers
        src/exceptions/filesystem/path_not_exist_exception.h
        src/exceptions/filesystem/file_not_found_exception.h
        src/exceptions/filesystem/file_not_open_exception.h
        src/constants/languages.h
        src/constants/System.h
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
        src/include/im_gui_tools.cpp
        src/include/im_gui_tools.h
        src/include/app.cpp
        src/include/app.h
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
        src/classes/IDE/MenuBarRenderer.cpp
        src/classes/IDE/MenuBarRenderer.h
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
)

# ----------------------------------------------------------
# --- Main executable
# ----------------------------------------------------------
add_executable(${ADSProject} ${Sources} ${Headers})
add_dependencies(${ADSProject} copy_public_folder)  # Add this line

target_link_libraries(${ADSProject} PRIVATE
        nlohmann_json::nlohmann_json
        imgui
        ${SDL2_LIBRARIES}
        spdlog::spdlog
        uuid_v4::uuid_v4
)

# ----------------------------------------------------------
# --- Test config (test mode)
# ----------------------------------------------------------
include(CTest)

if (CMAKE_BUILD_TYPE STREQUAL "Test")    ### NUEVO ###
    message(STATUS "Modo TEST activado — incluyendo GoogleTest")
    add_subdirectory(lib/googletest)
    add_subdirectory(tests)
    enable_testing()
endif ()

# ----------------------------------------------------------
# --- Useful information
# ----------------------------------------------------------
message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
