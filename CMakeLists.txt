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
  # src/model.cpp
  imgui/imconfig.h
  imgui/imgui.h
  imgui/imgui_internal.h
  imgui/imstb_rectpack.h
  imgui/imstb_textedit.h
  imgui/imstb_truetype.h
  imgui/imgui.cpp
  imgui/imgui_demo.cpp
  imgui/imgui_draw.cpp
  imgui/imgui_tables.cpp
  imgui/imgui_widgets.cpp
  imgui/imgui_impl_glfw.h
  imgui/imgui_impl_glfw.cpp
  imgui/imgui_impl_opengl3.h
  imgui/imgui_impl_opengl3.cpp
)

target_include_directories(renderer PRIVATE 
  ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(renderer
  glfw
  stb
  assimp
)
