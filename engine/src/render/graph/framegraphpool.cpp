/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2022  Julian Zampiccoli
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

#include "render/graph/framegraphpool.hpp"
#include "render/graph/shader/framegraphshader.hpp"

namespace xng {
    static std::unique_ptr<TextureBuffer> allocateTexture(const Texture &t, RenderDevice &device) {
        auto texture = device.createTextureBuffer(t.description);
        if (!t.images.empty()) {
            if (t.description.textureType == TEXTURE_CUBE_MAP) {
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

    FrameGraphPool::FrameGraphPool(RenderDevice &device, SPIRVCompiler &spirvCompiler, SPIRVDecompiler &spirvDecompiler)
            : device(&device), shaderCompiler(&spirvCompiler), shaderDecompiler(&spirvDecompiler) {}

    void FrameGraphPool::collectGarbage() {
        std::set<Uri> unusedUris;
        for (auto &pair: uriObjects) {
            if (usedUris.find(pair.first) == usedUris.end()) {
                unusedUris.insert(pair.first);
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

        for (auto &v: unusedPipelines) {
            pipelines.erase(v);
        }

        usedUris.clear();
        usedTextures.clear();
        usedTargets.clear();
        usedPipelines.clear();

        textures.clear();
        shaderBuffers.clear();
        targets.clear();
    }

    VertexBuffer &FrameGraphPool::getMesh(const ResourceHandle<Mesh> &handle) {
        usedUris.insert(handle.getUri());
        auto it = uriObjects.find(handle.getUri());
        if (it == uriObjects.end()) {
            uriObjects[handle.getUri()] = device->createInstancedVertexBuffer(handle.get(), {MatrixMath::identity()});
        }
        return dynamic_cast<VertexBuffer &>(*uriObjects.at(handle.getUri()));
    }

    TextureBuffer &FrameGraphPool::getTexture(const ResourceHandle<Texture> &handle) {
        usedUris.insert(handle.getUri());
        auto it = uriObjects.find(handle.getUri());
        if (it == uriObjects.end()) {
            uriObjects[handle.getUri()] = allocateTexture(handle.get(), *device);
        }
        return dynamic_cast<TextureBuffer &>(*uriObjects.at(handle.getUri()));
    }

    ShaderProgram &FrameGraphPool::getShader(const ResourceHandle<Shader> &handle) {
        usedUris.insert(handle.getUri());
        auto it = uriObjects.find(handle.getUri());
        if (it == uriObjects.end()) {
            auto &shader = handle.get();
            ShaderProgramDesc desc;
            shader.vertexShader.preprocess(*shaderCompiler, FrameGraphShader::getShaderInclude());
            shader.fragmentShader.preprocess(*shaderCompiler, FrameGraphShader::getShaderInclude());
            shader.geometryShader.preprocess(*shaderCompiler, FrameGraphShader::getShaderInclude());
            desc.shaders.insert(std::pair<ShaderStage, SPIRVShader>(ShaderStage::VERTEX,
                                                                    shader.vertexShader.compile(
                                                                            *shaderCompiler).getShaders().at(0)));
            desc.shaders.insert(std::pair<ShaderStage, SPIRVShader>(ShaderStage::FRAGMENT,
                                                                    shader.fragmentShader.compile(
                                                                            *shaderCompiler).getShaders().at(
                                                                            0)));
            if (!shader.geometryShader.isEmpty()) {
                desc.shaders.insert(std::pair<ShaderStage, SPIRVShader>(ShaderStage::GEOMETRY,
                                                                        shader.geometryShader.compile(
                                                                                *shaderCompiler).getShaders().at(
                                                                                0)));
            }
            uriObjects[handle.getUri()] = device->createShaderProgram(*shaderDecompiler, desc);
        }
        return dynamic_cast<ShaderProgram &>(*uriObjects.at(handle.getUri()));
    }

    RenderPipeline &FrameGraphPool::getPipeline(const ResourceHandle<Shader> &shader, const RenderPipelineDesc &desc) {
        auto pair = PipelinePair(shader.getUri(), desc);
        usedPipelines.insert(pair);
        usedUris.insert(shader.getUri());
        auto it = pipelines.find(pair);
        if (it == pipelines.end()) {
            pipelines[pair] = device->createRenderPipeline(desc, getShader(shader));
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

    void FrameGraphPool::destroyTextureBuffer(TextureBuffer &buffer) {
        usedTextures[buffer.getDescription()]--;
        bool found = false;
        long index = 0;
        for (auto i = 0; i < textures[buffer.getDescription()].size(); i++) {
            if (textures[buffer.getDescription()][i].get() == &buffer) {
                index = i;
                found = true;
                break;
            }
        }
        assert(found);
        textures[buffer.getDescription()].erase(textures[buffer.getDescription()].begin() + index);
    }

    ShaderBuffer &FrameGraphPool::createShaderBuffer(const ShaderBufferDesc &desc) {
        auto index = usedShaderBuffers[desc]++;
        if (shaderBuffers[desc].size() <= index) {
            shaderBuffers[desc].resize(usedShaderBuffers[desc]);
            shaderBuffers[desc].at(index) = device->createShaderBuffer(desc);
        }
        return *shaderBuffers[desc].at(index);
    }

    void FrameGraphPool::destroyShaderBuffer(ShaderBuffer &buffer) {
        usedShaderBuffers[buffer.getDescription()]--;
        bool found = false;
        long index = 0;
        for (auto i = 0; i < shaderBuffers[buffer.getDescription()].size(); i++) {
            if (shaderBuffers[buffer.getDescription()][i].get() == &buffer) {
                index = i;
                found = true;
                break;
            }
        }
        assert(found);
        shaderBuffers[buffer.getDescription()].erase(shaderBuffers[buffer.getDescription()].begin() + index);
    }

    RenderTarget &FrameGraphPool::createRenderTarget(const RenderTargetDesc &desc) {
        auto index = usedTargets[desc]++;
        if (targets[desc].size() <= index) {
            targets[desc].resize(usedTargets[desc]);
            targets[desc].at(index) = device->createRenderTarget(desc);
        }
        return *targets[desc].at(index);
    }

    void FrameGraphPool::destroyRenderTarget(RenderTarget &target) {
        usedTargets[target.getDescription()]--;
        bool found = false;
        long index = 0;
        for (auto i = 0; i < targets[target.getDescription()].size(); i++) {
            if (targets[target.getDescription()][i].get() == &target) {
                index = i;
                found = true;
                break;
            }
        }
        assert(found);
        targets[target.getDescription()].erase(targets[target.getDescription()].begin() + index);
    }
}
