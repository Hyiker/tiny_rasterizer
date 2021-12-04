#ifndef OBJ_PARSER
#define OBJ_PARSER
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_set>

#include "lib/OBJ_Loader.h"
#include "rasterizer/Model.hpp"
#include "rasterizer/Triangle.hpp"

static Rasterizer::Vertex _parseFaceVertex(std::string vstr,
                                           std::vector<Rasterizer::Vec3>& v,
                                           std::vector<Rasterizer::Vec3>& vn,
                                           std::vector<Rasterizer::Vec3>& vt) {
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
        vertex.texture_coord = vt[vinfo_index[1] - 1];
    }
    if (vinfo_index[2] != -1) {
        vertex.normal = vn[vinfo_index[2] - 1];
    }
    return std::move(vertex);
}
// Rasterizer::Model* parseOBJ(const std::string& filename) {
//     Rasterizer::Model* model = new Rasterizer::Model();
//     std::ifstream file(filename);
//     std::vector<Rasterizer::Vec3> vertices;
//     std::vector<Rasterizer::Vec3> texture_coords;
//     std::vector<Rasterizer::Vec3> normals;
//     int n_faces = 0;
//
//     std::unordered_set<std::string> unsup;
//     if (file.is_open()) {
//         std::string line_buffer;
//         while (std::getline(file, line_buffer)) {
//             if (line_buffer.size() == 0 || line_buffer[0] == '#') {
//                 // commment or empty line
//                 continue;
//             }
//             std::stringstream ss(line_buffer);
//             std::string flag;
//             ss >> flag;
//             if (flag == "v") {
//                 Rasterizer::Vec3 v;
//                 ss >> v.x >> v.y >> v.z;
//                 vertices.emplace_back(std::move(v));
//             } else if (flag == "vn") {
//                 Rasterizer::Vec3 vn;
//                 ss >> vn.x >> vn.y >> vn.z;
//                 normals.emplace_back(std::move(vn));
//             } else if (flag == "f") {
//                 std::string vvn;
//                 std::vector<std::string> vstrs;
//                 while (ss.tellg() != -1) {
//                     std::string vstr;
//                     ss >> vstr;
//                     if (vstr == "") {
//                         break;
//                     }
//                     vstrs.emplace_back(std::move(vstr));
//                 }
//                 if (vstrs.size() == 3) {
//                     Rasterizer::Triangle* tri = new Rasterizer::Triangle();
//                     for (int i = 0; i < 3; i++)
//                         tri->v[i] = _parseFaceVertex(vstrs[i], vertices,
//                                                      normals,
//                                                      texture_coords);
//                     model->addTriangle(tri);
//                 } else if (vstrs.size() == 4) {
//                     Rasterizer::Triangle *tri1 = new Rasterizer::Triangle(),
//                                          *tri2 = new Rasterizer::Triangle();
//                     for (int i = 0; i < 3; i++)
//                         tri1->v[i] = _parseFaceVertex(vstrs[i], vertices,
//                                                       normals,
//                                                       texture_coords);
//
//                     for (int i = 0; i < 3; i++)
//                         tri2->v[(i + 2) % 4] =
//                             _parseFaceVertex(vstrs[(i + 2) % 4], vertices,
//                                              normals, texture_coords);
//                     model->addTriangle(tri1);
//                     model->addTriangle(tri2);
//                 } else {
//                     std::cout << "Unsupported polygon with " << vstrs.size()
//                               << " vertices\n";
//                 }
//
//                 n_faces++;
//             } else if (flag == "vt") {
//                 Rasterizer::Vec3 vt;
//                 ss >> vt.x >> vt.y;
//                 texture_coords.emplace_back(std::move(vt));
//             } else {
//                 if (unsup.count(flag) == 0) {
//                     unsup.insert(flag);
//                     std::cout << "ignoring unsupported flag \"" << flag <<
//                     "\""
//                               << std::endl;
//                 }
//                 continue;
//             }
//         }
//         std::cout << "added " << n_faces << " faces" << std::endl;
//     } else {
//         std::cerr << "can't open file " << filename;
//         exit(1);
//     }
//     return model;
// }

Rasterizer::Model* parseOBJ(const std::string& filename) {
    Rasterizer::Model* model = new Rasterizer::Model();
    objl::Loader loader;
    bool loadout = loader.LoadFile(filename);
    for (auto mesh : loader.LoadedMeshes) {
        for (int i = 0; i < mesh.Vertices.size(); i += 3) {
            Rasterizer::Triangle* t = new Rasterizer::Triangle();
            for (int j = 0; j < 3; j++) {
                t->v[j].coord =
                    Rasterizer::Vec3(mesh.Vertices[i + j].Position.X,
                                     mesh.Vertices[i + j].Position.Y,
                                     mesh.Vertices[i + j].Position.Z);
                t->v[j].normal =
                    Rasterizer::Vec3(mesh.Vertices[i + j].Normal.X,
                                     mesh.Vertices[i + j].Normal.Y,
                                     mesh.Vertices[i + j].Normal.Z);
                t->v[j].texture_coord = Rasterizer::Vec3(
                    mesh.Vertices[i + j].TextureCoordinate.X,
                    mesh.Vertices[i + j].TextureCoordinate.Y, 0.0);
            }
            model->addTriangle(t);
        }
    }
    return model;
}
#endif /* OBJ_PARSER */
