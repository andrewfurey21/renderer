cmake_minimum_required(VERSION 3.0)

project(renderer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

cmake_policy(SET CMP0072 NEW)
find_package(OpenGL REQUIRED)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")
add_executable(renderer
  include/glad/src/glad.c
  src/main.cpp
)

target_include_directories(renderer PRIVATE 
  ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(renderer
  glfw
  stb
)
