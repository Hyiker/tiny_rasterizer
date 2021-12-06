#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include "rasterizer/Scene.hpp"
#include "rasterizer/Shader.hpp"
#include "utils/ObjParser.hpp"
namespace Rasterizer {
struct SceneManager {
    static Scene* getPredefinedSceneByName(const std::string& name) {
        if (name == "shoe") {
            return SceneManager::getShoeScene();
        }
        std::cerr << "scene " << name << " is not pre-defined" << std::endl;
        exit(1);
    }

   private:
    static Scene* getShoeScene() {
        Vec3 eye_pos(0, 6, 6), up(0, 1, 0), center(0, 0, 0);
        Camera cam(45, 1.0f, eye_pos, up, center, 0.1, 30);
        Scene* scene = new Scene(800, 800, textureFragmentShader);
        Model* model = parseOBJ("assets/shoe/Black_shoe.obj");
        model->scale(0.2);
        scene->addModel(model);
        scene->addLight(Light(Vec3(10, 10, 10), Vec3(500, 500, 500)));
        scene->setCamera(cam);
        return scene;
    }
};
}  // namespace Rasterizer

#endif /* SCENEMANAGER_H */
