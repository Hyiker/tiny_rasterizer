#ifndef SCENE_HPP
#define SCENE_HPP
#include <cmath>
#include <fstream>
#include <limits>
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
    Mat4 getTransformMatrix() {
        // TODO: implement real model transformation
        Mat4 model_mat = Mat4::identity();
        Mat4 proj_mat = cam.getProjectionMatrix();
        Mat4 view_mat = cam.getViewMatrix();
        return proj_mat * view_mat * model_mat;
    }

   public:
    Camera cam;
    std::array<std::array<RGBColor, width>, height> pixels;
    std::array<std::array<float, width>, height> zbuffer;
    std::vector<Triangle*> triangles;

    // defaultly, look from 0, 0, 5 along the negative z axis
    Scene() {
        for (int i = 0; i < height; i++) {
            pixels[i].fill(RGBColor(0, 0, 0));
            zbuffer[i].fill(std::numeric_limits<float>::infinity());
        }
    }
    void render() {
        // triangles with unvisible triangles excluded
        // auto triangles = triangleExclusion();

        Mat4 proj_mat = getTransformMatrix();
        std::vector<Triangle> transformed_triangles;
        for (const auto triangle : triangles) {
            std::array<Vertex, 3>& vertices = triangle->v;
            std::array<Vertex, 3> t_vertices;
            for (int i = 0; i < 3; i++) {
                Vec3 transformed(proj_mat * vertices[i].coord.toVec4());
                std::cout << transformed << std::endl;
                t_vertices[i].coord = std::move(transformed);
                t_vertices[i].color = vertices[i].color;
            }
            // for each triangle, check if pixel is inside
            Triangle t_tri(t_vertices);
            this->draw(t_tri);
        }
    }
    Vec3 screenCoordToViewCoord(float u, float v) {
        return Vec3((u - 0.5 * width) / width * cam.width,
                    (v - 0.5 * height) / height * cam.width, 0.0f);
    }
    void draw(const Triangle& triangle) {
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                auto coord =
                    screenCoordToViewCoord(float(x) + 0.5f, float(y) + 0.5f);
                if (triangle.inside(coord)) {
                    auto [u, v, w] =
                        triangle.computeBarycentric2D(coord.x, coord.y);
                    coord.z = triangle.interpolateZ(u, v, w);
                    if (coord.z < zbuffer[y][x]) {
                        pixels[y][x] = triangle.getColor(u, v, w);
                        zbuffer[y][x] = coord.z;
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
};

}  // namespace Rasterizer

#endif /* SCENE_HPP */
