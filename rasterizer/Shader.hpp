#ifndef SHADER_HPP
#define SHADER_HPP
#include <functional>

#include "rasterizer/Math.hpp"
#include "rasterizer/Texture.hpp"

namespace Rasterizer {
struct FragmentShaderPayload {
    Vec3 normal;
    Vec3 view_position;
    Vec3 screen_position;
    Vec3 texture_coord;
    Texture* texture;
};
RGBColor normalFragmentShader(FragmentShaderPayload& payload) {
    RGBColor color = (payload.normal + Vec3(1.0f)) / 2.f;
    return color;
}
RGBColor textureFragmentShader(FragmentShaderPayload& payload) {
    if (!payload.texture) {
        return RGBColor(0, 0, 0);
    }
    return payload.texture->getRGBBilinear(payload.texture_coord.x,
                                           payload.texture_coord.y);
}
using Shader = std::function<RGBColor(FragmentShaderPayload&)>;
}  // namespace Rasterizer
#endif /* SHADER */
