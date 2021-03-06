#ifndef MESH_H
#define MESH_H
#include "rasterizer/Material.hpp"
#include "rasterizer/Texture.hpp"
#include "rasterizer/Triangle.hpp"
namespace Rasterizer {
struct Mesh {
    std::vector<Triangle*> triangles;
    Material* material;
    std::string name;
    Mesh() : material{nullptr} {}
};
}  // namespace Rasterizer

#endif /* MESH_H */
