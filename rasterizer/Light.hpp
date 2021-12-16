#ifndef LIGHT_HPP
#define LIGHT_HPP

#include "rasterizer/Math.hpp"
#include "rasterizer/Texture.hpp"

namespace Rasterizer {
constexpr unsigned int DEPTH_TEXTURE_RESOLUTION = 1024 * 10;
constexpr float LIGHT_NEAR = 1e-2;
constexpr float LIGHT_FAR = 50;
constexpr float LIGHT_LRTB = 8.0f;
static constexpr uint32_t NUM_SAMPLES = 16;
static constexpr float INV_NUM_SAMPLES = 1.f / float(NUM_SAMPLES);
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
    float plainShadowMap(const Vec3& depth_coord) const {
        if (!dt) {
            return 1.0f;
        }
        // NDC to uv space
        float u = depth_coord.x, v = depth_coord.y;
        if (u > 1.0f || u < 0.f || v > 1.0f || v < 0.f) {
            return 0.0f;
        }
        float nearest_depth = dt->getBilinear(u, v);
        if (depth_coord.z + EPSILON > nearest_depth) {
            return 1.0f;
        } else {
            return 0.0f;
        }
    }

    float PCFShadowMap(const Vec3& depth_coord) const {
        std::array<Vec3, NUM_SAMPLES> poisson_disk;
        math::poissonDiskSamples<NUM_SAMPLES, 10>(poisson_disk, depth_coord);
        float filter_scale = 0.00125f;
        float visibility = 0.0f;
        for (int i = 0; i < NUM_SAMPLES; i++) {
            float u = depth_coord.x + poisson_disk[i].x * filter_scale,
                  v = depth_coord.y + poisson_disk[i].y * filter_scale;
            if (u > 1.0f || u < 0.f || v > 1.0f || v < 0.f) {
                visibility += 0.0f;
                continue;
            }
            float nearest_depth = dt->getBilinear(u, v);
            if (depth_coord.z + EPSILON > nearest_depth) {
                visibility += 1.f;
            }
        }
        return visibility * INV_NUM_SAMPLES;
    }
    float computeVisibility(const Vec3& world_coord) const {
        Vec3 coord = this->getCorrectionMatrix() * this->getVP() *
                     world_coord.toVec4(1.0f);
        return PCFShadowMap(coord);
    }
};
}  // namespace Rasterizer
#endif /* LIGHT_HPP */
