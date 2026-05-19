# CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project(ForthGraphics)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Добавляем путь к модулям CMake
set(SFML_DIR "D:/Test/SFML-3.0.2/lib/cmake/SFML/")

# Ищем SFML (использует FindSFML.cmake)
find_package(SFML 3 REQUIRED COMPONENTS  Graphics System Window ) #Audio Graphics Main Network System Window

#add_executable(forth_graphics test_graphics.cpp)
add_executable(graphics_only test_simple.cpp)

target_link_libraries(graphics_only SFML::Graphics SFML::Window SFML::System)

# target_include_directories(forth_graphics PRIVATE ${SFML_INCLUDE_DIR})
