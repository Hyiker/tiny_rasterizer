#ifndef MODEL_HPP
#define MODEL_HPP
#include <vector>

#include "rasterizer/Math.hpp"
#include "rasterizer/Texture.hpp"
#include "rasterizer/Triangle.hpp"
namespace Rasterizer {
class Model {
   public:
    Texture* texture;
    std::vector<Triangle*> triangles;
    Mat4 transform;
    Model() : texture{nullptr}, transform{Mat4::identity()} {}

    Mat4 getModelMatrix() const { return transform; }

    void addTriangle(Triangle* tri) { this->triangles.emplace_back(tri); }

    ~Model() {
        delete texture;
        for (auto p_tri : triangles) {
            delete p_tri;
            p_tri = nullptr;
        }
    }
};
}  // namespace Rasterizer
#endif /* MODEL_HPP */
