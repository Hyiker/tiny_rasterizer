#ifndef SHADER_HPP
#define SHADER_HPP
#include <functional>

#include "rasterizer/Light.hpp"
#include "rasterizer/Material.hpp"
#include "rasterizer/Math.hpp"
#include "rasterizer/Texture.hpp"

namespace Rasterizer {
struct FragmentShaderPayload {
    Vec3 normal;
    Vec3 view_position;
    Vec3 screen_position;
    Vec3 texture_coord;
    RGBColor color;
    Material* material;
    Vec3 eye_pos;
    std::vector<Light> lights;
};
using Shader = std::function<RGBColor(FragmentShaderPayload&)>;
RGBColor normalFragmentShader(FragmentShaderPayload& payload);
RGBColor textureFragmentShader(FragmentShaderPayload& payload);
RGBColor blingphongFragmentShader(FragmentShaderPayload& payload);
RGBColor textureFragmentShaderNoLight(FragmentShaderPayload& payload);
}  // namespace Rasterizer
#endif /* SHADER */
