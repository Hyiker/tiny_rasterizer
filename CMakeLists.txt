cmake_minimum_required(VERSION 3.13)
project(tiny_rasterizer)

set(CMAKE_CXX_STANDARD 17)

find_package(Eigen3 REQUIRED)

include_directories(${EIGEN3_INCLUDE_DIRS})

set(SOURCE_FILES src/main.cc)

add_executable(rasterizer ${SOURCE_FILES})