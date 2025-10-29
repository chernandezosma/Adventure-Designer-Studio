cmake_minimum_required(VERSION 3.14...3.31)

set (ADSProject Adventure_Designer_Studio)

project(${ADSProject} CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(IMGUI_DIR "lib/imgui")

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/translations/" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/translations")

# --- Configurar testeo opcional ---
include(CTest)  # define la opción estándar BUILD_TESTING

if(BUILD_TESTING)
    add_subdirectory(lib/googletest)
endif()

#enable_testing()

# --- Dependencias externas ---
find_package(OpenGL REQUIRED)
# require sudo apt install libglfw3-dev
#find_package(glfw3 3.3 REQUIRED)
# require sudo apt install libsdl2-dev
find_package(SDL2 REQUIRED)

add_subdirectory(lib/nlohmann_json)
#add_subdirectory(lib/googletest)

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

include_directories(
        src/classes
        src/exceptions
        src/constants
        ${IMGUI_DIR}
        ${IMGUI_DIR}/backends
        ${SDL2_INCLUDE_DIRS}
)

set (Headers
    src/exceptions/path_not_exist_exception.h
    src/exceptions/not_found_exception.h
    src/exceptions/file_not_open_exception.h
    src/constants/languages.h
    src/classes/i18n/i18n.h
)

set(Sources
    main.cpp
    src/classes/i18n/i18n.cpp
)

add_executable(${ADSProject} ${Sources} ${Headers})
#add_subdirectory(tests)

#target_link_libraries(${ADSProject} PRIVATE nlohmann_json::nlohmann_json imgui glfw)
target_link_libraries(${ADSProject} PRIVATE
        nlohmann_json::nlohmann_json
        imgui
        ${SDL2_LIBRARIES}
#        OpenGL::GL
)

# --- Tests (solo si está habilitado) ---
if(BUILD_TESTING)
    message(STATUS "Compilando tests...")
    add_subdirectory(tests)
endif()

#target_link_libraries(Adventure_Designer_Studio PUBLIC GTest::gtest_main)
#target_link_libraries(tests gtest GTest::gtest_main)
#
#include(GoogleTest)
#gtest_discover_tests(Adventure_Designer_Studio_Test)