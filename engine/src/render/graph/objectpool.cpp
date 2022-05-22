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

        if (!t.images.empty()) {
            if (t.attributes.textureType == TextureBuffer::TEXTURE_CUBE_MAP) {
                for (int i = TextureBuffer::POSITIVE_X; i <= TextureBuffer::NEGATIVE_Z; i++) {
                    texture->upload(static_cast<TextureBuffer::CubeMapFace>(i), t.images.at(i).get());
                }
            } else {
                texture->upload(t.images.at(0).get());
            }
        }

        return texture;
    }

    ObjectPool::ObjectPool(RenderAllocator &allocator)
            : allocator(allocator) {}

    MeshBuffer &ObjectPool::getMeshBuffer(const ResourceHandle<Mesh> &handle) {
        usedUris.insert(handle.getUri());
        auto it = uriObjects.find(handle.getUri());
        if (it == uriObjects.end()) {
            uriObjects[handle.getUri()] = allocator.createMeshBuffer(handle.get());
        }
        return dynamic_cast<MeshBuffer &>(*uriObjects[handle.getUri()]);
    }

    TextureBuffer &ObjectPool::getTextureBuffer(const ResourceHandle<Texture> &handle) {
        usedUris.insert(handle.getUri());
        auto it = uriObjects.find(handle.getUri());
        if (it == uriObjects.end()) {
            uriObjects[handle.getUri()] = allocateTexture(handle.get(), allocator);
        }
        return dynamic_cast<TextureBuffer &>(*uriObjects[handle.getUri()]);
    }

    ShaderProgram &ObjectPool::getShaderProgram(const ResourceHandle<Shader> &handle) {
        usedUris.insert(handle.getUri());
        auto it = uriObjects.find(handle.getUri());
        if (it == uriObjects.end()) {
            auto &shader = handle.get();
            if (shader.geometryShader.isEmpty())
                uriObjects[handle.getUri()] = allocator.createShaderProgram(shader.vertexShader,
                                                                            shader.fragmentShader);
            else
                uriObjects[handle.getUri()] = allocator.createShaderProgram(shader.vertexShader,
                                                                            shader.fragmentShader,
                                                                            shader.geometryShader);
        }
        return dynamic_cast<ShaderProgram &>(*uriObjects[handle.getUri()]);
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

    TextureBuffer &ObjectPool::getTextureBuffer(TextureBuffer::Attributes attributes) {
        auto index = usedTextures[attributes]++;
        if (textures[attributes].size() <= index) {
            textures[attributes].resize(usedTextures[attributes]);
            textures[attributes].at(index) = allocator.createTextureBuffer(attributes);
        }
        return *textures[attributes].at(index);
    }

    void ObjectPool::endFrame() {
        std::set<Uri> unusedUris;

        for (auto &pair: uriObjects) {
            if (usedUris.find(pair.first) == usedUris.end()) {
                unusedUris.insert(pair.first);
            }
        }

        std::set<std::pair<std::pair<int, int>, int>> unusedTargets;
        for (auto &pair: renderTargets) {
            if (usedTargets.find(pair.first) == usedTargets.end()) {
                unusedTargets.insert(pair.first);
            }
        }

        for (auto &id: unusedUris) {
            uriObjects.erase(id);
        }

        for (auto &pair: unusedTargets) {
            renderTargets.erase(pair);
        }

        usedUris.clear();
        usedTargets.clear();

        std::unordered_set<TextureBuffer::Attributes, TextureAttributesHashFunction> unusedTextures;

        for (auto &pair: textures) {
            if (usedTextures.find(pair.first) == usedTextures.end()) {
                unusedTextures.insert(pair.first);
            } else {
                pair.second.resize(usedTextures.at(pair.first));
            }
        }

        usedTextures.clear();
    }
}