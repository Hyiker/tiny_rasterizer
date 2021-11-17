#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

#include <array>

#include "rasterizer/Math.hpp"
namespace Rasterizer {
using RGBColor = Vec3;
class Triangle {
   public:
    std::array<Vec3, 3> v;
    std::array<RGBColor, 3> vertex_color;
    std::array<Vec3, 3> vertex_normal;
    Triangle() = default;
    Triangle(const std::array<Rasterizer::Vec3, 3>& v) : v{v} {}
    bool inside(const Rasterizer::Vec3& p) {
        // get 3 edge vector and point-vertex vector
        Rasterizer::Vec3 e1 = v[0] - v[1], e2 = v[1] - v[2], e3 = v[2] - v[0],
                         e4 = p - v[0];
        // cross product pv & edges
        Rasterizer::Vec3 c1 = e1.cross(e4), c2 = e2.cross(e4),
                         c3 = e3.cross(e4);
        float d1 = c1.dot(c2), d2 = c2.dot(c3), d3 = c3.dot(c1);
        return (d1 >= 0 && d2 >= 0 && d3 >= 0) ||
               (d1 <= 0 && d2 <= 0 && d3 <= 0);
    }
};

}  // namespace Rasterizer

#endif /* TRIANGLE_HPP */
