#ifndef TEXTURE_HPP
#define TEXTURE_HPP
#include <iostream>
#include <vector>

#include "lib/lodepng.h"
#include "lib/tga.hpp"
#include "rasterizer/Math.hpp"
namespace Rasterizer {
enum class TextureWrapMode { REPEAT, CLAMP };
struct Texture {
    unsigned int width, height;
    // rgba format
    std::vector<std::vector<RGBAColor>> tex;
    static Texture* loadPNGTexture(const std::string& filename) {
        Texture* texture = new Texture();
        std::vector<unsigned char> raw_pixels;
        unsigned error = lodepng::decode(raw_pixels, texture->width,
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
                    RGBAColor(raw_pixels[p_start], raw_pixels[p_start + 1],
                              raw_pixels[p_start + 2],
                              raw_pixels[p_start + 3]) /
                    255.0f;
            }
        }
        return texture;
    }
    static Texture* loadTGATexture(const std::string& filename) {
        tga::TGA tga_file;
        if (!tga_file.Load(filename)) {
            std::cerr << "can't load tga texture file " << filename
                      << std::endl;
            exit(-1);
        }
        Texture* texture = new Texture();
        texture->width = tga_file.GetWidth();
        texture->height = tga_file.GetHeight();
        texture->tex.resize(texture->height);
        for (auto& row : texture->tex) {
            row.resize(texture->width);
        }
        uint8_t* tga_pixels = tga_file.GetData();
        switch (tga_file.GetFormat()) {
            case tga::ImageFormat::RGBA:
                for (int y = 0; y < texture->height; y++) {
                    for (int x = 0; x < texture->width; x++) {
                        int p_start = y * texture->width * 4 + x * 4;
                        texture->tex[texture->height - y - 1][x] =
                            RGBAColor(tga_pixels[p_start],
                                      tga_pixels[p_start + 1],
                                      tga_pixels[p_start + 2],
                                      tga_pixels[p_start + 3]) /
                            255.0f;
                    }
                }
                break;

            case tga::ImageFormat::RGB:
                for (int y = 0; y < texture->height; y++) {
                    for (int x = 0; x < texture->width; x++) {
                        int p_start = y * texture->width * 3 + x * 3;
                        texture->tex[texture->height - y - 1][x] =
                            RGBAColor(tga_pixels[p_start],
                                      tga_pixels[p_start + 1],
                                      tga_pixels[p_start + 2], 1.0f) /
                            255.0f;
                    }
                }
                break;
            default:
                std::cerr << "unsupported tga format" << std::endl;
                return nullptr;
        }
        return texture;
    }

    RGBColor getRGB(float u, float v,
                    TextureWrapMode wrap_mode = TextureWrapMode::REPEAT) const {
        std::tie(u, v) = wrapByMode(u, v, wrap_mode);
        float u_img = u * (width - 1);
        // v coord needs a flip here
        // https://pic1.zhimg.com/80/d29b9a00c83bebfd0055b70192390ecd_720w.jpg?source=1940ef5c
        float v_img = (1.0f - v) * (height - 1);
        return RGBColor(tex[v_img][u_img]);
    }
    RGBColor getRGBBilinear(
        float u, float v,
        TextureWrapMode wrap_mode = TextureWrapMode::REPEAT) const {
        std::tie(u, v) = wrapByMode(u, v, wrap_mode);
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

   private:
    std::tuple<float, float> wrapByMode(float u, float v,
                                        TextureWrapMode mode) const {
        if (mode == TextureWrapMode::CLAMP) {
            u = math::clamp(u, 0.0f, 1.0f);
            v = math::clamp(v, 0.0f, 1.0f);
        } else if (mode == TextureWrapMode::REPEAT) {
            float _;
            u = std::modff(u, &_);
            v = std::modff(v, &_);
        }
        return std::make_tuple(u, v);
    }
};
}  // namespace Rasterizer
#endif /* TEXTURE_HPP */
