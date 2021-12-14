#ifndef SCENE_H
#define SCENE_H

#if __has_include(<omp.h>)
#include <omp.h>
#define OMP_ENABLE
#endif

#include <cmath>
#include <fstream>
#include <functional>
#include <limits>
#include <vector>

#include "lib/lodepng.h"
#include "rasterizer/Camera.hpp"
#include "rasterizer/Light.hpp"
#include "rasterizer/Math.hpp"
#include "rasterizer/Model.hpp"
#include "rasterizer/Shader.hpp"
#include "rasterizer/Texture.hpp"
#include "rasterizer/Triangle.hpp"
#define SSAA_ENABLE
// #define MSAA_ENABLE

namespace Rasterizer {

#if defined(SSAA_ENABLE) || defined(MSAA_ENABLE)
static constexpr int AA_SAMPLE_RATIO = 2;
static constexpr int AA_SAMPLE_RATIO_SQR = (AA_SAMPLE_RATIO * AA_SAMPLE_RATIO);
static constexpr float AA_SAMPLE_STEP = (1.f / float(AA_SAMPLE_RATIO));
static constexpr float AA_SAMPLE_OFFSET = (0.5f / float(AA_SAMPLE_RATIO));
#endif
class Scene {
   private:
    // if the triangle is backfaced
    bool backfaceCulling(const Triangle& tri) const {
        Vec3 a(tri.v[0].coord), b(tri.v[1].coord), c(tri.v[2].coord);
        float signed_area = (b - a).cross(c - a).z;
        return signed_area >= 0.f;
    }

   public:
    Camera cam;
    int width;
    int height;
    std::vector<std::vector<RGBColor>> pixels;
#if defined(SSAA_ENABLE) || defined(MSAA_ENABLE)
    std::vector<std::vector<std::array<float, AA_SAMPLE_RATIO_SQR>>>
#else
    std::vector<std::vector<float>>
#endif
        zbuffer;
#if defined(SSAA_ENABLE) || defined(MSAA_ENABLE)
    std::vector<std::vector<std::array<RGBColor, AA_SAMPLE_RATIO_SQR>>>
        colorbuffer;
#endif

    std::vector<Model*> models;
    std::vector<Light*> lights;
    Shader fragment_shader;
    Scene() = default;

    // defaultly, look from 0, 0, 5 along the negative z axis
    Scene(int width, int height, Shader fragment_shader = normalFragmentShader)
        : width{width}, height{height}, fragment_shader{fragment_shader} {
        pixels.resize(height);
#if defined(SSAA_ENABLE) || defined(MSAA_ENABLE)
        colorbuffer.resize(height);
#endif
        zbuffer.resize(height);
        for (int i = 0; i < height; i++) {
            pixels[i].resize(width);
            zbuffer[i].resize(width);
            std::fill(pixels[i].begin(), pixels[i].end(), RGBColor(0));
#if defined(SSAA_ENABLE) || defined(MSAA_ENABLE)
            colorbuffer[i].resize(width);
            for (int j = 0; j < width; j++) {
                std::fill(zbuffer[i][j].begin(), zbuffer[i][j].end(),
                          -std::numeric_limits<float>::infinity());
                std::fill(colorbuffer[i][j].begin(), colorbuffer[i][j].end(),
                          RGBColor(0.0f));
            }
#else
            std::fill(zbuffer[i].begin(), zbuffer[i].end(),
                      -std::numeric_limits<float>::infinity());
#endif
        }
    }
    // setup scene, compute light depthmap
    void sceneSetup() { computeDepthTexture(); }
    void render() { rasterizeTriangles(); }
    void computeDepthTexture() {
        for (auto& light : lights) {
            light->resetDepthTexture();
            Mat4 mv = light->getVP();

            for (const auto model : models) {
                Mat4 mvp = mv * model->getModelMatrix();
                Mat4 cmvp = light->getCorrectionMatrix() * mvp;
                for (const auto mesh : model->meshes) {
                    for (const auto triangle : mesh->triangles) {
                        auto vertices = triangle->v;
                        std::array<Vertex, 3> t_vertices;
                        for (int i = 0; i < 3; i++) {
                            t_vertices[i].coord =
                                cmvp * vertices[i].coord.toVec4(1.0f);
                            t_vertices[i].coord.x = DEPTH_TEXTURE_RESOLUTION *
                                                    t_vertices[i].coord.x;
                            t_vertices[i].coord.y =
                                DEPTH_TEXTURE_RESOLUTION *
                                (1.0f - t_vertices[i].coord.y);
                        }

                        // for each triangle, check if pixel is inside
                        Triangle t_tri(t_vertices);
                        Vec3 bbox[2] = {
                            Vec3(std::max(
                                     0.f,
                                     std::min(std::min(t_vertices[0].coord.x,
                                                       t_vertices[1].coord.x),
                                              t_vertices[2].coord.x)),
                                 std::max(
                                     0.f,
                                     std::min(std::min(t_vertices[0].coord.y,
                                                       t_vertices[1].coord.y),
                                              t_vertices[2].coord.y)),
                                 0.0f),
                            Vec3(std::min(
                                     float(DEPTH_TEXTURE_RESOLUTION),
                                     std::max(std::max(t_vertices[0].coord.x,
                                                       t_vertices[1].coord.x),
                                              t_vertices[2].coord.x)),
                                 std::min(
                                     float(DEPTH_TEXTURE_RESOLUTION),
                                     std::max(std::max(t_vertices[0].coord.y,
                                                       t_vertices[1].coord.y),
                                              t_vertices[2].coord.y)),
                                 0.0f),
                        };
                        for (int y = bbox[0].y; y < bbox[1].y; y++) {
                            for (int x = bbox[0].x; x < bbox[1].x; x++) {
                                auto [alpha, beta, gamma] =
                                    t_tri.computeBarycentric2D(float(x) + 0.5f,
                                                               float(y) + 0.5f);
                                float z = 1.f / (alpha / t_tri.v[0].coord.z +
                                                 beta / t_tri.v[1].coord.z +
                                                 gamma / t_tri.v[1].coord.z);
                                light->dt->tex[y][x] =
                                    std::max(z, light->dt->tex[y][x]);
                            }
                        }
                    }
                }
            }
        }
    }
    void rasterizeTriangles() {
        Mat4 proj_mat = cam.getProjectionMatrix();
        Mat4 view_mat = cam.getViewMatrix();
        float f1 = (cam.far - cam.near) / 2.0f,
              f2 = (cam.far + cam.near) / 2.0f;

        std::vector<Triangle> transformed_triangles;
        int tcnt = 0;
        for (const auto& model : models) {
            Mat4 model_mat = model->getModelMatrix();
            Mat4 norm_mat =
                (cam.getViewMatrix() * model_mat).inverse().transpose();
            Mat4 mvp = proj_mat * view_mat * model_mat;
            for (const auto mesh : model->meshes) {
#ifdef OMP_ENABLE
#pragma omp parallel for
#endif
                for (const auto triangle : mesh->triangles) {
                    std::array<Vertex, 3>& vertices = triangle->v;
                    std::array<Vertex, 3> t_vertices;
                    std::array<Vec3, 3> view_pos;
                    std::array<Vec3, 3> world_pos;
                    std::array<float, 3> ws;
                    for (int i = 0; i < 3; i++) {
                        Vec4 transformed4(mvp * vertices[i].coord.toVec4(1.0f));
                        Vec3 transformed(transformed4);
                        transformed = transformed / transformed4.w;
                        if (transformed4.w < 0) {
                            transformed.x = -transformed.x;
                            transformed.y = -transformed.y;
                            ws[i] = transformed4.w;
                        } else if (transformed4.w == 0) {
                            break;
                        } else {
                            ws[i] = -transformed4.w;
                        }

                        t_vertices[i].coord = std::move(transformed);
                        t_vertices[i].coord.x =
                            0.5 * width * (t_vertices[i].coord.x + 1.0);
                        t_vertices[i].coord.y =
                            0.5 * height * (-t_vertices[i].coord.y + 1.0);
                        t_vertices[i].coord.z = t_vertices[i].coord.z * f1 + f2;
                        t_vertices[i].normal =
                            Vec3(norm_mat * vertices[i].normal.toVec4(0.0f))
                                .normalized();
                        t_vertices[i].texture_coord = vertices[i].texture_coord;
                        t_vertices[i].color = RGBColor(148, 121, 92);
                        view_pos[i] = Vec3(view_mat * model_mat *
                                           vertices[i].coord.toVec4(1.0f));
                        world_pos[i] =
                            Vec3(model_mat * vertices[i].coord.toVec4(1.0f));
                    }
                    // for each triangle, check if pixel is inside
                    Triangle t_tri(t_vertices);
                    if (!backfaceCulling(t_tri))
                        this->draw(t_tri, view_pos, world_pos, mesh->material,
                                   ws);
                }
            }
        }
    }
    void draw(const Triangle& triangle, const std::array<Vec3, 3>& view_pos,
              const std::array<Vec3, 3>& world_pos, Material* material,
              const std::array<float, 3>& ws) {
        Vec3 bounding_box[2] = {
            Vec3(std::numeric_limits<float>::infinity(),
                 std::numeric_limits<float>::infinity(), 0),
            Vec3(-std::numeric_limits<float>::infinity(),
                 -std::numeric_limits<float>::infinity(), 0)};
        for (auto& vertex : triangle.v) {
            float u = vertex.coord.x;
            float v = vertex.coord.y;
            bounding_box[0].x =
                std::max(std::min(bounding_box[0].x, std::floor(u)), 0.0f);
            bounding_box[0].y =
                std::max(std::min(bounding_box[0].y, std::floor(v)), 0.0f);
            bounding_box[1].x = std::min(
                float(width), std::max(bounding_box[1].x, std::ceil(u)));
            bounding_box[1].y = std::min(
                float(height), std::max(bounding_box[1].y, std::ceil(v)));
        }
        for (int y = bounding_box[0].y; y < bounding_box[1].y; y++) {
            for (int x = bounding_box[0].x; x < bounding_box[1].x; x++) {
#if defined(SSAA_ENABLE) || defined(MSAA_ENABLE)
                int inside_cnt = 0;
                RGBColor color(0);
                for (int i = 0; i < AA_SAMPLE_RATIO_SQR; i++) {
                    float xf = float(x) + AA_SAMPLE_OFFSET +
                               AA_SAMPLE_STEP * float(i % AA_SAMPLE_RATIO),
                          yf = float(y) + AA_SAMPLE_OFFSET +
                               AA_SAMPLE_STEP * float(i / AA_SAMPLE_RATIO);
#else
                float xf = float(x) + 0.5f, yf = float(y) + 0.5f;
#endif

                    if (triangle.inside(Vec3(xf, yf, 0.0))) {
                        auto [alpha, beta, gamma] =
                            triangle.computeBarycentric2D(xf, yf);
                        float Z = 1.f / (alpha / ws[0] + beta / ws[1] +
                                         gamma / ws[2]);
                        float zp = alpha * triangle.v[0].coord.z / ws[0] +
                                   beta * triangle.v[1].coord.z / ws[1] +
                                   gamma * triangle.v[2].coord.z / ws[2];
                        zp *= Z;
#if defined(SSAA_ENABLE) || defined(MSAA_ENABLE)
                        if (Z > zbuffer[y][x][i]) {
                            inside_cnt++;
#else
                    if (Z > zbuffer[y][x]) {
#endif
                            Vec3 normal =
                                (alpha * triangle.v[0].normal / ws[0] +
                                 beta * triangle.v[1].normal / ws[1] +
                                 gamma * triangle.v[2].normal / ws[2]);
                            normal = (normal * Z).normalized();
                            Vec3 view_position = view_pos[0] * alpha +
                                                 view_pos[1] * beta +
                                                 view_pos[2] * gamma;
                            FragmentShaderPayload payload;
                            payload.normal = normal;
                            payload.view_position = view_position;
                            payload.screen_position = Vec3(xf, yf, 0.0f);
                            payload.texture_coord =
                                (alpha * triangle.v[0].texture_coord / ws[0] +
                                 beta * triangle.v[1].texture_coord / ws[1] +
                                 gamma * triangle.v[2].texture_coord / ws[2]) *
                                Z;
                            payload.color =
                                (alpha * triangle.v[0].color / ws[0] +
                                 beta * triangle.v[1].color / ws[1] +
                                 gamma * triangle.v[2].color / ws[2]) *
                                Z;
                            payload.eye_pos = cam.eye_pos;
                            payload.lights = this->lights;
                            payload.material = material;
                            // visibility from shadow
                            float vis = 0.f;
                            Vec3 frag_world_pos =
                                (alpha * world_pos[0] / ws[0] +
                                 beta * world_pos[1] / ws[1] +
                                 gamma * world_pos[2] / ws[2]) *
                                Z;
                            for (auto light : lights) {
                                vis += light->computeVisibility(frag_world_pos,
                                                                cam.center);
                            }
                            vis = std::min(vis, 1.0f);

#if defined(SSAA_ENABLE) || defined(MSAA_ENABLE)
                            colorbuffer[y][x][i] =
                                fragment_shader(payload) * vis;
                            zbuffer[y][x][i] = Z;
#else
                        pixels[y][x] = fragment_shader(payload) * vis;

                        zbuffer[y][x] = Z;
#endif
                        }
                    }
#if defined(SSAA_ENABLE) || defined(MSAA_ENABLE)
                    color = color + colorbuffer[y][x][i];
                }
                if (inside_cnt != 0) {
                    pixels[y][x] = color / AA_SAMPLE_RATIO_SQR;
                }
#endif
            }
        }
    }
    void setCamera(const Camera& cam) { this->cam = cam; }
    void addModel(Model* model) { this->models.emplace_back(model); }
    void addLight(Light* light) { this->lights.push_back(light); }
    void dumpToPPM(const std::string& path) {
        std::ofstream ofs(path, std::ios::binary);
        ofs.write("P6\n", 3);
        std::string ws = std::to_string(width), hs = std::to_string(height);
        ofs.write(ws.c_str(), ws.length());
        ofs.write(" ", 1);
        ofs.write(hs.c_str(), hs.length());
        ofs.write(" 255\n", 5);
        for (auto& row : pixels) {
            for (auto& pixel : row) {
                Vec3 p255 = pixel * 255.0f;
                char x = p255.x, y = p255.y, z = p255.z;
                ofs.write(&x, 1);
                ofs.write(&y, 1);
                ofs.write(&z, 1);
            }
        }
        ofs.flush();
        ofs.close();
    }
    void dumpToPNG(const std::string& path) {
        std::vector<unsigned char> image;
        image.resize(pixels.size() * pixels[0].size() * 4);
        int i = 0;
        for (auto& row : pixels) {
            for (auto& pixel : row) {
                Vec3 p255 = pixel * 255.0f;
                unsigned char r = p255.x, g = p255.y, b = p255.z, a = 255;
                image[i++] = r;
                image[i++] = g;
                image[i++] = b;
                image[i++] = a;
            }
        }
        unsigned error = lodepng::encode(path, image, width, height);
        if (error) {
            std::cerr << "dump to png failed: " << lodepng_error_text(error);
            exit(1);
        }
    }
    void clear() {
        for (int i = 0; i < height; i++) {
            std::fill(pixels[i].begin(), pixels[i].end(), RGBColor(0));
#if defined(SSAA_ENABLE) || defined(MSAA_ENABLE)
            for (int j = 0; j < width; j++) {
                std::fill(zbuffer[i][j].begin(), zbuffer[i][j].end(),
                          -std::numeric_limits<float>::infinity());
                std::fill(colorbuffer[i][j].begin(), colorbuffer[i][j].end(),
                          RGBColor(0.0f));
            }
#else
            std::fill(zbuffer[i].begin(), zbuffer[i].end(),
                      -std::numeric_limits<float>::infinity());
#endif
        }
    }
};

}  // namespace Rasterizer

#endif /* SCENE_H */
