cmake_minimum_required(VERSION 3.14...3.31)

set(ADSProject Adventure_Designer_Studio)

project(${ADSProject} CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(IMGUI_DIR "lib/imgui")

# ----------------------------------------------------------
# --- Config build environments
# ----------------------------------------------------------
# By default CMake use only one, but we can force to use three ones
set(AVAILABLE_BUILD_TYPES Debug Release Test)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the build type." FORCE)
endif()

set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${AVAILABLE_BUILD_TYPES})

message(STATUS "Selected build type: ${CMAKE_BUILD_TYPE}")

# ----------------------------------------------------------
# --- Compilation flags configuration
# ----------------------------------------------------------
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS_DEBUG   "-g3 -O0 -Wall -Wextra -DDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
    set(CMAKE_CXX_FLAGS_TEST    "-g3 -O0 -Wall -Wextra -DTEST_MODE")   ### NUEVO ###
elseif(MSVC)
    set(CMAKE_CXX_FLAGS_DEBUG   "/Zi /Od /DEBUG:FULL /DDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "/O2 /DNDEBUG")
    set(CMAKE_CXX_FLAGS_TEST    "/Zi /Od /DEBUG:FULL /DTEST_MODE")     ### NUEVO ###
endif()

# ----------------------------------------------------------
# --- Copy resources (translations)
# ----------------------------------------------------------
if(NOT EXISTS "${CMAKE_BINARY_DIR}/translations")
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/translations/core")
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/translations/game")
endif ()
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/translations/" DESTINATION "${CMAKE_BINARY_DIR}/translations")

if(NOT EXISTS "${CMAKE_BINARY_DIR}/public")
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/public/assets")
endif ()
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/public/" DESTINATION "${CMAKE_BINARY_DIR}/public")

# ----------------------------------------------------------
# --- External dependencies
# ----------------------------------------------------------
find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)
add_subdirectory(lib/nlohmann_json)

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
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${SDL2_INCLUDE_DIRS}
)

# ----------------------------------------------------------
# --- Header and source files
# ----------------------------------------------------------
set(Headers
    src/exceptions/path_not_exist_exception.h
    src/exceptions/file_not_found_exception.h
    src/exceptions/file_not_open_exception.h
    src/constants/languages.h
    src/classes/i18n/i18n.h
)

set(Sources
    main.cpp
    src/classes/i18n/i18n.cpp
        src/classes/env/env.cpp
        src/classes/env/env.h
    include/string.h
    include/string.cpp
    include/i18nUtils.h
    include/i18nUtils.cpp
    src/constants/SystemConst.h
)

# ----------------------------------------------------------
# --- Main executable
# ----------------------------------------------------------
add_executable(${ADSProject} ${Sources} ${Headers})

target_link_libraries(${ADSProject} PRIVATE
    nlohmann_json::nlohmann_json
    imgui
    ${SDL2_LIBRARIES}
)

# ----------------------------------------------------------
# --- Test config (test mode)
# ----------------------------------------------------------
include(CTest)

if(CMAKE_BUILD_TYPE STREQUAL "Test")    ### NUEVO ###
    message(STATUS "Modo TEST activado — incluyendo GoogleTest")
    add_subdirectory(lib/googletest)
    add_subdirectory(tests)
    enable_testing()
endif()

# ----------------------------------------------------------
# --- Useful information
# ----------------------------------------------------------
message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
