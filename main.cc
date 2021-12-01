#include <GLFW/glfw3.h>

#include <iostream>

#include "rasterizer/Camera.hpp"
#include "rasterizer/Scene.hpp"
#include "rasterizer/Triangle.hpp"

using namespace Rasterizer;
int main(int argc, char const* argv[]) {
    Rasterizer::Camera cam(45, 4.0f / 3.0f, Vec3(0, 0, 5), Vec3(0, 0, 1), 0.1,
                           50);
    Rasterizer::Scene<800, 600> scene;
    scene.setCamera(cam);

    Rasterizer::Triangle* tri1 = new Triangle({
        Vertex(Vec3(-0.01, -0.01, 0), RGBColor(255, 0, 0).rgbNormalized(),
               Vec3(0, 0, -1)),
        Vertex(Vec3(0.009, 0, 0), RGBColor(0, 255, 0).rgbNormalized(),
               Vec3(0, 0, -1)),
        Vertex(Vec3(0, 0.003, 0), RGBColor(0, 0, 255).rgbNormalized(),
               Vec3(0, 0, -1)),
    });
    Rasterizer::Triangle* tri2 = new Triangle({
        Vertex(Vec3(0, 0, 0.01), RGBColor(144, 15, 0).rgbNormalized(),
               Vec3(0, 0, -1)),
        Vertex(Vec3(0.01, 0, -0.01), RGBColor(190, 255, 16).rgbNormalized(),
               Vec3(0, 0, -1)),
        Vertex(Vec3(0.018, 0.02, 0), RGBColor(0, 0, 255).rgbNormalized(),
               Vec3(0, 0, -1)),
    });

    scene.addTriangle(tri1);
    scene.addTriangle(tri2);
    scene.render();

    if (argc == 1) {
        std::cerr << "output path not provided\n";
        exit(-1);
    }

    scene.dumpToPPM(argv[1]);
    return 0;
}
