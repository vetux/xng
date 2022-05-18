/**
 *  xEngine - C++ game engine library
 *  Copyright (C) 2021  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "render/graph/objectpool.hpp"

namespace xengine {
    static std::unique_ptr<TextureBuffer> allocateTexture(const Texture &t, RenderAllocator &allocator) {
        auto texture = allocator.createTextureBuffer(t.attributes);

        if (t.attributes.textureType == TextureBuffer::TEXTURE_CUBE_MAP) {
            for (int i = TextureBuffer::POSITIVE_X; i <= TextureBuffer::NEGATIVE_Z; i++) {
                texture->upload(static_cast<TextureBuffer::CubeMapFace>(i), t.images.at(i).get());
            }
        } else {
            if (!t.images.empty()) {
                texture->upload(t.images.at(0).get());
            }
        }

        return texture;
    }

    ObjectPool::ObjectPool(RenderAllocator &allocator)
            : allocator(allocator) {}

    MeshBuffer &ObjectPool::getMeshBuffer(const Mesh &mesh) {
        usedIds.insert(mesh.getId());
        auto it = idObjects.find(mesh.getId());
        if (it == idObjects.end()) {
            idObjects[mesh.getId()] = allocator.createMeshBuffer(mesh);
        }
        return dynamic_cast<MeshBuffer &>(*idObjects[mesh.getId()]);
    }

    MeshBuffer &ObjectPool::getInstancedBuffer(const Mesh &mesh, const std::vector<Transform> &offsets) {
        usedInstancedMeshes.insert(mesh.getId());
        auto it = instancedMeshBuffers.find(mesh.getId());
        if (it == instancedMeshBuffers.end()) {
            instancedMeshBuffers[mesh.getId()] = allocator.createInstancedMeshBuffer(mesh, offsets);
        }
        return dynamic_cast<MeshBuffer &>(*instancedMeshBuffers[mesh.getId()]);
    }

    TextureBuffer &ObjectPool::getTextureBuffer(const Texture &texture) {
        usedIds.insert(texture.getId());
        auto it = idObjects.find(texture.getId());
        if (it == idObjects.end()) {
            idObjects[texture.getId()] = allocateTexture(texture, allocator);
        }
        return dynamic_cast<TextureBuffer &>(*idObjects[texture.getId()]);
    }

    ShaderProgram &ObjectPool::getShaderProgram(const Shader &shader) {
        usedIds.insert(shader.getId());
        auto it = idObjects.find(shader.getId());
        if (it == idObjects.end()) {
            if (shader.geometryShader.isEmpty())
                idObjects[shader.getId()] = allocator.createShaderProgram(shader.vertexShader,
                                                                          shader.fragmentShader);
            else
                idObjects[shader.getId()] = allocator.createShaderProgram(shader.vertexShader,
                                                                          shader.fragmentShader,
                                                                          shader.geometryShader);
        }
        return dynamic_cast<ShaderProgram &>(*idObjects[shader.getId()]);
    }

    RenderTarget &ObjectPool::getRenderTarget(Vec2i size, int samples) {
        auto pair = std::pair<std::pair<int, int>, int>({size.x, size.y}, samples);
        usedTargets.insert(pair);
        auto it = renderTargets.find(pair);
        if (it == renderTargets.end()) {
            renderTargets[pair] = allocator.createRenderTarget(size, samples);
        }
        return dynamic_cast<RenderTarget &>(*renderTargets[pair]);
    }

    void ObjectPool::endFrame() {
        std::set<Resource::Id> unusedIds;
        for (auto &pair: idObjects) {
            if (usedIds.find(pair.first) == usedIds.end()) {
                unusedIds.insert(pair.first);
            }
        }

        std::set<Resource::Id> unusedInstancedMeshes;
        for (auto &pair: instancedMeshBuffers) {
            if (usedInstancedMeshes.find(pair.first) == usedInstancedMeshes.end()) {
                unusedInstancedMeshes.insert(pair.first);
            }
        }

        std::set<std::pair<std::pair<int, int>, int>> unusedTargets;
        for (auto &pair: renderTargets) {
            if (usedTargets.find(pair.first) == usedTargets.end()) {
                unusedTargets.insert(pair.first);
            }
        }

        for (auto &id: unusedIds) {
            idObjects.erase(id);
        }
        for (auto &id: unusedInstancedMeshes) {
            instancedMeshBuffers.erase(id);
        }
        for (auto &pair: unusedTargets) {
            renderTargets.erase(pair);
        }

        usedIds.clear();
        usedInstancedMeshes.clear();
        usedTargets.clear();
    }
}