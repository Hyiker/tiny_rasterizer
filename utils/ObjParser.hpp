#ifndef OBJ_PARSER
#define OBJ_PARSER
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_set>

#include "lib/OBJ_Loader.h"
#include "rasterizer/Material.hpp"
#include "rasterizer/Model.hpp"
#include "rasterizer/Triangle.hpp"

static Rasterizer::Vec3 to_vec3(const objl::Vector3& ov3) {
    return Rasterizer::Vec3(ov3.X, ov3.Y, ov3.Z);
}

Rasterizer::Model* parseOBJ(const std::string& filename) {
    Rasterizer::Model* model = new Rasterizer::Model();
    objl::Loader loader;
    bool loadout = loader.LoadFile(filename);
    for (auto mesh : loader.LoadedMeshes) {
        Rasterizer::Material* mesh_material = new Rasterizer::Material();
        mesh_material->name = mesh.MeshMaterial.name;
        mesh_material->illum = mesh.MeshMaterial.illum;
        mesh_material->Ns = mesh.MeshMaterial.Ns;
        mesh_material->Ka = to_vec3(mesh.MeshMaterial.Ka);
        mesh_material->Ks = to_vec3(mesh.MeshMaterial.Ks);
        mesh_material->Kd = to_vec3(mesh.MeshMaterial.Kd);
        for (int i = 0; i < mesh.Indices.size(); i += 3) {
            Rasterizer::Triangle* t = new Rasterizer::Triangle();
            for (int j = 0; j < 3; j++) {
                auto& vertex = mesh.Vertices[mesh.Indices[i + j]];
                t->v[j].coord = Rasterizer::Vec3(
                    vertex.Position.X, vertex.Position.Y, vertex.Position.Z);
                t->v[j].normal = Rasterizer::Vec3(
                    vertex.Normal.X, vertex.Normal.Y, vertex.Normal.Z);
                t->v[j].texture_coord =
                    Rasterizer::Vec3(vertex.TextureCoordinate.X,
                                     vertex.TextureCoordinate.Y, 0.0);
            }
            t->material = mesh_material;
            model->addTriangle(t);
        }
    }
    return model;
}
#endif /* OBJ_PARSER */
