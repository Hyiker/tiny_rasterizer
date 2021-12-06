#ifndef LIGHT_HPP
#define LIGHT_HPP

#include "rasterizer/Math.hpp"
namespace Rasterizer {
struct Light {
    Vec3 position;
    Vec3 intensity;
    Light(const Vec3& pos, const Vec3 intensity)
        : position{pos}, intensity{intensity} {}
};
}  // namespace Rasterizer
#endif /* LIGHT_HPP */
