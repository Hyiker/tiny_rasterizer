#ifndef MODEL_HPP
#define MODEL_HPP
#include <vector>

#include "rasterizer/Material.hpp"
#include "rasterizer/Math.hpp"
#include "rasterizer/Mesh.hpp"
#include "rasterizer/Texture.hpp"
#include "rasterizer/Triangle.hpp"
namespace Rasterizer {
class Model {
   public:
    std::vector<Mesh*> meshes;
    Mat4 transform;
    Model() : transform{Mat4::identity()} {}
    Mat4 getModelMatrix() const { return transform; }
    void scale(float ratio) {
        Mat4 scale_mat;
        scale_mat << ratio, 0, 0, 0, 0, ratio, 0, 0, 0, 0, ratio, 0, 0, 0, 0, 1;
        transform = transform * scale_mat;
    }
};
}  // namespace Rasterizer
#endif /* MODEL_HPP */
