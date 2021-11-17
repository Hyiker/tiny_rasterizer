#ifndef SCENE_HPP
#define SCENE_HPP
#include <cmath>
#include <fstream>
#include <vector>

#include "rasterizer/Math.hpp"
#include "rasterizer/Triangle.hpp"
namespace Rasterizer {
template <int width, int height>
class Scene {
   private:
    // remove triangles unvisible to camera
    // TODO: remove by vertices
    std::vector<Triangle*> triangleExclusion() { return {}; }
    Mat4 getProjectionMatrix(float near, float far) {
        Mat4 projection;
        float fov_rad = DEG2RAD(cam.fov);
        float h = 2.0f * near * std::tan(fov_rad * 0.5f);
        float w = h * cam.aspect_ratio;
        projection << 2 * near / w, 0, 0, 0, 0, 2 * near / h, 0, 0, 0, 0,
            (near + far) / (near - far), 2 * far * near / (far - near), 0, 0, 1,
            0;
        return projection;
    }
    Mat4 getViewMatrix() {
        Mat4 view;
        view << 1, 0, 0, -cam.eye_pos.x, 0, 1, 0, -cam.eye_pos.y, 0, 0, 1,
            -cam.eye_pos.z, 0, 0, 0, 1;
        return std::move(view);
    }
    Mat4 getTransformMatrix() {
        Mat4 proj_mat = getProjectionMatrix(0.1, 50);
        Mat4 view_mat = getViewMatrix();
        return proj_mat * view_mat;
    }

   public:
    Camera cam;
    std::array<std::array<Vec3, width>, height> pixels;
    std::vector<Triangle*> triangles;

    // defaultly, look from 0, 0, 5 along the negative z axis
    Scene() : cam(45, 4.0f / 3.0f, Vec3(0, 0, 5)) {}
    void render() {
        // triangles with unvisible triangles excluded
        // auto triangles = triangleExclusion();

        Mat4 proj_mat = getTransformMatrix();
        for (const auto triangle : triangles) {
            std::array<Vec3, 3>& vertices = triangle->v;
            for (int i = 0; i < 3; i++) {
                Vec3 transformed(proj_mat * vertices[i].toVec4());
                std::cout << transformed << '\n';
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
                int x = p255.x, y = p255.y, z = p255.z;
                ofs.write((const char*)&x, 1);
                ofs.write((const char*)&y, 1);
                ofs.write((const char*)&z, 1);
            }
            ofs.write("\n", 1);
        }
        ofs.flush();
        ofs.close();
    }
};

}  // namespace Rasterizer

#endif /* SCENE_HPP */
