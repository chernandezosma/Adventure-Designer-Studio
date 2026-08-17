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
)
target_include_directories(ads_inspector PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/classes
)
target_link_libraries(ads_inspector PUBLIC imgui::imgui)

# Entities library (inspector adapters over DataObjects)
add_library(ads_entities STATIC
        ../src/classes/Entities/BaseEntity.cpp
        ../src/classes/Entities/Scene.cpp
        ../src/classes/Entities/Character.cpp
        ../src/classes/Entities/Item.cpp
)
target_include_directories(ads_entities PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/classes
)
target_link_libraries(ads_entities PUBLIC ads_inspector imgui::imgui)

# Lexicon library (Lexingine — vocabulary compiler subsystem)
add_library(ads_lexicon STATIC
        ../src/classes/Lexicon/LexEntry.cpp
        ../src/classes/Lexicon/FallbackNLPBackend.cpp
        ../src/classes/Lexicon/SynonymPipeline.cpp
        ../src/classes/Lexicon/Lexicon.cpp
        ../src/classes/Lexicon/LexiconSerializer.cpp
)
target_include_directories(ads_lexicon PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/classes
)
target_link_libraries(ads_lexicon PUBLIC nlohmann_json::nlohmann_json spdlog::spdlog)

# Core library (Project — owns DataObjects + entity adapters)
add_library(ads_core STATIC
        ../src/classes/Core/Project.cpp
)
target_include_directories(ads_core PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/classes
)
target_link_libraries(ads_core PUBLIC ads_entities ads_lexicon spdlog::spdlog fmt::fmt)

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
        ads_lexicon
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
