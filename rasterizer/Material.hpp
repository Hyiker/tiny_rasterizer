#ifndef MATERIAL_HPP
#define MATERIAL_HPP
#include <string>

#include "rasterizer/Math.hpp"
namespace Rasterizer {
struct Material {
    Material() : Ns{0.0f}, Ni{0.0f}, d{0.0f}, illum{0} {}

    // Material Name
    std::string name;
    // Ambient Color
    Vec3 Ka;
    // Diffuse Color
    Vec3 Kd;
    // Specular Color
    Vec3 Ks;
    // Specular Exponent
    float Ns;
    // Optical Density
    float Ni;
    // Dissolve
    float d;
    // Illumination
    int illum;
    // Ambient Texture Map
    std::string map_Ka;
    // Diffuse Texture Map
    std::string map_Kd;
    // Specular Texture Map
    std::string map_Ks;
    // Specular Hightlight Map
    std::string map_Ns;
    // Alpha Texture Map
    std::string map_d;
    // Bump Map
    std::string map_bump;
};
}  // namespace Rasterizer

#endif /* MATERIAL_HPP */