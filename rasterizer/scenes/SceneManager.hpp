#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include "rasterizer/Scene.hpp"
namespace Rasterizer {

namespace SceneManager {
Scene* getPredefinedSceneByName(const std::string& name);
}  // namespace SceneManager
}  // namespace Rasterizer

#endif /* SCENEMANAGER_H */
