#ifndef CAMERA_HPP
#define CAMERA_HPP
#include "rasterizer/Math.hpp"
namespace Rasterizer {
class Camera {
   public:
    // fov in degrees
    float fov, aspect_ratio;
    Vec3 eye_pos;
    Vec3 up;
    Vec3 center;
    float near, far;
    Camera()
        : Camera(45, 4.0f / 3.0f, Vec3(0, 0, 5), Vec3(0, 0, 0), Vec3(0, 0, 1),
                 0.1, 50) {}
    Camera(float fov, float aspect_ratio, const Vec3& eye_pos, const Vec3& up,
           const Vec3& center, float near, float far)
        : fov{float(DEG2RAD(fov))},
          aspect_ratio{aspect_ratio},
          eye_pos{eye_pos},
          center{center},
          up{up.normalized()},
          near{near},
          far{far} {}
    Mat4 getProjectionMatrix() {
        return Mat4::persp(near, far, fov, aspect_ratio);
    }
    Mat4 getViewMatrix() { return Mat4::lookAt(eye_pos, center, up); }
};

}  // namespace Rasterizer

#endif /* RASTERIZER_HPP */
