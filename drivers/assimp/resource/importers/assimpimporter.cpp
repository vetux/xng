/**
 *  xEngine - C++ Game Engine Library
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

#include "xng/driver/assimp/assimpimporter.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "xng/render/mesh.hpp"
#include "xng/render/skinnedmesh.hpp"
#include "xng/render/material.hpp"

#include "xng/render/geometry/vertexbuilder.hpp"

#include "xng/math/matrixmath.hpp"

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
        ret.transform = convertMat4(bone.mNode->mTransformation);
        ret.offset = convertMat4(bone.mOffsetMatrix);
        for (auto i = 0; i < bone.mNumWeights; i++) {
            VertexWeight weight;
            weight.vertex = bone.mWeights[i].mVertexId;
            weight.weight = bone.mWeights[i].mWeight;
            ret.weights.emplace_back(weight);
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
        ret.name = std::string(anim.mNodeName.C_Str());
        return ret;
    }

    static RigAnimation convertAnimation(const aiAnimation &animation) {
        RigAnimation ret;
        ret.name = std::string(animation.mName.C_Str());
        ret.duration = animation.mDuration;
        ret.ticksPerSecond = animation.mTicksPerSecond;
        for (auto i = 0; i < animation.mNumChannels; i++) {
            auto &channel = *animation.mChannels[i];
            ret.channels.emplace_back(convertBoneAnimation(channel));
        }
        return ret;
    }

    static Mesh convertMesh(const aiMesh &assMesh, const std::vector<ResourceHandle<Material>> &materials) {
        Mesh ret;
        ret.primitive = TRIANGLES;
        ret.vertexLayout = SkinnedMesh::getDefaultVertexLayout();
        for (int faceIndex = 0; faceIndex < assMesh.mNumFaces; faceIndex++) {
            const auto &face = dynamic_cast<const aiFace &>(assMesh.mFaces[faceIndex]);
            if (face.mNumIndices != 3)
                throw std::runtime_error("Mesh triangulation failed");
            for (int z = 0; z < face.mNumIndices; z++) {
                ret.indices.emplace_back(face.mIndices[z]);
            }
        }

        std::vector<Bone> bones;
        std::map<size_t, std::set<std::pair<float, size_t>>> boneVertexMapping;
        for (auto i = 0; i < assMesh.mNumBones; i++) {
            auto bone = convertBone(*assMesh.mBones[i]);
            auto boneIndex = bones.size();
            bones.emplace_back(bone);
            for (auto &v: bone.weights) {
                boneVertexMapping[v.vertex].insert(std::make_pair(v.weight, boneIndex));
            }
        }

        for (auto &bone: bones) {
            ret.bones.emplace_back(bone.name);
        }

        for (auto vertexIndex = 0; vertexIndex < assMesh.mNumVertices; vertexIndex++) {
            const auto &p = dynamic_cast<const aiVector3D &>(assMesh.mVertices[vertexIndex]);

            Vec3f pos{p.x, p.y, p.z};
            Vec3f norm{};
            Vec2f uv{};
            Vec3f tangent{};
            Vec3f bitangent{};
            Vec4i boneIds(-1);
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
                        throw std::runtime_error("More than 4 bones per vertex are not supported.");
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

        if (assMesh.mMaterialIndex >= 0) {
            ret.material = materials.at(assMesh.mMaterialIndex);
        }

        return ret;
    }

    static Material convertMaterial(const aiMaterial &assMaterial, const std::string &path) {
        Material ret;

        aiColor3D c;
        assMaterial.Get(AI_MATKEY_COLOR_DIFFUSE, c);
        ret.albedo = {static_cast<uint8_t>(255 * c.r),
                      static_cast<uint8_t>(255 * c.g),
                      static_cast<uint8_t>(255 * c.b),
                      0};

        float alpha = 0;
        assMaterial.Get(AI_MATKEY_TRANSPARENCYFACTOR, alpha);

        alpha = 1 - alpha;

        ret.albedo.a() = static_cast<uint8_t>(255 * alpha);

        ret.transparent = ret.albedo.a() != 255;

        assMaterial.Get(AI_MATKEY_METALLIC_FACTOR, ret.metallic);
        assMaterial.Get(AI_MATKEY_ROUGHNESS_FACTOR, ret.roughness);

        auto texPath = std::make_unique<aiString>();
        auto tex = assMaterial.GetTexture(aiTextureType_DIFFUSE,
                                          0,
                                          texPath.get());

        if (tex == aiReturn::aiReturn_SUCCESS) {
            ret.albedoTexture = ResourceHandle<Texture>(Uri(path + std::string(texPath->C_Str())));
        }

        tex = assMaterial.GetTexture(aiTextureType_METALNESS,
                                     0,
                                     texPath.get());

        if (tex == aiReturn::aiReturn_SUCCESS) {
            ret.metallicTexture = ResourceHandle<Texture>(Uri(path + std::string(texPath->C_Str())));
        }

        tex = assMaterial.GetTexture(aiTextureType_AMBIENT_OCCLUSION,
                                     0,
                                     texPath.get());

        if (tex == aiReturn::aiReturn_SUCCESS) {
            ret.ambientOcclusionTexture = ResourceHandle<Texture>(
                    Uri(path + std::string(texPath->C_Str())));
        }

        tex = assMaterial.GetTexture(aiTextureType_NORMALS,
                                     0,
                                     texPath.get());

        if (tex == aiReturn::aiReturn_SUCCESS) {
            ret.normal = ResourceHandle<Texture>(Uri(path + std::string(texPath->C_Str())));
        }

        return ret;
    }

    static void getChildNodesRecursive(aiNode *node,
                                       std::map<std::string, std::string> &parentMapping,
                                       std::vector<Bone> &bones) {
        Bone bone;
        bone.offset = MatrixMath::identity();
        bone.transform = convertMat4(node->mTransformation);
        bone.name = node->mName.C_Str();
        bones.emplace_back(bone);
        for (auto i = 0; i < node->mNumChildren; i++) {
            auto *child = node->mChildren[i];
            parentMapping[child->mName.C_Str()] = node->mName.C_Str();
            getChildNodesRecursive(child, parentMapping, bones);
        }
    }

    static void getBoneOffsets(Rig &rig, const aiMesh &mesh) {
        for (auto i = 0; i < mesh.mNumBones; i++) {
            auto bone = mesh.mBones[i];
            rig.getBone(bone->mName.C_Str()).offset = convertMat4(bone->mOffsetMatrix);
        }
    }

    static Rig getRig(const aiMesh &mesh, const std::vector<aiMesh *> &subMeshes, const aiScene &scene) {
        std::vector<Bone> bones;
        std::map<std::string, std::string> parentMapping;
        if (mesh.mNumBones > 0) {
            getChildNodesRecursive(scene.mRootNode, parentMapping, bones);
        }
        auto ret = Rig(bones, parentMapping);

        getBoneOffsets(ret, mesh);

        for (auto &m: subMeshes) {
            getBoneOffsets(ret, *m);
        }

        return ret;
    }

    static ResourceBundle readAsset(const std::vector<char> &assetBuffer,
                                    const std::string &hint,
                                    const Uri &path,
                                    Archive *archive) {
        // TODO: Implement assimp IOSystem pointing to archive
        // TODO: Automatically apply scene settings such as coordinate system and unit scale when importing, https://github.com/assimp/assimp/issues/849#issuecomment-875475292

        Assimp::Importer importer;

        const auto *scenePointer = importer.ReadFileFromMemory(assetBuffer.data(),
                                                               assetBuffer.size(),
                                                               aiPostProcessSteps::aiProcess_Triangulate
                                                               | aiProcess_CalcTangentSpace
                                                               | aiProcess_FlipUVs
                                                               | aiProcess_JoinIdenticalVertices
                                                               | aiProcess_PopulateArmatureData,
                                                               hint.c_str());

        if (scenePointer == nullptr)
            throw std::runtime_error("Failed to read mesh data from memory");

        const auto &scene = dynamic_cast<const aiScene &>(*scenePointer);

        ResourceBundle ret;

        std::map<std::string, std::vector<aiMesh *>> meshes;
        for (auto i = 0; i < scene.mNumMeshes; i++) {
            const auto &mesh = *scene.mMeshes[i];
            std::string name = mesh.mName.C_Str();
            meshes[name].emplace_back(scene.mMeshes[i]);
        }

        auto p = std::filesystem::path(path.getFile());

        std::string prefix;
        if (p.has_parent_path()) {
            prefix = p.parent_path().string() + "/";
        }

        std::vector<ResourceHandle<Material>> materialResourceHandles;
        for (auto i = 0; i < scene.mNumMaterials; i++) {
            auto material = convertMaterial(*scene.mMaterials[i], prefix);

            aiString materialName;
            scene.mMaterials[i]->Get(AI_MATKEY_NAME, materialName);

            ret.add(materialName.data, std::make_unique<Material>(material));

            materialResourceHandles.emplace_back(Uri(path.getScheme(), path.getFile(), materialName.data));
        }

        for (auto &pair: meshes) {
            SkinnedMesh mesh;
            aiMesh *meshPtr{};
            std::vector<aiMesh *> subMeshPtrs;
            for (auto &assMesh: pair.second) {
                std::string name = assMesh->mName.C_Str();
                Uri meshUri(path.getScheme(), path.getFile(), name);
                if (mesh.vertexLayout.getSize() == 0) {
                    mesh = convertMesh(*assMesh, materialResourceHandles);
                    meshPtr = assMesh;
                } else {
                    mesh.subMeshes.emplace_back(convertMesh(*assMesh, materialResourceHandles));
                    subMeshPtrs.emplace_back(assMesh);
                }
            }

            if (meshPtr->HasBones()) {
                mesh.rig = getRig(*meshPtr, subMeshPtrs, scene);
            }

            ret.add(pair.first, std::make_unique<SkinnedMesh>(mesh));

            mesh.vertexLayout = Mesh::getDefaultVertexLayout();
            for(auto &v : mesh.vertices){
                v.buffer.resize(mesh.vertexLayout.getSize());
            }

            for (size_t i = 0; i < std::numeric_limits<size_t>::max(); i++) {
                auto name = pair.first + "_mesh_" + std::to_string(i);
                if (!ret.has(name)) {
                    ret.add(name, std::make_unique<Mesh>(mesh));
                    break;
                }
            }
        }

        for (auto i = 0; i < scene.mNumAnimations; i++) {
            auto anim = convertAnimation(*scene.mAnimations[i]);
            ret.add(anim.name, std::make_unique<RigAnimation>(anim));
        }

        return ret;
    }

    ResourceBundle AssImpImporter::read(std::istream &stream,
                                        const std::string &hint,
                                        const std::string &path,
                                        Archive *archive) {
        std::vector<char> buffer;

        char c;
        while (!stream.eof()) {
            stream.read(&c, 1);
            if (stream.gcount() == 1) {
                buffer.emplace_back(c);
            }
        }

        return readAsset(buffer, hint, Uri(path), archive);
    }

    const std::set<std::string> &AssImpImporter::getSupportedFormats() const {
        static const std::set<std::string> formats = {".fbx", ".dae", ".gltf", ".glb", ".blend", ".3ds", ".ase", ".obj",
                                                      ".ifc", ".xgl", ".zgl", ".ply", ".dxf", ".lwo", ".lws", ".lxo",
                                                      ".stl", ".x", ".ac", ".ms3d", ".cob", ".scn"};
        return formats;
    }
}