cmake_minimum_required(VERSION 3.14...3.31)

set(ADSProject_Tests Adventure_Designer_Studio_Tests)

# ============================================================================
# Extract testable libraries from main project
# ============================================================================

# i18n library
add_library(ads_i18n STATIC ../src/classes/i18n/i18n.cpp)
target_include_directories(ads_i18n PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/classes
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/exceptions
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/constants
)
target_link_libraries(ads_i18n PUBLIC nlohmann_json::nlohmann_json spdlog::spdlog)

# Utilities library (adsString, env, i18nUtils, etc.)
add_library(ads_utils STATIC
        ../src/include/adsString.cpp
        ../src/include/i18nUtils.cpp
        ../src/classes/env/env.cpp
)
target_include_directories(ads_utils PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/classes
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/constants
)
target_link_libraries(ads_utils PUBLIC nlohmann_json::nlohmann_json)

# Inspector library (property metadata/constraints/events/editor registry)
find_package(imgui CONFIG REQUIRED)

add_library(ads_inspector STATIC
        ../src/classes/Inspector/PropertyConstraints.cpp
        ../src/classes/Inspector/PropertyDescriptor.cpp
        ../src/classes/Inspector/PropertyEvent.cpp
        ../src/classes/Inspector/PropertyEditorRegistry.cpp
        ../src/classes/Inspector/Editors/StringEditor.cpp
        ../src/classes/Inspector/Editors/IntEditor.cpp
        ../src/classes/Inspector/Editors/FloatEditor.cpp
        ../src/classes/Inspector/Editors/BoolEditor.cpp
        ../src/classes/Inspector/Editors/EnumEditor.cpp
        ../src/classes/Inspector/Editors/ColorEditor.cpp
        ../src/classes/Inspector/Editors/Vector2Editor.cpp
        ../src/classes/Inspector/Editors/SelectEditor.cpp
        ../src/classes/Inspector/Editors/AffordanceListEditor.cpp
)
target_include_directories(ads_inspector PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/classes
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/constants
        ${CMAKE_CURRENT_SOURCE_DIR}/../lib/IconFontCppHeaders
)
target_link_libraries(ads_inspector PUBLIC imgui::imgui)

# Entities library (inspector adapters over DataObjects)
add_library(ads_entities STATIC
        ../src/classes/Data/SceneData.cpp
        ../src/classes/Entities/BaseEntity.cpp
        ../src/classes/Entities/Scene.cpp
        ../src/classes/Entities/Character.cpp
        ../src/classes/Entities/Item.cpp
        ../src/classes/Entities/State.cpp
        ../src/classes/Entities/StateChain.cpp
)
target_include_directories(ads_entities PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/classes
)
target_link_libraries(ads_entities PUBLIC ads_inspector imgui::imgui)

# LexEngine library (vocabulary compiler subsystem)
add_library(ads_lexengine STATIC
        ../src/classes/LexEngine/LexEntry.cpp
        ../src/classes/LexEngine/FallbackNLPBackend.cpp
        ../src/classes/LexEngine/SynonymPipeline.cpp
        ../src/classes/LexEngine/LexEngine.cpp
        ../src/classes/LexEngine/LexEngineSerializer.cpp
)
target_include_directories(ads_lexengine PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/classes
)
target_link_libraries(ads_lexengine PUBLIC nlohmann_json::nlohmann_json spdlog::spdlog)

# Core library (Project — owns DataObjects + entity adapters)
find_package(SDL3 CONFIG REQUIRED)

add_library(ads_core STATIC
        ../src/classes/Core/Project.cpp
        ../src/classes/Core/ProjectSerializer.cpp
        ../src/classes/Core/PathService.cpp
        ../src/classes/Core/TranslationCatalog.cpp
        ../src/classes/Core/TranslationSerializer.cpp
)
target_include_directories(ads_core PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/classes
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/exceptions
)
target_link_libraries(ads_core PUBLIC ads_entities ads_lexengine nlohmann_json::nlohmann_json spdlog::spdlog fmt::fmt SDL3::SDL3)

# ============================================================================
# Gather all test sources
# ============================================================================
file(GLOB_RECURSE TEST_SOURCES "*.cpp")

# ============================================================================
# Create test executable
# ============================================================================
add_executable(${ADSProject_Tests} ${TEST_SOURCES})

# ============================================================================
# Link libraries
# ============================================================================
target_link_libraries(${ADSProject_Tests} PUBLIC
        ads_i18n
        ads_utils
        ads_inspector
        ads_entities
        ads_lexengine
        ads_core
        GTest::gtest_main
        GTest::gtest
)

# ============================================================================
# Configure include paths
# ============================================================================
target_include_directories(${ADSProject_Tests} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/classes
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/exceptions
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/constants
)

# ============================================================================
# Register tests
# ============================================================================
add_test(
        NAME ${ADSProject_Tests}
        COMMAND ${ADSProject_Tests}
)

include(GoogleTest)
gtest_discover_tests(${ADSProject_Tests})
