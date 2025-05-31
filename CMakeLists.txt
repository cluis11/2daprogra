cmake_minimum_required(VERSION 3.15)
project(GeneticKingdom)

# Configuración explícita para SFML 3.0
set(SFML_VERSION 3.0)

# Configuración del compilador
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Configuración específica para Visual Studio
if(MSVC)
    add_compile_options(/W4 /WX /permissive-)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS NOMINMAX)
endif()

# Buscar SFML 3.0 con sintaxis moderna
find_package(SFML ${SFML_VERSION} CONFIG REQUIRED
    COMPONENTS Graphics Window System
)


add_executable(GeneticKingdom
    src/main.cpp
    src/Game.cpp
    src/Wave.cpp
    src/GridSystem.cpp
    src/Entity.cpp
    src/Tower.cpp
    src/EnemyGenome.cpp
    src/GeneticManager.cpp
    src/Enemy.cpp
    src/PathFinding.cpp
    src/GameStats.cpp
)

# Copy assets to build directory
file(COPY assets/ DESTINATION ${CMAKE_BINARY_DIR}/assets)

# For multi-config generators (like Visual Studio)
if(CMAKE_CONFIGURATION_TYPES)
    file(COPY assets/ DESTINATION ${CMAKE_BINARY_DIR}/Debug/assets)
    file(COPY assets/ DESTINATION ${CMAKE_BINARY_DIR}/Release/assets)
endif()

target_include_directories(GeneticKingdom PRIVATE include)

# Vincular SFML 3.0 con targets modernos
target_link_libraries(GeneticKingdom PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
)

# Mensajes de verificación
message(STATUS "SFML version: ${SFML_VERSION}")
message(STATUS "SFML include dir: ${SFML_INCLUDE_DIR}")
message(STATUS "Assets will be copied to: ${CMAKE_BINARY_DIR}/assets")