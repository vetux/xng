/**
 *   xEngine - C++ Game Engine Library
 *   Copyright (C) 2026 Julia Zampiccoli
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the Lesser General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "xng/renderer/renderscene.hpp"

#include "xng/adapters/opengl/opengl.hpp"

namespace xng {
    RenderScene::RenderScene(rg::Runtime &runtime,
                             const size_t chunkSize,
                             const size_t chunkCount,
                             const size_t tileSize,
                             const size_t tileBorder,
                             const float maxAnisotropy)
        : runtime(runtime),
          chunkStreamer(runtime.getResourceHeap(), chunkSize, chunkCount),
          skeletonStreamer(runtime.getResourceHeap(), chunkStreamer),
          meshStreamer(runtime.getResourceHeap(), chunkStreamer),
          virtualTextureStreamer(runtime, chunkStreamer, tileSize, tileBorder, maxAnisotropy),
          unitQuadMesh(createMesh(Mesh::normalizedQuad())),
          unitCubeMesh(createMesh(Mesh::normalizedCube())) {
    }

    RenderObjectHandle<RenderTexture> RenderScene::createTexture(const ImageRGBA &image,
                                                                 const WrappingMethod wrapping,
                                                                 const unsigned int mipLevels) {
        const auto id = allocateID();
        const auto size = image.getResolution();
        const auto maxMip = mipLevels - 1;
        const auto textureHandle = virtualTextureStreamer.create(std::make_shared<ImageTileLoader>(image,
            maxMip + 1,
            virtualTextureStreamer.getTileSize(),
            virtualTextureStreamer.getTileBorder(),
            wrapping,
            runtime.getResourceHeap()));
        textures.emplace(id,
                         RenderTexture(virtualTextureStreamer,
                                       textureHandle,
                                       size,
                                       maxMip));
        types[id] = RenderObject::RENDER_TEXTURE;
        return {this, id, textures.at(id)};
    }

    RenderObjectHandle<RenderTexture> RenderScene::createTexture(const ImageRGBA &image,
                                                                 const WrappingMethod wrapping) {
        const auto id = allocateID();
        const auto size = image.getResolution();
        const auto maxMip = rg::Texture::calculateMipLevels(image.getResolution()) - 1;
        const auto textureHandle = virtualTextureStreamer.create(std::make_shared<ImageTileLoader>(image,
            maxMip + 1,
            virtualTextureStreamer.getTileSize(),
            virtualTextureStreamer.getTileBorder(),
            wrapping,
            runtime.getResourceHeap()));
        textures.emplace(id,
                         RenderTexture(virtualTextureStreamer,
                                       textureHandle,
                                       size,
                                       maxMip));
        types[id] = RenderObject::RENDER_TEXTURE;
        return {this, id, textures.at(id)};
    }

    RenderObjectHandle<RenderTexture> RenderScene::createTexture(const std::shared_ptr<TileLoader> &tileLoader) {
        const auto id = allocateID();
        const auto size = tileLoader->getSize();
        const auto maxMip = tileLoader->getMipLevels() - 1;
        const auto textureHandle = virtualTextureStreamer.create(tileLoader);
        textures.emplace(id,
                         RenderTexture(virtualTextureStreamer,
                                       textureHandle,
                                       size,
                                       maxMip));
        types[id] = RenderObject::RENDER_TEXTURE;
        return {this, id, textures.at(id)};
    }

    RenderObjectHandle<RenderSkeleton> RenderScene::createSkeleton(const std::vector<std::string> &boneNames) {
        const auto id = allocateID();
        auto skeletonHandle = skeletonStreamer.create(boneNames.size());
        std::unordered_map<std::string, unsigned int> boneOffsets;
        for (size_t i = 0; i < boneNames.size(); i++) {
            boneOffsets[boneNames.at(i)] = static_cast<unsigned int>(i);
        }
        skeletons.emplace(id, RenderSkeleton(skeletonStreamer, skeletonHandle, boneOffsets));
        types[id] = RenderObject::RENDER_SKELETON;
        return {this, id, skeletons.at(id)};
    }


    RenderObjectHandle<RenderMesh> RenderScene::createMesh(const Mesh &mesh) {
        const auto id = allocateID();
        const auto meshHandle = meshStreamer.create(mesh, {});
        meshes.emplace(id, RenderMesh(meshStreamer, meshHandle, {}));
        types[id] = RenderObject::RENDER_MESH;
        return {this, id, meshes.at(id)};
    }

    RenderObjectHandle<RenderMesh> RenderScene::createMesh(const Mesh &mesh,
                                                           RenderObjectHandle<RenderSkeleton> skeleton) {
        const auto id = allocateID();
        const auto meshHandle = meshStreamer.create(mesh, skeleton.get().getOffsets());
        meshes.emplace(id, RenderMesh(meshStreamer, meshHandle, skeleton));
        types[id] = RenderObject::RENDER_MESH;
        return {this, id, meshes.at(id)};
    }

    void RenderScene::incrementReference(const RenderObject::ID id) {
        refCounts.at(id)++;
    }

    void RenderScene::decrementReference(const RenderObject::ID id) {
        refCounts.at(id)--;
        if (refCounts.at(id) <= 0) {
            switch (types.at(id)) {
                case RenderObject::RENDER_TEXTURE:
                    destroyTexture(id);
                    break;
                case RenderObject::RENDER_SKELETON:
                    destroySkeleton(id);
                    break;
                case RenderObject::RENDER_MESH:
                    destroyMesh(id);
                    break;
                case RenderObject::RENDER_MODEL:
                    destroyModel(id);
                    break;
                case RenderObject::RENDER_LIGHT_POINT:
                    destroyPointLight(id);
                    break;
                case RenderObject::RENDER_LIGHT_DIRECTIONAL:
                    destroyDirectionalLight(id);
                    break;
                case RenderObject::RENDER_LIGHT_SPOT:
                    destroySpotLight(id);
                    break;
                case RenderObject::RENDER_CANVAS:
                    destroyCanvas(id);
                    break;
                case RenderObject::RENDER_PAINT:
                    destroyPaint(id);
                    break;
            }
            types.erase(id);
            refCounts.erase(id);
            freeID(id);
        }
    }

    void RenderScene::destroyTexture(const RenderObject::ID id) {
        virtualTextureStreamer.destroy(textures.at(id).getHandle());
        textures.erase(id);
    }

    void RenderScene::destroySkeleton(const RenderObject::ID id) {
        skeletonStreamer.destroy(skeletons.at(id).getBaseBone());
        skeletons.erase(id);
    }

    void RenderScene::destroyMesh(const RenderObject::ID id) {
        meshStreamer.destroy(meshes.at(id).getHandle());
        meshes.erase(id);
    }
}
