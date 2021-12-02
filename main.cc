#include <GLFW/glfw3.h>

#include <iostream>

#include "rasterizer/Camera.hpp"
#include "rasterizer/Scene.hpp"
#include "rasterizer/Triangle.hpp"
#include "utils/ObjParser.hpp"

using namespace Rasterizer;
int main(int argc, char const* argv[]) {
    if (argc < 3) {
        std::cerr << "no enough arguments\n";
        exit(-1);
    }
    Rasterizer::Vec3 eye_pos(8, 0, 7), up(0, 1, 0), center(0, 0, 0);
    Rasterizer::Camera cam(45, 4.0f / 3.0f, eye_pos, up, center, 5, 30);
    Rasterizer::Scene scene(800, 600);
    scene.setCamera(cam);
    parseOBJ(scene, argv[2]);
    scene.render();

    scene.dumpToPPM(argv[1]);
    return 0;
}