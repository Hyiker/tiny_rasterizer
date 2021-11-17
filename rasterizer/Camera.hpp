#ifndef CAMERA_HPP
#define CAMERA_HPP
#include "rasterizer/Math.hpp"
namespace Rasterizer {
class Camera {
   public:
    // fov in degrees
    float fov, aspect_ratio;
    Vec3 eye_pos;
    Camera() : fov{45}, aspect_ratio{4.0f / 3.0f}, eye_pos{Vec3(0, 0, 5)} {}
    Camera(float fov, float aspect_ratio, const Vec3& eye_pos)
        : fov{fov}, aspect_ratio{aspect_ratio}, eye_pos{eye_pos} {}
};

}  // namespace Rasterizer

#endif /* RASTERIZER_HPP */
