#include "rasterizer/scenes/SceneManager.hpp"

#include <functional>
#include <map>

#include "rasterizer/Shader.hpp"
#include "utils/ObjParser.hpp"
using namespace std;
using namespace Rasterizer;
const static map<string, function<Scene*()>> SCENE_MAPPING = {
    {"shoe", SceneManager::getShoeScene},
    {"rem", SceneManager::getRemScene},
    {"mug", SceneManager::getMugScene}};

Scene* SceneManager::getPredefinedSceneByName(const std::string& name) {
    auto func = SCENE_MAPPING.find(name);
    if (func != SCENE_MAPPING.end()) {
        return func->second();
    }
    std::cerr << "scene " << name << " is not pre-defined" << std::endl;
    exit(1);
}

Scene* SceneManager::getMugScene() {
    Vec3 eye_pos(0, 6, 6), center(0, 0.04, 0),
        up((center - eye_pos).cross(Vec3(-1, 0, 0)).normalized());
    Camera cam(45, 1.0f, eye_pos, up, center, 0.01, 50);
    Scene* scene = new Scene(800, 800, textureFragmentShader);
    Model* model = parseOBJ("assets/mug/teamugobj.obj");
    scene->addModel(model);
    scene->addLight(Light(Vec3(7, 7, 0), Vec3(300, 300, 300)));
    scene->setCamera(cam);
    return scene;
}

Scene* SceneManager::getShoeScene() {
    Vec3 eye_pos(0, 6, 6), up(0, 1, 0), center(0, 0, 0);
    Camera cam(45, 1.0f, eye_pos, up, center, 0.1, 30);
    Scene* scene = new Scene(800, 800, textureFragmentShader);
    Model* model = parseOBJ("assets/shoe/Black_shoe.obj");
    model->scale(0.2);
    scene->addModel(model);
    scene->addLight(Light(Vec3(20, 20, 20), Vec3(500, 500, 500)));
    scene->setCamera(cam);
    return scene;
}

Scene* SceneManager::getRemScene() {
    Vec3 eye_pos(0, 1.2, 3), center(0, 0.5, 0),
        up((center - eye_pos).cross(Vec3(-1, 0, 0)).normalized());
    Scene* scene = new Scene(800, 800, textureFragmentShader);
    Model* model = parseOBJ("assets/rem/Rem.obj");
    scene->addModel(model);
    scene->addLight(Light(Vec3(0, 3, 3), Vec3(500, 500, 500)));
    Camera cam(45, float(scene->width) / float(scene->height), eye_pos, up,
               center, 0.1, 50);
    scene->setCamera(cam);
    return scene;
}
