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

#include "render/graph/framegraphpool.hpp"

namespace xengine {
    static std::unique_ptr<TextureBuffer> allocateTexture(const Texture &t, RenderDevice &device) {
        auto texture = device.createTextureBuffer(t.textureDescription);
        if (!t.images.empty()) {
            if (t.textureDescription.textureType == TEXTURE_CUBE_MAP) {
                for (int i = POSITIVE_X; i <= NEGATIVE_Z; i++) {
                    auto size = t.images.at(i).get().getSize();
                    texture->upload(static_cast<CubeMapFace>(i),
                                    RGBA,
                                    reinterpret_cast<const uint8_t *>(t.images.at(i).get().getData()),
                                    sizeof(ColorRGBA) * (size.x * size.y));
                }
            } else {
                auto size = t.images.at(0).get().getSize();
                texture->upload(RGBA,
                                reinterpret_cast<const uint8_t *>(t.images.at(0).get().getData()),
                                sizeof(ColorRGBA) * (size.x * size.y));
            }
        }

        return texture;
    }

    FrameGraphPool::FrameGraphPool(RenderDevice &device)
            : device(&device) {}

    void FrameGraphPool::collectGarbage() {
        std::set<Uri> unusedUris;
        for (auto &pair: uriObjects) {
            if (usedUris.find(pair.first) == usedUris.end()) {
                unusedUris.insert(pair.first);
            }
        }

        std::unordered_set<TextureBufferDesc> unusedTextures;
        for (auto &pair: textures) {
            if (usedTextures.find(pair.first) == usedTextures.end()) {
                unusedTextures.insert(pair.first);
            } else {
                pair.second.resize(usedTextures.at(pair.first));
            }
        }

        std::unordered_set<RenderTargetDesc> unusedTargets;
        for (auto &pair: targets) {
            if (usedTargets.find(pair.first) == usedTargets.end()) {
                unusedTargets.insert(pair.first);
            } else {
                pair.second.resize(usedTargets.at(pair.first));
            }
        }

        std::unordered_set<PipelinePair, PipelinePairHash> unusedPipelines;
        for (auto &pair: pipelines) {
            if (usedPipelines.find(pair.first) == usedPipelines.end()) {
                unusedPipelines.insert(pair.first);
            }
        }

        for (auto &v: unusedUris) {
            uriObjects.erase(v);
        }

        for (auto &v: unusedTextures) {
            textures.erase(v);
        }

        for (auto &v: unusedTargets) {
            targets.erase(v);
        }

        for (auto &v: unusedPipelines) {
            pipelines.erase(v);
        }

        usedUris.clear();
        usedTextures.clear();
        usedTargets.clear();
        usedPipelines.clear();
    }

    VertexBuffer &FrameGraphPool::getMesh(const ResourceHandle<Mesh>& handle) {
        usedUris.insert(handle.getUri());
        auto it = uriObjects.find(handle.getUri());
        if (it == uriObjects.end()) {
            auto vertexBuffer = device->createInstancedVertexBuffer(handle.get(), {Transform()});
            vertexBuffer->upload(handle.get(), {Transform()});
            uriObjects[handle.getUri()] = std::move(vertexBuffer);
        }
        return dynamic_cast<VertexBuffer &>(*uriObjects.at(handle.getUri()));
    }

    TextureBuffer &FrameGraphPool::getTexture(const ResourceHandle<Texture>& handle) {
        usedUris.insert(handle.getUri());
        auto it = uriObjects.find(handle.getUri());
        if (it == uriObjects.end()) {
            uriObjects[handle.getUri()] = allocateTexture(handle.get(), *device);
        }
        return dynamic_cast<TextureBuffer &>(*uriObjects.at(handle.getUri()));
    }

    ShaderProgram &FrameGraphPool::getShader(const ResourceHandle<Shader>& handle) {
        usedUris.insert(handle.getUri());
        auto it = uriObjects.find(handle.getUri());
        if (it == uriObjects.end()) {
            auto &shader = handle.get();
            ShaderProgramDesc desc;
            desc.shaders.insert(std::pair<ShaderStage, SPIRVShader>(ShaderStage::VERTEX, shader.vertexShader.compile().getShaders().at(0)));
            desc.shaders.insert(std::pair<ShaderStage, SPIRVShader>(ShaderStage::FRAGMENT, shader.fragmentShader.compile().getShaders().at(0)));
            if (!shader.geometryShader.isEmpty()) {
                desc.shaders.insert(std::pair<ShaderStage, SPIRVShader>(ShaderStage::GEOMETRY, shader.geometryShader.compile().getShaders().at(0)));
            }
            uriObjects[handle.getUri()] = device->createShaderProgram(desc);
        }
        return dynamic_cast<ShaderProgram &>(*uriObjects.at(handle.getUri()));
    }

    RenderPipeline &FrameGraphPool::getPipeline(const ResourceHandle<Shader> &shaderRes,
                                                const RenderPipelineDesc &descIn) {
        auto &shader = getShader(shaderRes);
        auto desc = descIn;
        desc.shader = shader;
        auto pair = PipelinePair({.uri = shaderRes.getUri(), .desc = desc});
        usedPipelines.insert(pair);
        auto it = pipelines.find(pair);
        if (it == pipelines.end()) {
            pipelines[pair] = device->createPipeline(desc);
        }
        return dynamic_cast<RenderPipeline &>(*pipelines.at(pair));
    }

    TextureBuffer &FrameGraphPool::createTextureBuffer(const TextureBufferDesc &desc) {
        auto index = usedTextures[desc]++;
        if (textures[desc].size() <= index) {
            textures[desc].resize(usedTextures[desc]);
            textures[desc].at(index) = device->createTextureBuffer(desc);
        }
        return *textures[desc].at(index);
    }

    RenderTarget &FrameGraphPool::createRenderTarget(const RenderTargetDesc &desc) {
        auto index = usedTargets[desc]++;
        if (targets[desc].size() <= index) {
            targets[desc].resize(usedTargets[desc]);
            targets[desc].at(index) = device->createRenderTarget(desc);
        }
        return *targets[desc].at(index);
    }
}