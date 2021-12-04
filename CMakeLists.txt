cmake_minimum_required(VERSION 3.13)
project(tiny_rasterizer)

set(CMAKE_CXX_STANDARD 17)

include_directories(./)

find_package(glfw3 3.3 REQUIRED)

set(SOURCE_FILES main.cc utils/ObjParser.hpp rasterizer/Scene.hpp rasterizer/Camera.hpp 
                    rasterizer/Shader.hpp rasterizer/Model.hpp rasterizer/Triangle.hpp
                    rasterizer/Light.hpp
                    rasterizer/Math.hpp window/Window.hpp
                    rasterizer/Texture.hpp
                    lib/lodepng.h lib/lodepng.cpp lib/OBJ_Loader.h)

add_executable(rasterizer ${SOURCE_FILES})
target_link_libraries(rasterizer glfw)