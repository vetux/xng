/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2021  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "assimpparser.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "asset/mesh.hpp"
#include "asset/material.hpp"

#include "driver/registerdriver.hpp"

namespace xng {
    static const bool dr = REGISTER_DRIVER("assimp", ResourceParser, AssImpParser);

    static Mesh convertMesh(const aiMesh &assMesh) {
        Mesh ret;
        ret.primitive = TRI;
        for (int y = 0; y < assMesh.mNumFaces; y++) {
            const auto &face = dynamic_cast<const aiFace &>(assMesh.mFaces[y]);
            if (face.mNumIndices != 3)
                throw std::runtime_error("Mesh triangulation failed");
            for (int z = 0; z < face.mNumIndices; z++) {
                ret.indices.emplace_back(face.mIndices[z]);
            }
        }

        for (int y = 0; y < assMesh.mNumVertices; y++) {
            const auto &p = dynamic_cast<const aiVector3D &>(assMesh.mVertices[y]);

            Vec3f pos{p.x, p.y, p.z};
            Vec3f norm{};
            Vec2f uv{};
            Vec3f tangent{};
            Vec3f bitangent{};

            if (assMesh.mNormals != nullptr) {
                const auto &n = dynamic_cast<const aiVector3D &>(assMesh.mNormals[y]);
                norm = {n.x, n.y, n.z};
                const auto &t = dynamic_cast<const aiVector3D &>(assMesh.mTangents[y]);
                tangent = {t.x, t.y, t.z};
                const auto &bt = dynamic_cast<const aiVector3D &>(assMesh.mBitangents[y]);
                bitangent = {bt.x, bt.y, bt.z};
            }

            if (assMesh.mTextureCoords[0] != nullptr) {
                const auto &t = dynamic_cast<const aiVector3D &>(assMesh.mTextureCoords[0][y]);
                uv = {t.x, t.y};
            }

            ret.vertices.emplace_back(Vertex(pos, norm, uv, tangent, bitangent));
        }

        return ret;
    }

    static Material convertMaterial(const aiMaterial &assMaterial) {
        Material ret;

        aiColor3D c;
        assMaterial.Get(AI_MATKEY_COLOR_DIFFUSE, c);
        ret.diffuse = {static_cast<uint8_t>(255 * c.r),
                       static_cast<uint8_t>(255 * c.g),
                       static_cast<uint8_t>(255 * c.b),
                       255};

        assMaterial.Get(AI_MATKEY_COLOR_AMBIENT, c);
        ret.ambient = {static_cast<uint8_t>(255 * c.r),
                       static_cast<uint8_t>(255 * c.g),
                       static_cast<uint8_t>(255 * c.b),
                       255};

        assMaterial.Get(AI_MATKEY_COLOR_SPECULAR, c);
        ret.specular = {static_cast<uint8_t>(255 * c.r),
                        static_cast<uint8_t>(255 * c.g),
                        static_cast<uint8_t>(255 * c.b),
                        255};

        assMaterial.Get(AI_MATKEY_SHININESS, ret.shininess);

        return ret;
    }

    static ResourceBundle readAsset(const std::string &assetBuffer, const std::string &hint, Archive *archive) {
        //TODO: Implement assimp IOSystem pointing to archive

        Assimp::Importer importer;

        const auto *scenePointer = importer.ReadFileFromMemory(assetBuffer.data(),
                                                               assetBuffer.size(),
                                                               aiPostProcessSteps::aiProcess_Triangulate |
                                                               aiProcess_CalcTangentSpace | aiProcess_FlipUVs,
                                                               hint.c_str());
        if (scenePointer == nullptr)
            throw std::runtime_error("Failed to read mesh data from memory");

        const auto &scene = dynamic_cast<const aiScene &>(*scenePointer);

        ResourceBundle ret;

        for (auto i = 0; i < scene.mNumMeshes; i++) {
            const auto &mesh = dynamic_cast<const aiMesh &>(*scene.mMeshes[i]);
            std::string name = mesh.mName.C_Str();
            ret.add(name, std::make_unique<Mesh>(convertMesh(mesh)));
        }

        for (auto i = 0; i < scene.mNumMaterials; i++) {
            auto material = convertMaterial(dynamic_cast<const aiMaterial &>(*scene.mMaterials[i]));

            aiString materialName;
            scene.mMaterials[i]->Get(AI_MATKEY_NAME, materialName);

            ret.add(materialName.data, std::make_unique<Material>(material));
        }

        return ret;
    }

    ResourceBundle AssImpParser::read(const std::string &buffer,
                                      const std::string &hint,
                                      const ResourceImporter &importer,
                                      Archive *archive) const {
        return readAsset(buffer, hint, archive);
    }

    const std::set<std::string> &AssImpParser::getSupportedFormats() const {
        static const std::set<std::string> formats = {".fbx", ".dae", ".gltf", ".glb", ".blend", ".3ds", ".ase", ".obj",
                                                      ".ifc", ".xgl", ".zgl", ".ply", ".dxf", ".lwo", ".lws", ".lxo",
                                                      ".stl", ".x", ".ac", ".ms3d", ".cob", ".scn"};
        return formats;
    }
}