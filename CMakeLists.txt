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
# Copy at configure time so resources are available immediately
message(STATUS "Copying the content of public folder: ${CMAKE_BUILD_TYPE}")
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/public DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Copy on every build to pick up changes in source public folder
add_custom_target(copy_public_folder ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/public
        ${CMAKE_CURRENT_BINARY_DIR}/public
        COMMENT "Copying public folder to build directory"
)

# ----------------------------------------------------------
# --- Copy .env file
# ----------------------------------------------------------
add_custom_target(copy_env_file ALL
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/.env
        ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/.env
        COMMENT "Copying .env file to build directory"
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
find_package(nfd CONFIG REQUIRED)

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
        # Inspector system
        src/classes/Inspector/PropertyConstraints.cpp
        src/classes/Inspector/PropertyConstraints.h
        src/classes/Inspector/PropertyDescriptor.cpp
        src/classes/Inspector/PropertyDescriptor.h
        src/classes/Inspector/PropertyEditorRegistry.cpp
        src/classes/Inspector/PropertyEditorRegistry.h
        src/classes/Inspector/PropertyEvent.cpp
        src/classes/Inspector/PropertyEvent.h
        src/classes/Inspector/PropertyType.h
        src/classes/Inspector/PropertyValue.h
        src/classes/Inspector/IInspectable.h
        # Inspector editors
        src/classes/Inspector/Editors/IPropertyEditor.h
        src/classes/Inspector/Editors/StringEditor.cpp
        src/classes/Inspector/Editors/StringEditor.h
        src/classes/Inspector/Editors/IntEditor.cpp
        src/classes/Inspector/Editors/IntEditor.h
        src/classes/Inspector/Editors/FloatEditor.cpp
        src/classes/Inspector/Editors/FloatEditor.h
        src/classes/Inspector/Editors/BoolEditor.cpp
        src/classes/Inspector/Editors/BoolEditor.h
        src/classes/Inspector/Editors/EnumEditor.cpp
        src/classes/Inspector/Editors/EnumEditor.h
        src/classes/Inspector/Editors/ColorEditor.cpp
        src/classes/Inspector/Editors/ColorEditor.h
        src/classes/Inspector/Editors/Vector2Editor.cpp
        src/classes/Inspector/Editors/Vector2Editor.h
        # Portable types (no rendering dependency)
        src/classes/Types/Color.h
        # DataObject layer (pure data, serialisable)
        src/classes/Data/BaseData.h
        src/classes/Data/SceneData.h
        src/classes/Data/CharacterData.h
        src/classes/Data/ItemData.h
        # Entity classes (inspector adapters)
        src/classes/Entities/BaseEntity.cpp
        src/classes/Entities/BaseEntity.h
        src/classes/Entities/Scene.cpp
        src/classes/Entities/Scene.h
        src/classes/Entities/Character.cpp
        src/classes/Entities/Character.h
        src/classes/Entities/Item.cpp
        src/classes/Entities/Item.h
        # Core classes
        src/classes/Core/Project.cpp
        src/classes/Core/Project.h
)

# ----------------------------------------------------------
# --- Main executable
# ----------------------------------------------------------
add_executable(${ADSProject} ${Sources} ${Headers})
add_dependencies(${ADSProject} copy_public_folder copy_env_file)

target_link_libraries(${ADSProject} PRIVATE
        nlohmann_json::nlohmann_json
        imgui::imgui
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
        $<IF:$<TARGET_EXISTS:SDL2::SDL2main>,SDL2::SDL2main,SDL2::SDL2main-static>
        spdlog::spdlog
        fmt::fmt
        Boost::uuid
        Boost::headers
        nfd::nfd
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
