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
    float width, height;
    float near, far;
    Camera()
        : Camera(45, 4.0f / 3.0f, Vec3(0, 0, 5), Vec3(0, 0, 0), Vec3(0, 0, 1),
                 0.1, 50) {}
    Camera(float fov, float aspect_ratio, const Vec3& eye_pos, const Vec3& up,
           const Vec3& center, float near, float far)
        : fov{fov},
          aspect_ratio{aspect_ratio},
          eye_pos{eye_pos},
          center{center},
          up{up.normalized()},
          near{near},
          far{far} {
        float fov_rad = DEG2RAD(fov);
        height = 2.0f * near * std::tan(fov_rad * 0.5f);
        width = height * aspect_ratio;
    }
    Mat4 getProjectionMatrix() {
        Mat4 projection;
        projection << 2 * near / width, 0, 0, 0, 0, 2 * near / height, 0, 0, 0,
            0, (near + far) / (near - far), 2 * far * near / (far - near), 0, 0,
            1, 0;
        return projection;
    }
    Mat4 getViewMatrix() {
        // TODO: implement camera rotation
        Mat4 t_view, r_view;
        Vec3 focal = (center - eye_pos).normalized();
        Vec3 f = -focal, r = focal.cross(up).normalized(), u = f.cross(r);
        t_view << 1, 0, 0, -eye_pos.x, 0, 1, 0, -eye_pos.y, 0, 0, 1, -eye_pos.z,
            0, 0, 0, 1;
        r_view << r.x, r.y, r.z, 0, u.x, u.y, u.z, 0, f.x, f.y, f.z, 0, 0, 0, 0,
            1;
        return std::move(r_view * t_view);
    }
};

}  // namespace Rasterizer

#endif /* RASTERIZER_HPP */
