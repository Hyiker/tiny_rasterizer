cmake_minimum_required(VERSION 3.13)
project(tiny_rasterizer)

set(CMAKE_CXX_STANDARD 17)

include_directories(./)

set(SOURCE_FILES main.cc rasterizer/Scene.hpp rasterizer/Camera.hpp rasterizer/Triangle.hpp rasterizer/Math.hpp)

add_executable(rasterizer ${SOURCE_FILES})