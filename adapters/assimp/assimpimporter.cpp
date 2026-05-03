/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2026 Julia Zampiccoli
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

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "xng/adapters/assimp/assimp.hpp"

#include "xng/assets/assetscene.hpp"
#include "xng/assets/mesh.hpp"
#include "xng/assets/material.hpp"
#include "xng/assets/nodeanimation.hpp"

#include "xng/math/matrixmath.hpp"
#include "xng/math/quaternion.hpp"
#include "xng/math/transform.hpp"

#include "xng/resource/importers/stbiimporter.hpp"

namespace xng::assimp {
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

    static Vec3f convertVector(const aiVector3D &vec) {
        return {vec.x, vec.y, vec.z};
    }

    static Quaternion convertQuaterion(const aiQuaternion &q) {
        return {q.w, q.x, q.y, q.z};
    }

    static Transform convertTransform(const aiMatrix4x4 &mat) {
        aiVector3D scaling;
        aiQuaternion rotation;
        aiVector3D position;
        mat.Decompose(scaling, rotation, position);
        return {
            convertVector(position),
            convertQuaterion(rotation),
            convertVector(scaling)
        };
    }

    static AnimationChannel::Behaviour convertAnimBehaviour(const aiAnimBehaviour &b) {
        switch (b) {
            default:
            case aiAnimBehaviour_DEFAULT:
                return AnimationChannel::DEFAULT;
            case aiAnimBehaviour_CONSTANT:
                return AnimationChannel::CONSTANT;
            case aiAnimBehaviour_LINEAR:
                return AnimationChannel::LINEAR;
            case aiAnimBehaviour_REPEAT:
                return AnimationChannel::REPEAT;
        }
    }

    static AnimationChannel convertChannel(const aiNodeAnim &anim) {
        AnimationChannel ret;
        ret.preState = convertAnimBehaviour(anim.mPreState);
        ret.postState = convertAnimBehaviour(anim.mPostState);
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
        return ret;
    }

    static NodeAnimation convertNodeAnimation(const aiAnimation &animation) {
        NodeAnimation ret;
        ret.duration = animation.mDuration;
        ret.ticksPerSecond = animation.mTicksPerSecond;
        for (auto i = 0; i < animation.mNumChannels; i++) {
            auto &channel = *animation.mChannels[i];
            ret.channels[channel.mNodeName.C_Str()] = convertChannel(channel);
        }
        return ret;
    }

    static Mesh convertMesh(const aiMesh &assMesh) {
        Mesh ret;
        ret.primitive = Mesh::TRIANGLES;
        for (int faceIndex = 0; faceIndex < assMesh.mNumFaces; faceIndex++) {
            const auto &face = assMesh.mFaces[faceIndex];
            if (face.mNumIndices != 3)
                throw std::runtime_error("Mesh triangulation failed");
            for (int z = 0; z < face.mNumIndices; z++) {
                ret.indices.emplace_back(face.mIndices[z]);
            }
        }

        for (auto vertexIndex = 0; vertexIndex < assMesh.mNumVertices; vertexIndex++) {
            const auto &p = assMesh.mVertices[vertexIndex];
            ret.positions.emplace_back(p.x, p.y, p.z);

            if (assMesh.mNormals != nullptr) {
                const auto &n = assMesh.mNormals[vertexIndex];
                ret.normals.emplace_back(n.x, n.y, n.z);
                const auto &t = assMesh.mTangents[vertexIndex];
                ret.tangents.emplace_back(t.x, t.y, t.z);
                const auto &bt = assMesh.mBitangents[vertexIndex];
                ret.bitangents.emplace_back(bt.x, bt.y, bt.z);
            } else {
                ret.normals.emplace_back();
                ret.tangents.emplace_back();
                ret.bitangents.emplace_back();
            }

            if (assMesh.mTextureCoords[0] != nullptr) {
                const auto &t = assMesh.mTextureCoords[0][vertexIndex];
                ret.uvs.emplace_back(t.x, t.y);
            } else {
                ret.uvs.emplace_back();
            }
        }

        for (auto i = 0; i < assMesh.mNumBones; i++) {
            const auto &bone = *assMesh.mBones[i];
            auto &weights = ret.boneWeights[bone.mName.C_Str()];
            for (auto w = 0; w < bone.mNumWeights; w++) {
                Mesh::VertexWeight vw;
                vw.vertex = bone.mWeights[w].mVertexId;
                vw.weight = bone.mWeights[w].mWeight;
                weights.emplace_back(vw);
            }
        }

        return ret;
    }

    static Material convertMaterial(const aiMaterial &assMaterial,
                                    const Uri &fileUri,
                                    const std::map<std::string, aiTexture *> &embeddedTextures) {
        Material ret;

        aiColor3D c;
        assMaterial.Get(AI_MATKEY_COLOR_DIFFUSE, c);
        ret.albedo = {
            static_cast<uint8_t>(255 * c.r),
            static_cast<uint8_t>(255 * c.g),
            static_cast<uint8_t>(255 * c.b),
            0
        };

        float alpha = 0;
        assMaterial.Get(AI_MATKEY_TRANSPARENCYFACTOR, alpha);

        alpha = 1 - alpha;

        ret.albedo.a() = static_cast<uint8_t>(255 * alpha);

        assMaterial.Get(AI_MATKEY_METALLIC_FACTOR, ret.metallic);
        assMaterial.Get(AI_MATKEY_ROUGHNESS_FACTOR, ret.roughness);

        std::string parentPath;
        auto filePath = std::filesystem::path(fileUri.getFile());
        if (filePath.has_parent_path()) {
            parentPath += filePath.parent_path().string() + "/";
        }

        auto texPath = std::make_unique<aiString>();
        auto tex = assMaterial.GetTexture(aiTextureType_DIFFUSE,
                                          0,
                                          texPath.get());

        auto embeddedTexturePath = std::string(texPath->C_Str());

        if (tex == aiReturn::aiReturn_SUCCESS) {
            if (embeddedTextures.find(embeddedTexturePath) != embeddedTextures.end()) {
                ret.albedoTexture = ResourceHandle<ImageRGBA>(
                    Uri(fileUri.getScheme(), fileUri.getFile(), embeddedTexturePath + "_texture"));
            } else {
                ret.albedoTexture = ResourceHandle<ImageRGBA>(Uri(fileUri.getScheme(),
                                                                  parentPath + std::string(texPath->C_Str()),
                                                                  ""));
            }
        }

        tex = assMaterial.GetTexture(aiTextureType_METALNESS,
                                     0,
                                     texPath.get());
        embeddedTexturePath = std::string(texPath->C_Str());

        if (tex == aiReturn::aiReturn_SUCCESS) {
            if (embeddedTextures.find(embeddedTexturePath) != embeddedTextures.end()) {
                ret.metallicTexture = ResourceHandle<ImageRGBA>(
                    Uri(fileUri.getScheme(), fileUri.getFile(), embeddedTexturePath + "_texture"));
            } else {
                ret.metallicTexture = ResourceHandle<ImageRGBA>(Uri(fileUri.getScheme(),
                                                                    parentPath + std::string(texPath->C_Str()),
                                                                    ""));
            }
        }

        tex = assMaterial.GetTexture(aiTextureType_AMBIENT_OCCLUSION,
                                     0,
                                     texPath.get());
        embeddedTexturePath = std::string(texPath->C_Str());

        if (tex == aiReturn::aiReturn_SUCCESS) {
            if (embeddedTextures.find(embeddedTexturePath) != embeddedTextures.end()) {
                ret.ambientOcclusionTexture = ResourceHandle<ImageRGBA>(
                    Uri(fileUri.getScheme(), fileUri.getFile(), embeddedTexturePath + "_texture"));
            } else {
                ret.ambientOcclusionTexture = ResourceHandle<ImageRGBA>(Uri(fileUri.getScheme(),
                                                                            parentPath + std::string(texPath->C_Str()),
                                                                            ""));
            }
        }

        tex = assMaterial.GetTexture(aiTextureType_NORMALS,
                                     0,
                                     texPath.get());
        embeddedTexturePath = std::string(texPath->C_Str());

        if (tex == aiReturn::aiReturn_SUCCESS) {
            if (embeddedTextures.find(embeddedTexturePath) != embeddedTextures.end()) {
                ret.normal = ResourceHandle<ImageRGBA>(Uri(fileUri.getScheme(),
                                                           fileUri.getFile(),
                                                           embeddedTexturePath + "_texture"));
            } else {
                ret.normal = ResourceHandle<ImageRGBA>(Uri(fileUri.getScheme(),
                                                           parentPath + std::string(texPath->C_Str()),
                                                           ""));
            }
        }

        return ret;
    }

    static ImageRGBA convertImage(const aiTexture &texture) {
        if (texture.mHeight == 0) {
            const std::vector buffer(reinterpret_cast<const char *>(texture.pcData),
                               reinterpret_cast<const char *>(texture.pcData) + texture.mWidth);
            return StbiImporter::readImageRGBA(buffer);
        }

        std::vector<ColorRGBA> pixels;
        pixels.reserve(texture.mWidth * texture.mHeight);
        // TODO: Fix Slow Conversion of aiTexture
        for (auto i = 0; i < texture.mWidth * texture.mHeight; i++) {
            auto &texel = texture.pcData[i];
            pixels.emplace_back(texel.r, texel.g, texel.b, texel.a);
        }
        return ImageRGBA(static_cast<int>(texture.mWidth), static_cast<int>(texture.mHeight), pixels);
    }

    static std::map<std::string, Mat4f> collectBoneOffsets(const aiScene &scene) {
        std::map<std::string, Mat4f> ret;
        for (auto i = 0; i < scene.mNumMeshes; i++) {
            const auto &mesh = *scene.mMeshes[i];
            for (auto j = 0; j < mesh.mNumBones; j++) {
                const auto &bone = *mesh.mBones[j];
                ret[bone.mName.C_Str()] = convertMat4(bone.mOffsetMatrix);
            }
        }
        return ret;
    }

    static AssetScene::Node buildNode(const aiNode &node,
                                      const aiScene &scene,
                                      const Uri &path,
                                      const std::map<std::string, Mat4f> &boneOffsets,
                                      const std::vector<ResourceHandle<Material> > &materials) {
        AssetScene::Node ret;
        ret.name = node.mName.C_Str();
        ret.transform = convertTransform(node.mTransformation);

        for (auto i = 0; i < node.mNumMeshes; i++) {
            const auto &assMesh = *scene.mMeshes[node.mMeshes[i]];
            AssetScene::Node::MeshData meshData;
            meshData.mesh = ResourceHandle<Mesh>(
                Uri(path.getScheme(), path.getFile(), assMesh.mName.C_Str()));
            if (assMesh.mMaterialIndex < materials.size()) {
                meshData.material = materials.at(assMesh.mMaterialIndex);
            }
            ret.meshes.emplace_back(std::move(meshData));
        }

        auto it = boneOffsets.find(ret.name);
        if (it != boneOffsets.end()) {
            ret.inverseBind = it->second;
        }

        for (auto i = 0; i < node.mNumChildren; i++) {
            ret.children.emplace_back(buildNode(*node.mChildren[i], scene, path, boneOffsets, materials));
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

        // This assumes the mesh was exported with uv origin at bottom left (Valid for blender and maya)
        const auto *scenePointer = importer.ReadFileFromMemory(assetBuffer.data(),
                                                               assetBuffer.size(),
                                                               aiProcess_Triangulate
                                                               | aiProcess_CalcTangentSpace
                                                               | aiProcess_JoinIdenticalVertices
                                                               | aiProcess_PopulateArmatureData
                                                               | aiProcess_MakeLeftHanded
                                                               | aiProcess_FlipUVs,
                                                               hint.c_str());

        if (scenePointer == nullptr)
            throw std::runtime_error("Failed to read mesh data from memory");

        const auto &scene = *scenePointer;

        ResourceBundle ret;

        std::map<std::string, aiTexture *> textures;
        for (auto i = 0; i < scene.mNumTextures; i++) {
            auto *tex = scene.mTextures[i];
            auto embeddedTexturePath = std::string(tex->mFilename.C_Str());
            textures[embeddedTexturePath] = tex;
        }

        std::vector<ResourceHandle<Material> > materialResourceHandles;
        for (auto i = 0; i < scene.mNumMaterials; i++) {
            auto material = convertMaterial(*scene.mMaterials[i],
                                            path,
                                            textures);

            aiString materialName;
            scene.mMaterials[i]->Get(AI_MATKEY_NAME, materialName);

            ret.add(materialName.data, std::make_unique<Material>(material));

            materialResourceHandles.emplace_back(Uri(path.getScheme(), path.getFile(), materialName.data));
        }

        for (auto i = 0; i < scene.mNumMeshes; i++) {
            const auto &assMesh = *scene.mMeshes[i];
            auto mesh = convertMesh(assMesh);
            ret.add(std::string(assMesh.mName.C_Str()), std::make_unique<Mesh>(mesh));
        }

        auto boneOffsets = collectBoneOffsets(scene);

        std::vector<ResourceHandle<NodeAnimation> > nodeAnimationHandles;
        for (auto i = 0; i < scene.mNumAnimations; i++) {
            auto anim = convertNodeAnimation(*scene.mAnimations[i]);
            std::string animName = scene.mAnimations[i]->mName.C_Str();
            ret.add(animName, std::make_unique<NodeAnimation>(anim));
            nodeAnimationHandles.emplace_back(
                Uri(path.getScheme(), path.getFile(), animName));
        }

        AssetScene assetScene;
        assetScene.root = buildNode(*scene.mRootNode, scene, path, boneOffsets, materialResourceHandles);
        assetScene.nodeAnimations = nodeAnimationHandles;

        std::string sceneName = scene.mName.C_Str();
        if (sceneName.empty()) {
            sceneName = scene.mRootNode->mName.C_Str();
        }
        ret.add(sceneName, std::make_unique<AssetScene>(assetScene));

        for (auto &pair: textures) {
            const auto &atex = *pair.second;
            auto img = convertImage(atex);
            ret.add(std::string(pair.first), std::make_unique<ImageRGBA>(img));
        }

        return ret;
    }

    ResourceBundle ResourceImporter::read(std::istream &stream,
                                          const Uri &path,
                                          Archive *archive) {
        std::vector<char> buffer;

        char c;
        while (!stream.eof()) {
            stream.read(&c, 1);
            if (stream.gcount() == 1) {
                buffer.emplace_back(c);
            }
        }

        return readAsset(buffer, path.getExtension(), path, archive);
    }

    const std::set<std::string> &ResourceImporter::getSupportedFormats() const {
        static const std::set<std::string> formats = {
            ".fbx", ".dae", ".gltf", ".glb", ".blend", ".3ds", ".ase", ".obj",
            ".ifc", ".xgl", ".zgl", ".ply", ".dxf", ".lwo", ".lws", ".lxo",
            ".stl", ".x", ".ac", ".ms3d", ".cob", ".scn"
        };
        return formats;
    }
}
