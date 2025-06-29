cmake_minimum_required(VERSION 3.14...3.31)


set (ADSProject_Tests Adventure_Designer_Studio_Tests)

add_library(i18n_lib STATIC ../src/classes/i18n/i18n.cpp)
target_include_directories(
        i18n_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/i18n
)
set (Sources
        I18nTests.cpp
)
add_executable(${ADSProject_Tests} ${Sources})

target_link_libraries(${ADSProject_Tests} PUBLIC
    i18n_lib
    gtest_main
    gtest
)

target_include_directories(${ADSProject_Tests} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR} # For languages.h and i_18_n_tests.h
        ${CMAKE_CURRENT_SOURCE_DIR}/i18n # For i18n.h
)

add_test(
    NAME ${ADSProject_Tests}
    COMMAND ${ADSProject_Tests}
)