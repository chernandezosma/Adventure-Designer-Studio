cmake_minimum_required(VERSION 3.14...3.31)

set (ADSProject Adventure_Designer_Studio)

project(${ADSProject} CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/translations/" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/translations")


enable_testing()

include_directories(
        src/classes
        src/exceptions
        src/constants
)

#add_subdirectory(lib/i18n_keyval)
add_subdirectory(lib/nlohmann_json)
add_subdirectory(lib/googletest)

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
add_subdirectory(tests)

target_link_libraries(${ADSProject} PRIVATE nlohmann_json::nlohmann_json)

#target_link_libraries(Adventure_Designer_Studio PUBLIC GTest::gtest_main)
#target_link_libraries(tests gtest GTest::gtest_main)
#
#include(GoogleTest)
#gtest_discover_tests(Adventure_Designer_Studio_Test)