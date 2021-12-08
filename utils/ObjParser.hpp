#ifndef OBJPARSER_H
#define OBJPARSER_H
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_set>

#include "lib/OBJ_Loader.h"
#include "rasterizer/Material.hpp"
#include "rasterizer/Mesh.hpp"
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
        Rasterizer::Mesh* pm = new Rasterizer::Mesh();
        Rasterizer::Material* mesh_material = new Rasterizer::Material();
        mesh_material->name = mesh.MeshMaterial.name;
        mesh_material->illum = mesh.MeshMaterial.illum;
        mesh_material->Ns = mesh.MeshMaterial.Ns;
        mesh_material->Ka = to_vec3(mesh.MeshMaterial.Ka);
        mesh_material->Ks = to_vec3(mesh.MeshMaterial.Ks);
        mesh_material->Kd = to_vec3(mesh.MeshMaterial.Kd);
        mesh_material->Ni = mesh.MeshMaterial.Ni;
        mesh_material->d = mesh.MeshMaterial.d;
        mesh_material->map_Kd = mesh.MeshMaterial.map_Kd;
        if (mesh.MeshMaterial.map_Kd.length() > 0) {
            std::cout << "loading Kd texture " << mesh_material->map_Kd
                      << std::endl;
            mesh_material->Kd_tex =
                Rasterizer::Texture::loadTGATexture(mesh_material->map_Kd);
        }

        if (mesh.MeshMaterial.map_Ks.length() > 0) {
            std::cout << "loading Ks texture " << mesh_material->map_Ks
                      << std::endl;
            mesh_material->Ks_tex =
                Rasterizer::Texture::loadTGATexture(mesh_material->map_Ks);
        }

        if (mesh.MeshMaterial.map_Ka.length() > 0) {
            std::cout << "loading Ka texture " << mesh_material->map_Ka
                      << std::endl;
            mesh_material->Ka_tex =
                Rasterizer::Texture::loadTGATexture(mesh_material->map_Ka);
        }
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
            pm->triangles.emplace_back(t);
        }
        pm->material = mesh_material;
        model->meshes.emplace_back(pm);
    }
    return model;
}
#endif /* OBJPARSER_H */
