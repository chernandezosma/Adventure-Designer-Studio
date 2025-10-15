cmake_minimum_required(VERSION 3.14...3.31)

set(ADSProject_Tests Adventure_Designer_Studio_Tests)

# Crear librería estática con el código fuente de i18n
add_library(i18n_lib STATIC ../src/classes/i18n/i18n.cpp)

# Configurar includes para la librería i18n
target_include_directories(i18n_lib PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/classes
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/exceptions
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/constants
)

# Enlazar nlohmann_json a la librería i18n
target_link_libraries(i18n_lib PUBLIC nlohmann_json::nlohmann_json)

# Archivos fuente de los tests
set(Sources
        i18nTests.cpp
)

# Crear ejecutable de tests
add_executable(${ADSProject_Tests} ${Sources})

# Enlazar librerías
target_link_libraries(${ADSProject_Tests} PUBLIC
        i18n_lib
        gtest_main
        gtest
)

# Configurar directorios de include para los tests
target_include_directories(${ADSProject_Tests} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}                     # Para i18nTests.h
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/classes      # Para i18n/i18n.h
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/exceptions   # Para las excepciones
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/constants    # Para languages.h
)

# Añadir el test
add_test(
        NAME ${ADSProject_Tests}
        COMMAND ${ADSProject_Tests}
)

# Descubrir tests de Google Test automáticamente
include(GoogleTest)
gtest_discover_tests(${ADSProject_Tests})