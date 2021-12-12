#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include "rasterizer/Scene.hpp"
namespace Rasterizer {

namespace SceneManager {
Scene* getPredefinedSceneByName(const std::string& name);
Scene* getMugScene();
Scene* getShoeScene();
Scene* getRemScene();
}  // namespace SceneManager
}  // namespace Rasterizer

#endif /* SCENEMANAGER_H */
