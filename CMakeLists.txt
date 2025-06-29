cmake_minimum_required(VERSION 3.31)
project(Adventure_Designer_Studio)

set(CMAKE_CXX_STANDARD 23)
include_directories(
        src/classes
        src/exceptions
        src/constants
)

add_subdirectory(lib/i18n_keyval)
add_subdirectory(lib/nlohmann_json)

add_executable(Adventure_Designer_Studio
        main.cpp
        src/constants/languages.h
        src/classes/i18n/translations.cpp
        src/classes/i18n/translations.h
        src/exceptions/PathNotExistException.h
)

target_link_libraries(Adventure_Designer_Studio PRIVATE i18n_keyval::i18n_keyval)
target_link_libraries(Adventure_Designer_Studio PRIVATE nlohmann_json::nlohmann_json)