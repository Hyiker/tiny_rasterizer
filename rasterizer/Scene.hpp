#ifndef SCENE_HPP
#define SCENE_HPP
#include <cmath>
#include <fstream>
#include <limits>
#include <vector>

#include "rasterizer/Camera.hpp"
#include "rasterizer/Math.hpp"
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
    std::vector<Triangle*> triangles;

    // defaultly, look from 0, 0, 5 along the negative z axis
    Scene(int width, int height) : width{width}, height{height} {
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
        // triangles with unvisible triangles excluded
        // auto triangles = triangleExclusion();

        Mat4 proj_mat = getTransformMatrix();
        std::vector<Triangle> transformed_triangles;
        int i = 1;
        for (const auto triangle : triangles) {
            std::cout << "\rRendering triangle " << i++ << "   ";
            std::array<Vertex, 3>& vertices = triangle->v;
            std::array<Vertex, 3> t_vertices;
            for (int i = 0; i < 3; i++) {
                Vec3 transformed(proj_mat * vertices[i].coord.toVec4());
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
                    (v - 0.5 * height) / height * cam.height, 0.0f);
    }
    std::tuple<float, float> viewCoordToScreenCoord(float x, float y) {
        return std::make_tuple(x / cam.width * width + 0.5 * float(width),
                               y / cam.height * height + 0.5 * float(height));
    }
    void draw(const Triangle& triangle) {
        Vec3 bounding_box[2] = {Vec3(width, height, 0), Vec3(0, 0, 0)};
        for (auto& vertex : triangle.v) {
            auto [u, v] =
                viewCoordToScreenCoord(vertex.coord.x, vertex.coord.y);
            bounding_box[0].x =
                std::max(std::min(bounding_box[0].x, std::floor(u)), 0.0f);
            bounding_box[0].y =
                std::max(std::min(bounding_box[0].y, std::floor(v)), 0.0f);
            bounding_box[1].x =
                std::min(float(width), std::max(bounding_box[1].x, std::ceil(u)));
            bounding_box[1].y =
                std::min(float(height), std::max(bounding_box[1].y, std::ceil(v)));
        }
        for (int y = bounding_box[0].y; y < bounding_box[1].y; y++) {
            for (int x = bounding_box[0].x; x < bounding_box[1].x; x++) {
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
