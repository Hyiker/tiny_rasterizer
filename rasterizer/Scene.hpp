#ifndef SCENE_HPP
#define SCENE_HPP
#include <cmath>
#include <fstream>
#include <functional>
#include <limits>
#include <vector>

#include "rasterizer/Camera.hpp"
#include "rasterizer/Math.hpp"
#include "rasterizer/Shader.hpp"
#include "rasterizer/Texture.hpp"
#include "rasterizer/Triangle.hpp"
namespace Rasterizer {
class Scene {
   private:
    // remove triangles unvisible to camera
    // TODO: remove by vertices
    std::vector<Triangle*> triangleExclusion() { return {}; }
    Mat4 getTransformMatrix() {
        // TODO: implement real model transformation
        Mat4 model_mat = Mat4::identity();
        Mat4 proj_mat = cam.getProjectionMatrix();
        Mat4 view_mat = cam.getViewMatrix();
        return proj_mat * view_mat * model_mat;
    }

   public:
    Camera cam;
    int width;
    int height;
    std::vector<std::vector<RGBColor>> pixels;
    std::vector<std::vector<float>> zbuffer;
    // TODO: add model class
    Texture* texture;
    std::vector<Triangle*> triangles;
    Shader fragment_shader;

    // defaultly, look from 0, 0, 5 along the negative z axis
    Scene(int width, int height, Texture* texture = nullptr,
          Shader fragment_shader = normalFragmentShader)
        : width{width},
          height{height},
          texture{texture},
          fragment_shader{fragment_shader} {
        pixels.resize(height);
        zbuffer.resize(height);
        for (int i = 0; i < height; i++) {
            pixels[i].resize(width);
            zbuffer[i].resize(width);
            std::fill(pixels[i].begin(), pixels[i].end(), RGBColor(0, 0, 0));
            std::fill(zbuffer[i].begin(), zbuffer[i].end(),
                      std::numeric_limits<float>::infinity());
        }
    }
    void render() {
        Mat4 proj_mat = getTransformMatrix();
        Mat4 norm_mat = cam.getViewMatrix().inverse().transpose();
        Mat4 view_mat = cam.getViewMatrix();
        float f1 = (50.f - 0.1f) / 2.0f, f2 = (50.f + 0.1f) / 2.0f;

        std::vector<Triangle> transformed_triangles;
        int tcnt = 0;
        for (const auto triangle : triangles) {
            std::cout << "\rRendering triangle " << tcnt++ << "   ";
            std::array<Vertex, 3>& vertices = triangle->v;
            std::array<Vertex, 3> t_vertices;
            std::array<Vec3, 3> view_pos;
            for (int i = 0; i < 3; i++) {
                Vec4 transformed4(proj_mat * vertices[i].coord.toVec4(1.0f));
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
                view_pos[i] = Vec3(view_mat * vertices[i].coord.toVec4(1.0f));
            }
            // for each triangle, check if pixel is inside
            Triangle t_tri(t_vertices);
            this->draw(t_tri, view_pos);
        }
    }
    void draw(const Triangle& triangle, const std::array<Vec3, 3>& view_pos) {
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
                float xf = float(x) + 0.5, yf = float(y) + 0.5;
                if (triangle.inside(Vec3(xf, yf, 0.0))) {
                    auto [alpha, beta, gamma] =
                        triangle.computeBarycentric2D(xf, yf);
                    float z = triangle.interpolateZ(alpha, beta, gamma);
                    if (z < zbuffer[y][x]) {
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
                        payload.texture = this->texture;
                        payload.texture_coord =
                            triangle.interpolateTextureCoord(alpha, beta,
                                                             gamma);
                        pixels[y][x] = fragment_shader(payload);
                        zbuffer[y][x] = z;
                    }
                }
            }
        }
    }
    void setCamera(const Camera& cam) { this->cam = cam; }
    void addTriangle(Triangle* t) { triangles.push_back(t); }
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
    ~Scene() {
        delete texture;
        for (auto p_tri : triangles) {
            delete p_tri;
            p_tri = nullptr;
        }
    }
};

}  // namespace Rasterizer

#endif /* SCENE_HPP */
