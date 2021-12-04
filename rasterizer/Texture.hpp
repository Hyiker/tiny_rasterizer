#ifndef TEXTURE_HPP
#define TEXTURE_HPP
#include <iostream>
#include <vector>

#include "lib/lodepng.h"
#include "rasterizer/Math.hpp"
namespace Rasterizer {
struct Texture {
    unsigned int width, height;
    std::vector<unsigned char> raw_pixels;
    // rgba format
    std::vector<std::vector<RGBAColor>> tex;
    static Texture* loadPNGTexture(const std::string& filename) {
        Texture* texture = new Texture();
        unsigned error = lodepng::decode(texture->raw_pixels, texture->width,
                                         texture->height, filename.c_str());
        if (error) {
            std::cerr << "can't load png texture file " << filename << ": "
                      << lodepng_error_text(error) << std::endl;
            exit(-1);
        }
        std::cout << "load PNG texture size:" << texture->width << "x"
                  << texture->height << std::endl;
        texture->tex.resize(texture->height);
        for (auto& row : texture->tex) {
            row.resize(texture->width);
        }
        for (int y = 0; y < texture->height; y++) {
            for (int x = 0; x < texture->width; x++) {
                int p_start = y * texture->width * 4 + x * 4;
                texture->tex[y][x] =
                    RGBAColor(texture->raw_pixels[p_start],
                              texture->raw_pixels[p_start + 1],
                              texture->raw_pixels[p_start + 2],
                              texture->raw_pixels[p_start + 3]) /
                    255.0f;
            }
        }
        return texture;
    }
    RGBColor getRGB(float u, float v) const {
        float u_img = u * (width - 1);
        // v coord needs a flip here
        // https://pic1.zhimg.com/80/d29b9a00c83bebfd0055b70192390ecd_720w.jpg?source=1940ef5c
        float v_img = (1.0f - v) * (height - 1);
        return RGBColor(tex[v_img][u_img]);
    }
    RGBColor getRGBBilinear(float u, float v) const {
        float u_img = u * (width - 1);
        float v_img = (1.0f - v) * (height - 1);

        float u_f = std::floor(u_img), u_c = std::ceil(u_img),
              v_f = std::floor(v_img), v_c = std::ceil(v_img);
        // horizontal interpolation
        RGBColor c00 = tex[v_f][u_f], c01 = tex[v_f][u_c], c10 = tex[v_c][u_f],
                 c11 = tex[v_c][u_c];
        return math::lerp(v_img - v_f, math::lerp(u_img - u_f, c00, c01),
                          math::lerp(u_img - u_f, c10, c11));
    }
};
}  // namespace Rasterizer
#endif /* TEXTURE_HPP */
