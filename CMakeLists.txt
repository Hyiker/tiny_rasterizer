cmake_minimum_required(VERSION 3.13)
set(LANG CXX)
if(APPLE)
    set(CMAKE_C_COMPILER /usr/bin/clang++)
    set(CMAKE_CXX_COMPILER /usr/bin/clang++)
    set(LANG ${LANG} OBJCXX)
endif()
project(tiny_rasterizer VERSION 0.0.1 LANGUAGES ${LANG})

set(CMAKE_CXX_STANDARD 17)
set(TARGET rasterizer)
include_directories(./)

set(SOURCES
    rasterizer/Shader.cpp
    rasterizer/Math.cpp
    utils/Measure.cpp
    lib/lodepng.cpp
    main.cpp
)
set(HEADERS 
    lib/lodepng.h
    lib/OBJ_Loader.h
    utils/GitHelper.hpp
    utils/ObjParser.hpp
    utils/Measure.hpp
    rasterizer/Scene.hpp
    rasterizer/Camera.hpp 
    rasterizer/Shader.hpp
    rasterizer/Model.hpp
    rasterizer/Triangle.hpp
    rasterizer/Light.hpp
    rasterizer/Material.hpp
    rasterizer/Math.hpp
    rasterizer/Texture.hpp
    rasterizer/scenes/SceneManager.hpp
    graphics/Viewer.hpp
    graphics/platform.h
)

set(BENCH_HEADERS
    rasterizer/Math.hpp
    benchmark/MathBench.hpp
    utils/GitHelper.hpp
    utils/Measure.hpp
)

set(BENCH_SOURCES
    rasterizer/Math.cpp
    benchmark/BenchMain.cpp
    utils/Measure.cpp
)
##################################################
# ARCHITECTURE SPECIFICATION
##################################################
if (CMAKE_SYSTEM_PROCESSOR MATCHES "(arm)|(arm64)|(arm32)")
    message("DETECTED ARCHITECTURE " ${CMAKE_SYSTEM_PROCESSOR} ", OPTIMIZING USING NEON")
    set(SOURCES rasterizer/math_arch/MathNeon.cpp ${SOURCES})
    set(BENCH_SOURCES rasterizer/math_arch/MathNeon.cpp ${BENCH_SOURCES})
else()
    message("DETECTED ARCHITECTURE " ${CMAKE_SYSTEM_PROCESSOR} ", OPTIMIZING USING NEON")
endif()


if(APPLE)
    set(SOURCES ${SOURCES} graphics/macos.mm)
endif()

add_executable(${TARGET} ${HEADERS} ${SOURCES})

##################################################
# ASSETS
##################################################
set(ASSETS_SRC "${CMAKE_SOURCE_DIR}/assets")
set(ASSETS_DST "$<TARGET_FILE_DIR:${TARGET}>/assets")

if(WIN32)
    set(ASSETS_CMD copy_directory)
    set(ASSETS_MSG "Copy assets")
else()
    set(ASSETS_CMD create_symlink)
    set(ASSETS_MSG "Link assets")
endif()


add_custom_command(
    TARGET ${TARGET} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo
    "${ASSETS_MSG}: ${ASSETS_SRC} to ${ASSETS_DST}"
)
add_custom_command(
    TARGET ${TARGET} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E ${ASSETS_CMD}
    "${ASSETS_SRC}" "${ASSETS_DST}"
)

if(APPLE)
    target_link_libraries(${TARGET} PRIVATE "-framework Cocoa")
endif()

##################################################
# BENCHMARK
##################################################
# Get git hash branch name: https://zhuanlan.zhihu.com/p/57319492
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(Utility)
set(GIT_HASH "")
get_git_hash(GIT_HASH)
message(STATUS "Git hash is ${GIT_HASH}")

set(GIT_BRANCH "")
get_git_branch(GIT_BRANCH)
message(STATUS "Git branch is ${GIT_BRANCH}")
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/utils/GitHelper.hpp.in
  ${CMAKE_CURRENT_SOURCE_DIR}/utils/GitHelper.hpp
  @ONLY
)

set(BENCH_TARGET bench)


add_executable(${BENCH_TARGET} ${BENCH_SOURCES} ${BENCH_HEADERS})
