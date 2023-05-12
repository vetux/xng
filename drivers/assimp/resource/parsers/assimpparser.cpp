/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2023  Julian Zampiccoli
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

#include "xng/driver/assimp/assimpparser.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "xng/asset/mesh.hpp"
#include "xng/asset/material.hpp"

#include "xng/geometry/vertexbuilder.hpp"

namespace xng {
    static Mat4f convertMat4(const aiMatrix4x4 &mat) {
        Mat4f ret;
        ret.set(0, 0, mat.a1);
        ret.set(1, 0, mat.a2);
        ret.set(2, 0, mat.a3);
        ret.set(3, 0, mat.a4);
        ret.set(0, 1, mat.b1);
        ret.set(1, 1, mat.b2);
        ret.set(2, 1, mat.b3);
        ret.set(3, 1, mat.b4);
        ret.set(0, 2, mat.c1);
        ret.set(1, 2, mat.c2);
        ret.set(2, 2, mat.c3);
        ret.set(3, 2, mat.c4);
        ret.set(0, 3, mat.d1);
        ret.set(1, 3, mat.d2);
        ret.set(2, 3, mat.d3);
        ret.set(3, 3, mat.d4);
        return ret;
    }

    static Bone convertBone(const aiBone &bone) {
        Bone ret;
        ret.name = std::string(bone.mName.data, bone.mName.length);
        ret.offset = convertMat4(bone.mOffsetMatrix);
        for (auto i = 0; i < bone.mNumWeights; i++) {
            ret.weights.emplace_back(
                    VertexWeight{.vertex = bone.mWeights[i].mVertexId, .weight = bone.mWeights[i].mWeight});
        }
        return ret;
    }

    static Vec3f convertVector(const aiVector3D &vec) {
        return {vec.x, vec.y, vec.z};
    }

    static Quaternion convertQuaterion(const aiQuaternion &q) {
        return {q.w, q.x, q.y, q.z};
    }

    static BoneAnimation::Behaviour convertAnimBehaviour(const aiAnimBehaviour &b) {
        switch (b) {
            default:
            case aiAnimBehaviour_DEFAULT:
                return BoneAnimation::DEFAULT;
            case aiAnimBehaviour_CONSTANT:
                return BoneAnimation::CONSTANT;
            case aiAnimBehaviour_LINEAR:
                return BoneAnimation::LINEAR;
            case aiAnimBehaviour_REPEAT:
                return BoneAnimation::REPEAT;
        }
    }

    static BoneAnimation convertBoneAnimation(const aiNodeAnim &anim) {
        BoneAnimation ret;
        for (auto i = 0; i < anim.mNumPositionKeys; i++) {
            auto k = anim.mPositionKeys[i];
            ret.positionFrames[k.mTime] = convertVector(k.mValue);
        }
        for (auto i = 0; i < anim.mNumRotationKeys; i++) {
            auto k = anim.mRotationKeys[i];
            ret.rotationFrames[k.mTime] = convertQuaterion(k.mValue);
        }
        for (auto i = 0; i < anim.mNumScalingKeys; i++) {
            auto k = anim.mScalingKeys[i];
            ret.scaleFrames[k.mTime] = convertVector(k.mValue);
        }
        ret.preState = convertAnimBehaviour(anim.mPreState);
        ret.postState = convertAnimBehaviour(anim.mPostState);
        ret.name = std::string(anim.mNodeName.data, anim.mNodeName.length);
        return ret;
    }

    static RigAnimation convertAnimation(const aiAnimation &animation) {
        RigAnimation ret;
        ret.name = std::string(animation.mName.data, animation.mName.length);
        ret.duration = animation.mDuration;
        ret.ticksPerSecond = animation.mTicksPerSecond;
        for (auto i = 0; i < animation.mNumChannels; i++) {
            ret.channels.emplace_back(convertBoneAnimation(*animation.mChannels[i]));
        }
        return ret;
    }

    static Mesh convertMesh(const aiMesh &assMesh) {
        Mesh ret;
        ret.primitive = TRIANGLES;
        ret.vertexLayout = Mesh::getDefaultVertexLayout();
        for (int faceIndex = 0; faceIndex < assMesh.mNumFaces; faceIndex++) {
            const auto &face = dynamic_cast<const aiFace &>(assMesh.mFaces[faceIndex]);
            if (face.mNumIndices != 3)
                throw std::runtime_error("Mesh triangulation failed");
            for (int z = 0; z < face.mNumIndices; z++) {
                ret.indices.emplace_back(face.mIndices[z]);
            }
        }

        std::vector<Bone> bones;
        bones.reserve(assMesh.mNumBones);
        std::map<size_t, std::set<std::pair<float, size_t>>> boneVertexMapping;
        for (auto i = 0; i < assMesh.mNumBones; i++) {
            bones.emplace_back(convertBone(*assMesh.mBones[i]));
            auto boneIndex = bones.size() - 1;
            auto bone = bones.end() - 1;
            for (auto &v: bone->weights) {
                boneVertexMapping[v.vertex].insert(std::make_pair(v.weight, boneIndex));
            }
        }

        ret.rig = Rig(bones);

        for (auto vertexIndex = 0; vertexIndex < assMesh.mNumVertices; vertexIndex++) {
            const auto &p = dynamic_cast<const aiVector3D &>(assMesh.mVertices[vertexIndex]);

            Vec3f pos{p.x, p.y, p.z};
            Vec3f norm{};
            Vec2f uv{};
            Vec3f tangent{};
            Vec3f bitangent{};
            Vec4i boneIds{};
            Vec4f boneWeights{};

            if (assMesh.mNormals != nullptr) {
                const auto &n = dynamic_cast<const aiVector3D &>(assMesh.mNormals[vertexIndex]);
                norm = {n.x, n.y, n.z};
                const auto &t = dynamic_cast<const aiVector3D &>(assMesh.mTangents[vertexIndex]);
                tangent = {t.x, t.y, t.z};
                const auto &bt = dynamic_cast<const aiVector3D &>(assMesh.mBitangents[vertexIndex]);
                bitangent = {bt.x, bt.y, bt.z};
            }

            if (assMesh.mTextureCoords[0] != nullptr) {
                const auto &t = dynamic_cast<const aiVector3D &>(assMesh.mTextureCoords[0][vertexIndex]);
                uv = {t.x, t.y};
            }

            auto it = boneVertexMapping.find(vertexIndex);
            if (it != boneVertexMapping.end()) {
                int boneId = 0;
                for (auto &pair: it->second) {
                    if (boneId > 3) {
                        throw std::runtime_error("More than 4 bones per vertex are not supported");
                    }
                    switch (boneId) {
                        case 0:
                            boneIds.x = static_cast<int>(pair.second);
                            boneWeights.x = pair.first;
                            break;
                        case 1:
                            boneIds.y = static_cast<int>(pair.second);
                            boneWeights.y = pair.first;
                            break;
                        case 2:
                            boneIds.z = static_cast<int>(pair.second);
                            boneWeights.z = pair.first;
                            break;
                        case 3:
                            boneIds.w = static_cast<int>(pair.second);
                            boneWeights.w = pair.first;
                            break;
                        default:
                            break;
                    }
                    boneId++;
                }
            }
            
            ret.vertices.emplace_back(VertexBuilder()
                                              .addVec3(pos)
                                              .addVec3(norm)
                                              .addVec2(uv)
                                              .addVec3(tangent)
                                              .addVec3(bitangent)
                                              .addVec4(boneIds)
                                              .addVec4(boneWeights)
                                              .build());
        }

        return ret;
    }

    static Material convertMaterial(const aiMaterial &assMaterial) {
        Material ret;

        aiColor3D c;
        assMaterial.Get(AI_MATKEY_COLOR_DIFFUSE, c);
        ret.albedo = {static_cast<uint8_t>(255 * c.r),
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

        ret.roughness = 128.0f - (float) assMaterial.Get(AI_MATKEY_SHININESS, c);

        return ret;
    }

    static ResourceBundle readAsset(const std::vector<char> &assetBuffer, const std::string &hint, Archive *archive) {
        //TODO: Implement assimp IOSystem pointing to archive

        Assimp::Importer importer;

        const auto *scenePointer = importer.ReadFileFromMemory(assetBuffer.data(),
                                                               assetBuffer.size(),
                                                               aiPostProcessSteps::aiProcess_Triangulate
                                                               | aiProcess_CalcTangentSpace
                                                               | aiProcess_FlipUVs
                                                               | aiProcess_JoinIdenticalVertices,
                                                               hint.c_str());

        if (scenePointer == nullptr)
            throw std::runtime_error("Failed to read mesh data from memory");

        const auto &scene = dynamic_cast<const aiScene &>(*scenePointer);

        ResourceBundle ret;

        for (auto i = 0; i < scene.mNumMeshes; i++) {
            const auto &mesh = *scene.mMeshes[i];
            std::string name = mesh.mName.C_Str();
            ret.add(name, std::make_unique<Mesh>(convertMesh(mesh)));
        }

        for (auto i = 0; i < scene.mNumAnimations; i++) {
            auto anim = convertAnimation(*scene.mAnimations[i]);
            ret.add(anim.name, std::make_unique<RigAnimation>(anim));
        }

        for (auto i = 0; i < scene.mNumMaterials; i++) {
            auto material = convertMaterial(*scene.mMaterials[i]);

            aiString materialName;
            scene.mMaterials[i]->Get(AI_MATKEY_NAME, materialName);

            ret.add(materialName.data, std::make_unique<Material>(material));
        }

        return ret;
    }

    ResourceBundle
    AssImpParser::read(const std::vector<char> &buffer, const std::string &hint, Archive *archive) const {
        return readAsset(buffer, hint, archive);
    }

    const std::set<std::string> &AssImpParser::getSupportedFormats() const {
        static const std::set<std::string> formats = {".fbx", ".dae", ".gltf", ".glb", ".blend", ".3ds", ".ase", ".obj",
                                                      ".ifc", ".xgl", ".zgl", ".ply", ".dxf", ".lwo", ".lws", ".lxo",
                                                      ".stl", ".x", ".ac", ".ms3d", ".cob", ".scn"};
        return formats;
    }
}