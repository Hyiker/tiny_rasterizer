#ifndef LIGHT_HPP
#define LIGHT_HPP

#include "rasterizer/Math.hpp"
#include "rasterizer/Texture.hpp"
namespace Rasterizer {
constexpr unsigned int DEPTH_TEXTURE_RESOLUTION = 1024;
constexpr float LIGHT_NEAR = 1e-2;
constexpr float LIGHT_FAR = 30;
constexpr float LIGHT_LRTB = 8.0f;
struct Light {
    Vec3 position;
    Vec3 intensity;
    DepthTexture* dt;
    Vec3 up;
    Vec3 focal;
    Light(const Vec3& pos, const Vec3 intensity, const Vec3& up = Vec3(0, 1, 0),
          const Vec3& focal_point = Vec3(0, 0, 0))
        : position{pos},
          intensity{intensity},
          dt(new DepthTexture(DEPTH_TEXTURE_RESOLUTION,
                              DEPTH_TEXTURE_RESOLUTION)),
          up{up},
          focal{focal_point} {}
    void resetDepthTexture() {
        for (auto& row : dt->tex) {
            fill(row.begin(), row.end(),
                 -std::numeric_limits<float>::infinity());
        }
    }
    Mat4 getCorrectionMatrix() const {
        Mat4 mat;
        mat << 0.5, 0, 0, 0.5, 0, 0.5, 0, 0.5, 0, 0, 0.5, 0.5, 0, 0, 0, 1;
        return mat;
    }
    Mat4 getVP() const {
        return Mat4::ortho(LIGHT_NEAR, LIGHT_FAR, LIGHT_LRTB) *
               Mat4::lookAt(position, focal, up);
    }
    float computeVisibility(const Vec3& world_coord, const Vec3& target) const {
        Vec3 coord = this->getCorrectionMatrix() * this->getVP() *
                     world_coord.toVec4(1.0f);
        if (!dt) {
            return 1.0f;
        }
        // NDC to uv space
        float u = coord.x, v = coord.y;
        if (u > 1.0f || u < 0.f || v > 1.0f || v < 0.f) {
            return 0.0f;
        }
        float nearest_depth = dt->getBilinear(u, v);
        if (coord.z + 5e-3 > nearest_depth) {
            return 1.0f;
        } else {
            return 0.0f;
        }
    }
};
}  // namespace Rasterizer
#endif /* LIGHT_HPP */
