#ifndef SCENE_H
#define SCENE_H
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

namespace Rasterizer {

static constexpr int SSAA_SAMPLE_RATIO = 2;
static constexpr int SSAA_SAMPLE_RATIO_SQR =
    (SSAA_SAMPLE_RATIO * SSAA_SAMPLE_RATIO);
static constexpr float SSAA_SAMPLE_STEP = (1.f / float(SSAA_SAMPLE_RATIO));
static constexpr float SSAA_SAMPLE_OFFSET = (0.5f / float(SSAA_SAMPLE_RATIO));
class Scene {
   private:
    // remove triangles unvisible to camera
    // TODO: remove by vertices
    std::vector<Triangle*> triangleExclusion() { return {}; }
    Mat4 getTransformMatrix() {
        Mat4 proj_mat = cam.getProjectionMatrix();
        Mat4 view_mat = cam.getViewMatrix();
        return proj_mat * view_mat;
    }

   public:
    Camera cam;
    int width;
    int height;
    std::vector<std::vector<RGBColor>> pixels;
    std::vector<std::vector<std::array<float, SSAA_SAMPLE_RATIO_SQR>>> zbuffer;
    std::vector<std::vector<std::array<RGBColor, SSAA_SAMPLE_RATIO_SQR>>>
        colorbuffer;
    std::vector<Model*> models;
    std::vector<Light> lights;
    Shader fragment_shader;
    Scene() = default;

    // defaultly, look from 0, 0, 5 along the negative z axis
    Scene(int width, int height, Shader fragment_shader = normalFragmentShader)
        : width{width}, height{height}, fragment_shader{fragment_shader} {
        pixels.resize(height);
        colorbuffer.resize(height);
        zbuffer.resize(height);
        for (int i = 0; i < height; i++) {
            pixels[i].resize(width);
            zbuffer[i].resize(width);
            colorbuffer[i].resize(width);
            std::fill(pixels[i].begin(), pixels[i].end(), RGBColor(0, 0, 0));
            for (int j = 0; j < width; j++) {
                std::fill(zbuffer[i][j].begin(), zbuffer[i][j].end(),
                          std::numeric_limits<float>::infinity());
                std::fill(colorbuffer[i][j].begin(), colorbuffer[i][j].end(),
                          RGBColor(0.0f));
            }
        }
    }
    void render() {
        Mat4 proj_mat = getTransformMatrix();
        Mat4 view_mat = cam.getViewMatrix();
        float f1 = (50.f - 0.1f) / 2.0f, f2 = (50.f + 0.1f) / 2.0f;

        std::vector<Triangle> transformed_triangles;
        int tcnt = 0;
        for (const auto& model : models) {
            Mat4 model_matrix = model->getModelMatrix();
            Mat4 norm_mat =
                (cam.getViewMatrix() * model_matrix).inverse().transpose();
            for (const auto mesh : model->meshes) {
                for (const auto triangle : mesh->triangles) {
                    std::array<Vertex, 3>& vertices = triangle->v;
                    std::array<Vertex, 3> t_vertices;
                    std::array<Vec3, 3> view_pos;
                    for (int i = 0; i < 3; i++) {
                        Vec4 transformed4(proj_mat * model_matrix *
                                          vertices[i].coord.toVec4(1.0f));
                        Vec3 transformed(transformed4);
                        transformed = transformed / transformed4.w;

                        t_vertices[i].coord = std::move(transformed);
                        t_vertices[i].coord.x =
                            0.5 * width * (t_vertices[i].coord.x + 1.0);
                        t_vertices[i].coord.y =
                            0.5 * height * (t_vertices[i].coord.y + 1.0);
                        t_vertices[i].coord.z = t_vertices[i].coord.z * f1 + f2;
                        t_vertices[i].normal =
                            Vec3(norm_mat * vertices[i].normal.toVec4(0.0f))
                                .normalized();
                        t_vertices[i].texture_coord = vertices[i].texture_coord;
                        t_vertices[i].color = RGBColor(148, 121, 92);
                        view_pos[i] = Vec3(view_mat * model_matrix *
                                           vertices[i].coord.toVec4(1.0f));
                    }
                    // for each triangle, check if pixel is inside
                    Triangle t_tri(t_vertices);

                    this->draw(t_tri, view_pos, mesh->material);
                }
            }
        }
    }
    void draw(const Triangle& triangle, const std::array<Vec3, 3>& view_pos,
              Material* material) {
        Vec3 bounding_box[2] = {Vec3(width, height, 0), Vec3(0, 0, 0)};
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
                int inside_cnt = 0;
                RGBColor color(0);
                for (int i = 0; i < SSAA_SAMPLE_RATIO_SQR; i++) {
                    float xf = float(x) + SSAA_SAMPLE_OFFSET +
                               SSAA_SAMPLE_STEP * float(i % SSAA_SAMPLE_RATIO),
                          yf = float(y) + SSAA_SAMPLE_OFFSET +
                               SSAA_SAMPLE_STEP * float(i / SSAA_SAMPLE_RATIO);
                    if (triangle.inside(Vec3(xf, yf, 0.0))) {
                        auto [alpha, beta, gamma] =
                            triangle.computeBarycentric2D(xf, yf);
                        float z = triangle.interpolateZ(alpha, beta, gamma);
                        if (z < zbuffer[y][x][i]) {
                            inside_cnt++;
                            Vec3 normal =
                                triangle.interpolateNormal(alpha, beta, gamma)
                                    .normalized();
                            Vec3 view_position = view_pos[0] * alpha +
                                                 view_pos[1] * beta +
                                                 view_pos[2] * gamma;
                            FragmentShaderPayload payload;
                            payload.normal = normal;
                            payload.view_position = view_position;
                            payload.screen_position = Vec3(xf, yf, 0.0f);
                            payload.texture_coord =
                                triangle.interpolateTextureCoord(alpha, beta,
                                                                 gamma);
                            payload.color =
                                triangle.interpolateColor(alpha, beta, gamma);
                            payload.eye_pos = cam.eye_pos;
                            payload.lights = this->lights;
                            payload.material = material;
                            colorbuffer[y][x][i] = fragment_shader(payload);
                            zbuffer[y][x][i] = z;
                        }
                    }
                    color = color + colorbuffer[y][x][i];
                }
                if (inside_cnt != 0) {
                    pixels[y][x] = color / SSAA_SAMPLE_RATIO_SQR;
                }
            }
        }
    }
    void setCamera(const Camera& cam) { this->cam = cam; }
    void addModel(Model* model) { this->models.emplace_back(model); }
    void addLight(const Light& light) { this->lights.push_back(light); }
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
            std::fill(pixels[i].begin(), pixels[i].end(), RGBColor(0, 0, 0));
            for (int j = 0; j < width; j++) {
                std::fill(zbuffer[i][j].begin(), zbuffer[i][j].end(),
                          std::numeric_limits<float>::infinity());
                std::fill(colorbuffer[i][j].begin(), colorbuffer[i][j].end(),
                          RGBColor(0.0f));
            }
        }
    }
};

}  // namespace Rasterizer

#endif /* SCENE_H */
