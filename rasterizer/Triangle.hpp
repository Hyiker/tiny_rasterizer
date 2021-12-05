#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

#include <array>
#include <tuple>

#include "rasterizer/Math.hpp"
namespace Rasterizer {
class Vertex {
   public:
    Vec3 coord;
    RGBColor color;
    Vec3 normal;
    Vec3 texture_coord;
    Vertex(const Vec3& coord = Vec3(0, 0, 0),
           const RGBColor& color = RGBColor(1.0),
           const Vec3& normal = Vec3(0, 0, 1),
           const Vec3& texture_coord = Vec3(0, 0, 0))
        : coord{coord},
          color{color},
          normal{normal},
          texture_coord{texture_coord} {}
};
class Triangle {
   public:
    std::array<Vertex, 3> v;
    // TODO: use mesh to hold material payload
    Material* material;
    Triangle() = default;
    Triangle(const std::array<Vertex, 3>& v, Material* material = nullptr)
        : v{v}, material{material} {}
    bool inside(const Rasterizer::Vec3& p, bool remove_z = true) const {
        // get 3 edge vector and point-vertex vector
        Rasterizer::Vec3 e1 = v[0].coord - p, e2 = v[1].coord - p,
                         e3 = v[2].coord - p;
        if (remove_z) {
            e1.z = e2.z = e3.z = 0.0f;
        }
        // cross product pv & edges
        Rasterizer::Vec3 c1 = e1.cross(e2), c2 = e2.cross(e3),
                         c3 = e3.cross(e1);
        float d1 = c1.z, d2 = c2.z, d3 = c3.z;
        return (d1 >= 0 && d2 >= 0 && d3 >= 0) ||
               (d1 <= 0 && d2 <= 0 && d3 <= 0);
    }
    std::tuple<float, float, float> computeBarycentric2D(float x,
                                                         float y) const {
        float c1 = (x * (v[1].coord.y - v[2].coord.y) +
                    (v[2].coord.x - v[1].coord.x) * y +
                    v[1].coord.x * v[2].coord.y - v[2].coord.x * v[1].coord.y) /
                   (v[0].coord.x * (v[1].coord.y - v[2].coord.y) +
                    (v[2].coord.x - v[1].coord.x) * v[0].coord.y +
                    v[1].coord.x * v[2].coord.y - v[2].coord.x * v[1].coord.y);
        float c2 = (x * (v[2].coord.y - v[0].coord.y) +
                    (v[0].coord.x - v[2].coord.x) * y +
                    v[2].coord.x * v[0].coord.y - v[0].coord.x * v[2].coord.y) /
                   (v[1].coord.x * (v[2].coord.y - v[0].coord.y) +
                    (v[0].coord.x - v[2].coord.x) * v[1].coord.y +
                    v[2].coord.x * v[0].coord.y - v[0].coord.x * v[2].coord.y);
        float c3 = 1.0f - c1 - c2;
        return {c1, c2, c3};
    }
    float interpolateZ(float u, float v, float w) const {
        return this->v[0].coord.z * u + this->v[1].coord.z * v +
               this->v[2].coord.z * w;
    }
    Vec3 interpolateNormal(float u, float v, float w) const {
        return this->v[0].normal * u + this->v[1].normal * v +
               this->v[2].normal * w;
    }

    Vec3 interpolateTextureCoord(float u, float v, float w) const {
        return this->v[0].texture_coord * u + this->v[1].texture_coord * v +
               this->v[2].texture_coord * w;
    }
    RGBColor interpolateColor(float u, float v, float w) const {
        return this->v[0].color * u + this->v[1].color * v +
               this->v[2].color * w;
    }
};

}  // namespace Rasterizer

#endif /* TRIANGLE_HPP */
