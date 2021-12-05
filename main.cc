#include <GLFW/glfw3.h>

#include <chrono>
#include <ctime>
#include <functional>
#include <iostream>

#include "rasterizer/Camera.hpp"
#include "rasterizer/Scene.hpp"
#include "rasterizer/Shader.hpp"
#include "utils/ObjParser.hpp"

using namespace Rasterizer;
using namespace std;

int64_t measure(std::function<void()> func) {
    auto start_ms = chrono::duration_cast<chrono::milliseconds>(
                        chrono::system_clock::now().time_since_epoch())
                        .count();
    func();
    auto end_ms = chrono::duration_cast<chrono::milliseconds>(
                      chrono::system_clock::now().time_since_epoch())
                      .count();
    auto duration = end_ms - start_ms;
    return duration;
}

int main(int argc, char const* argv[]) {
    if (argc < 3) {
        std::cerr << "no enough arguments\n";
        exit(-1);
    }
    Vec3 eye_pos(0, 5, 5), up(0, 1, 0), center(0, 0, 0);
    Camera cam(45, 1.0f, eye_pos, up, center, 0.1, 30);
    Scene scene(800, 800, textureFragmentShader);
    Model* model = parseOBJ(argv[2]);
    model->scale(0.2);
    scene.addModel(model);
    scene.addLight(Light(Vec3(20, 20, 20), Vec3(500, 500, 500)));
    if (argc >= 4) {
        model->texture = Texture::loadPNGTexture(argv[3]);
    }
    scene.setCamera(cam);
    auto duration = measure([&]() { scene.render(); });
    std::cout << "\nmilliseconds cost: " << duration << "\n"
              << "estimated fps: " << (1000.0f / float(duration));
    scene.dumpToPNG(argv[1]);
    return 0;
}
