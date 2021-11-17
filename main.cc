#include <iostream>

#include "rasterizer/Camera.hpp"
#include "rasterizer/Scene.hpp"
#include "rasterizer/Triangle.hpp"

using namespace Rasterizer;
int main(int argc, char const* argv[]) {
    Rasterizer::Scene<800, 600> scene;

    Rasterizer::Triangle* tri =
        new Triangle({Vec3(2, 3, -4), Vec3(3, 4, -5), Vec3(7, 7, -7)});

    scene.addTriangle(tri);
    scene.render();

    // scene.dumpToPPM("out/black.ppm");

    return 0;
}
