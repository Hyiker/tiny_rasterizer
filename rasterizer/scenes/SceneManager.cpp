#include "rasterizer/scenes/SceneManager.hpp"

#include <functional>
#include <map>

#include "rasterizer/Shader.hpp"
#include "utils/ObjParser.hpp"
using namespace std;
using namespace Rasterizer;

static Scene* getTreeScene() {
    Vec3 eye_pos(0.3, -4.05739, 4.46067), center(0, 0, 0.5),
        up((center - eye_pos).cross(Vec3(-1, 0, 0)).normalized());
    Camera cam(45, 1.0f, eye_pos, up, center, 0.01, 50);
    Scene* scene = new Scene(800, 800, textureFragmentShader);
    Model* model = parseOBJ("assets/tree/12150_Christmas_Tree_V2_L2.obj");
    model->scale(0.017);
    scene->addModel(model);
    scene->addLight(new Light(Vec3(0, -6, 0.6), Vec3(100, 100, 100) * 0.3));
    scene->addLight(new Light(Vec3(0, 3, 0.6), Vec3(100, 100, 100) * 0.8));
    scene->setCamera(cam);
    scene->sceneSetup();
    return scene;
}
static Scene* getMugScene() {
    Vec3 eye_pos(0, 6, 6), center(0, 0.04, 0),
        up((center - eye_pos).cross(Vec3(-1, 0, 0)).normalized());
    Camera cam(45, 1.0f, eye_pos, up, center, 0.01, 50);
    Scene* scene = new Scene(800, 800, textureFragmentShader);
    Model* model = parseOBJ("assets/mug/teamugobj.obj");
    model->scale(0.3);
    scene->addModel(model);
    scene->addLight(new Light(Vec3(7, 7, 0), Vec3(100, 100, 100) * 3));
    scene->setCamera(cam);
    scene->sceneSetup();
    return scene;
}

static Scene* getShoeScene() {
    Vec3 eye_pos(0, 6, 6), up(0, 1, 0), center(0, 0, 0);
    Camera cam(45, 1.0f, eye_pos, up, center, 0.1, 30);
    Scene* scene = new Scene(800, 800, normalFragmentShader);
    Model* model = parseOBJ("assets/shoe/Black_shoe.obj");
    model->scale(0.2);
    scene->addModel(model);
    scene->setCamera(cam);
    scene->sceneSetup();
    return scene;
}

static Scene* getRemScene() {
    Vec3 eye_pos(0, 1.2, 3), center(0, 0.5, 0),
        up((center - eye_pos).cross(Vec3(-1, 0, 0)).normalized());
    Scene* scene = new Scene(800, 800, textureFragmentShader);
    Model* model = parseOBJ("assets/rem/Rem.obj");
    scene->addModel(model);
    scene->addLight(new Light(Vec3(0, 5, 3), Vec3(500, 500, 500)));
    Camera cam(45, float(scene->width) / float(scene->height), eye_pos, up,
               center, 0.1, 50);
    scene->setCamera(cam);
    scene->sceneSetup();
    return scene;
}

const static map<string, function<Scene*()>> SCENE_MAPPING = {
    {"shoe", getShoeScene},
    {"rem", getRemScene},
    {"mug", getMugScene},
    {"tree", getTreeScene}};

Scene* SceneManager::getPredefinedSceneByName(const std::string& name) {
    auto func = SCENE_MAPPING.find(name);
    if (func != SCENE_MAPPING.end()) {
        return func->second();
    }
    std::cerr << "scene " << name << " is not pre-defined" << std::endl;
    exit(1);
}