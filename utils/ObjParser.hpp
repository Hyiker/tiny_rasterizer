#ifndef OBJ_PARSER
#define OBJ_PARSER
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_set>

#include "rasterizer/Scene.hpp"
#include "rasterizer/Triangle.hpp"

static Rasterizer::Vertex _parseFaceVertex(std::string vstr,
                                           std::vector<Rasterizer::Vec3>& v,
                                           std::vector<Rasterizer::Vec3>& vn) {
    Rasterizer::Vertex vertex;
    long long vinfo_index[3]{-1, -1, -1};
    vstr += '/';
    int j = 0;
    long long index = 0;
    for (int i = 0; i < vstr.length(); i++) {
        if (vstr[i] == '/') {
            if (index != 0) {
                vinfo_index[j++] = index;
                index = 0;
            } else {
                j++;
            }
        } else {
            index = index * 10ll + (long long)(vstr[i] - '0');
        }
    }
    if (vinfo_index[0] != -1) {
        vertex.coord = v[vinfo_index[0] - 1];
    }
    if (vinfo_index[1] != -1) {
        // TODO: support texture
    }
    if (vinfo_index[2] != -1) {
        vertex.normal = vn[vinfo_index[2] - 1];
    }
    return std::move(vertex);
}
void parseOBJ(Rasterizer::Scene& scene, const std::string& filename) {
    std::ifstream file(filename);
    std::vector<Rasterizer::Vec3> vertices;
    std::vector<Rasterizer::Vec3> normals;
    int n_faces = 0;

    std::unordered_set<std::string> unsup;
    if (file.is_open()) {
        std::string line_buffer;
        while (std::getline(file, line_buffer)) {
            if (line_buffer.size() == 0 || line_buffer[0] == '#') {
                // commment or empty line
                continue;
            }
            std::stringstream ss(line_buffer);
            std::string flag;
            ss >> flag;
            if (flag == "v") {
                Rasterizer::Vec3 v;
                ss >> v.x >> v.y >> v.z;
                vertices.emplace_back(std::move(v));
            } else if (flag == "vn") {
                Rasterizer::Vec3 vn;
                ss >> vn.x >> vn.y >> vn.z;
                normals.emplace_back(std::move(vn));
            } else if (flag == "f") {
                std::string vvn;
                Rasterizer::Triangle* tri = new Rasterizer::Triangle();
                for (int i = 0; i < 3; i++) {
                    std::string vstr;
                    ss >> vstr;
                    tri->v[i] = _parseFaceVertex(vstr, vertices, normals);
                }
                n_faces++;
                scene.addTriangle(tri);
            } else {
                if (unsup.count(flag) == 0) {
                    unsup.insert(flag);
                    std::cout << "ignoring unsupported flag \"" << flag << "\""
                              << std::endl;
                }
                continue;
            }
        }
        std::cout << "added " << n_faces << " triangle faces" << std::endl;
    } else {
        std::cerr << "can't open file";
        exit(1);
    }
}

#endif /* OBJ_PARSER */
