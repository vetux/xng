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

#include "xng/render/graph/allocators/framegraphpoolallocator.hpp"

#include "xng/asset/texture.hpp"
#include "xng/asset/shader.hpp"

namespace xng {
    void FrameGraphPoolAllocator::collectGarbage() {
        // Deallocate unused persistent objects
        std::set<FrameGraphResource> delObjects;
        for (auto &pair: persistentObjects) {
            bool deallocate = true;
            for (auto &stage: frame.stages) {
                if (stage.persists.find(pair.first) != stage.persists.end()) {
                    deallocate = false;
                    break;
                }
            }
            if (deallocate) {
                delObjects.insert(pair.first);
            }
        }

        for (auto &obj: delObjects) {
            deallocate(obj);
        }

        // Deallocate temporary objects
        auto tmpObj = objects;
        for (auto &pair: tmpObj) {
            deallocate(pair.first);
        }

        assert(objects.empty());

        // Resize object pools
        for (auto &pair: vertexBuffers) {
            if (pair.second.size() - usedVertexBuffers.at(pair.first) > poolCacheSize) {
                pair.second.resize(usedVertexBuffers.at(pair.first) + poolCacheSize);
            }
        }
        for (auto &pair: indexBuffers) {
            if (pair.second.size() - usedIndexBuffers.at(pair.first) > poolCacheSize) {
                pair.second.resize(usedIndexBuffers.at(pair.first) + poolCacheSize);
            }
        }
        for (auto &pair: vertexArrayObjects) {
            if (pair.second.size() - usedVertexArrayObjects.at(pair.first) > poolCacheSize) {
                pair.second.resize(usedVertexArrayObjects.at(pair.first) + poolCacheSize);
            }
        }
        for (auto &pair: textures) {
            if (pair.second.size() - usedTextures.at(pair.first) > poolCacheSize) {
                pair.second.resize(usedTextures.at(pair.first) + poolCacheSize);
            }
        }
        for (auto &pair: textureArrays) {
            if (pair.second.size() - usedTextureArrays.at(pair.first) > poolCacheSize) {
                pair.second.resize(usedTextureArrays.at(pair.first) + poolCacheSize);
            }
        }
        for (auto &pair: shaderBuffers) {
            if (pair.second.size() - usedShaderBuffers.at(pair.first) > poolCacheSize) {
                pair.second.resize(usedShaderBuffers.at(pair.first) + poolCacheSize);
            }
        }
        for (auto &pair: targets) {
            if (pair.second.size() - usedTargets.at(pair.first) > poolCacheSize) {
                pair.second.resize(usedTargets.at(pair.first) + poolCacheSize);
            }
        }
    }

    RenderPipeline &FrameGraphPoolAllocator::getPipeline(const RenderPipelineDesc &desc) {
        if (usedPipelines[desc]++ == 0) {
            pipelines[desc] = device->createRenderPipeline(desc, *shaderDecompiler);
        }
        return *pipelines.at(desc);
    }

    RenderPass &FrameGraphPoolAllocator::getRenderPass(const RenderPassDesc &desc) {
        if (usedPasses[desc]++ == 0) {
            passes[desc] = device->createRenderPass(desc);
        }
        return *passes.at(desc);
    }

    VertexBuffer &FrameGraphPoolAllocator::createVertexBuffer(const VertexBufferDesc &desc) {
        auto index = usedVertexBuffers[desc]++;
        if (vertexBuffers[desc].size() <= index) {
            vertexBuffers[desc].resize(usedVertexBuffers[desc]);
            vertexBuffers[desc].at(index) = device->createVertexBuffer(desc);
        }
        return *vertexBuffers[desc].at(index);
    }

    IndexBuffer &FrameGraphPoolAllocator::createIndexBuffer(const IndexBufferDesc &desc) {
        auto index = usedIndexBuffers[desc]++;
        if (indexBuffers[desc].size() <= index) {
            indexBuffers[desc].resize(usedIndexBuffers[desc]);
            indexBuffers[desc].at(index) = device->createIndexBuffer(desc);
        }
        return *indexBuffers[desc].at(index);
    }

    VertexArrayObject &FrameGraphPoolAllocator::createVertexArrayObject(const VertexArrayObjectDesc &desc) {
        auto index = usedVertexArrayObjects[desc]++;
        if (vertexArrayObjects[desc].size() <= index) {
            vertexArrayObjects[desc].resize(usedVertexArrayObjects[desc]);
            vertexArrayObjects[desc].at(index) = device->createVertexArrayObject(desc);
        }
        return *vertexArrayObjects[desc].at(index);
    }

    TextureBuffer &FrameGraphPoolAllocator::createTextureBuffer(const TextureBufferDesc &desc) {
        auto index = usedTextures[desc]++;
        if (textures[desc].size() <= index) {
            textures[desc].resize(usedTextures[desc]);
            textures[desc].at(index) = device->createTextureBuffer(desc);
        }
        return *textures[desc].at(index);
    }

    TextureArrayBuffer &FrameGraphPoolAllocator::createTextureArrayBuffer(const TextureArrayBufferDesc &desc) {
        auto index = usedTextureArrays[desc]++;
        if (textureArrays[desc].size() <= index) {
            textureArrays[desc].resize(usedTextureArrays[desc]);
            textureArrays[desc].at(index) = device->createTextureArrayBuffer(desc);
        }
        return *textureArrays[desc].at(index);
    }

    ShaderBuffer &FrameGraphPoolAllocator::createShaderBuffer(const ShaderBufferDesc &desc) {
        auto index = usedShaderBuffers[desc]++;
        if (shaderBuffers[desc].size() <= index) {
            shaderBuffers[desc].resize(usedShaderBuffers[desc]);
            shaderBuffers[desc].at(index) = device->createShaderBuffer(desc);
        }
        return *shaderBuffers[desc].at(index);
    }

    RenderTarget &FrameGraphPoolAllocator::createRenderTarget(const RenderTargetDesc &desc) {
        auto index = usedTargets[desc]++;
        if (targets[desc].size() <= index) {
            targets[desc].resize(usedTargets[desc]);
            targets[desc].at(index) = device->createRenderTarget(desc);
        }
        return *targets[desc].at(index);
    }

    void FrameGraphPoolAllocator::destroy(RenderObject &obj) {
        switch (obj.getType()) {
            case RenderObject::RENDER_OBJECT_VERTEX_BUFFER: {
                auto &buffer = dynamic_cast<VertexBuffer &>(obj);
                usedVertexBuffers[buffer.getDescription()]--;
                bool found = false;
                long index = 0;
                for (auto i = 0; i < vertexBuffers[buffer.getDescription()].size(); i++) {
                    if (vertexBuffers[buffer.getDescription()][i].get() == &buffer) {
                        index = i;
                        found = true;
                        break;
                    }
                }
                assert(found);
                vertexBuffers[buffer.getDescription()].erase(vertexBuffers[buffer.getDescription()].begin() + index);
                break;
            }
            case RenderObject::RENDER_OBJECT_INDEX_BUFFER: {
                auto &buffer = dynamic_cast<IndexBuffer &>(obj);
                usedIndexBuffers[buffer.getDescription()]--;
                bool found = false;
                long index = 0;
                for (auto i = 0; i < indexBuffers[buffer.getDescription()].size(); i++) {
                    if (indexBuffers[buffer.getDescription()][i].get() == &buffer) {
                        index = i;
                        found = true;
                        break;
                    }
                }
                assert(found);
                indexBuffers[buffer.getDescription()].erase(indexBuffers[buffer.getDescription()].begin() + index);
                break;
            }
            case RenderObject::RENDER_OBJECT_VERTEX_ARRAY_OBJECT: {
                auto &buffer = dynamic_cast<VertexArrayObject &>(obj);
                usedVertexArrayObjects[buffer.getDescription()]--;
                bool found = false;
                long index = 0;
                for (auto i = 0; i < vertexArrayObjects[buffer.getDescription()].size(); i++) {
                    if (vertexArrayObjects[buffer.getDescription()][i].get() == &buffer) {
                        index = i;
                        found = true;
                        break;
                    }
                }
                assert(found);
                vertexArrayObjects[buffer.getDescription()].erase(
                        vertexArrayObjects[buffer.getDescription()].begin() + index);
                break;
            }
            case RenderObject::RENDER_OBJECT_TEXTURE_BUFFER: {
                auto &buffer = dynamic_cast<TextureBuffer &>(obj);
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
                break;
            }
            case RenderObject::RENDER_OBJECT_TEXTURE_ARRAY_BUFFER: {
                auto &buffer = dynamic_cast<TextureArrayBuffer &>(obj);
                usedTextureArrays[buffer.getDescription()]--;
                bool found = false;
                long index = 0;
                for (auto i = 0; i < textureArrays[buffer.getDescription()].size(); i++) {
                    if (textureArrays[buffer.getDescription()][i].get() == &buffer) {
                        index = i;
                        found = true;
                        break;
                    }
                }
                assert(found);
                textureArrays[buffer.getDescription()].erase(textureArrays[buffer.getDescription()].begin() + index);
                break;
            }
            case RenderObject::RENDER_OBJECT_SHADER_BUFFER: {
                auto &buffer = dynamic_cast<ShaderBuffer &>(obj);
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
                break;
            }
            case RenderObject::RENDER_OBJECT_RENDER_TARGET: {
                auto &target = dynamic_cast<RenderTarget &>(obj);
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
                break;
            }
            case RenderObject::RENDER_OBJECT_RENDER_PIPELINE: {
                auto &target = dynamic_cast<RenderPipeline &>(obj);
                if (--usedPipelines[target.getDescription()] == 0) {
                    usedPipelines.erase(target.getDescription());
                    pipelines.erase(target.getDescription());
                }
                break;
            }
            case RenderObject::RENDER_OBJECT_RENDER_PASS: {
                auto &target = dynamic_cast<RenderPass &>(obj);
                if (--usedPasses[target.getDescription()] == 0) {
                    usedPasses.erase(target.getDescription());
                    passes.erase(target.getDescription());
                }
                break;
            }
        }
    }

    /* static std::unique_ptr<TextureBuffer> allocateTexture(const Texture &t, RenderDevice &device) {
         auto desc = t.description;
         desc.bufferType = HOST_VISIBLE;
         auto stagingBuffer = device.createTextureBuffer(desc);

         if (!t.images.empty()) {
             if (t.description.textureType == TEXTURE_CUBE_MAP) {
                 for (int i = POSITIVE_X; i <= NEGATIVE_Z; i++) {
                     auto size = t.images.at(i).get().getSize();
                     stagingBuffer->upload(static_cast<CubeMapFace>(i),
                                           RGBA,
                                           reinterpret_cast<const uint8_t *>(t.images.at(i).get().getData()),
                                           sizeof(ColorRGBA) * (size.x * size.y));
                 }
             } else {
                 auto size = t.images.at(0).get().getSize();
                 stagingBuffer->upload(RGBA,
                                       reinterpret_cast<const uint8_t *>(t.images.at(0).get().getData()),
                                       sizeof(ColorRGBA) * (size.x * size.y));
             }
         }

         desc.bufferType = DEVICE_LOCAL;
         auto ret = device.createTextureBuffer(desc);
         ret->copy(*stagingBuffer);

         return ret;
     }

     void FrameGraphPoolAllocator::collectGarbage() {
         std::unordered_set<RenderPipelineDesc> unusedPipelines;
         for (auto &pair: pipelines) {
             if (usedPipelines.find(pair.first) == usedPipelines.end()) {
                 unusedPipelines.insert(pair.first);
             }
         }

         for (auto &v: unusedPipelines) {
             pipelines.erase(v);
         }

         usedTextures.clear();
         usedTargets.clear();
         usedPipelines.clear();

         textures.clear();
         shaderBuffers.clear();
         targets.clear();
     }

     VertexBuffer &FrameGraphPoolAllocator::getMesh(const ResourceHandle<Mesh> &handle) {
         usedUris.insert(handle.getUri());
         auto it = uriObjects.find(handle.getUri());
         if (it == uriObjects.end()) {
             uriObjects[handle.getUri()] = device->createInstancedVertexBuffer(handle.get(), {MatrixMath::identity()});
         }
         return dynamic_cast<VertexBuffer &>(*uriObjects.at(handle.getUri()));
     }

     TextureBuffer &FrameGraphPoolAllocator::getTexture(const ResourceHandle<Texture> &handle) {
         usedUris.insert(handle.getUri());
         auto it = uriObjects.find(handle.getUri());
         if (it == uriObjects.end()) {
             uriObjects[handle.getUri()] = allocateTexture(handle.get(), *device);
         }
         return dynamic_cast<TextureBuffer &>(*uriObjects.at(handle.getUri()));
     }

     ShaderProgram &FrameGraphPoolAllocator::getShader(const ResourceHandle<Shader> &handle) {
         usedUris.insert(handle.getUri());
         auto it = uriObjects.find(handle.getUri());
         if (it == uriObjects.end()) {
             auto &shader = handle.get();
             ShaderProgramDesc desc;
             shader.vertexShader.preprocess(*shaderCompiler);
             shader.fragmentShader.preprocess(*shaderCompiler);
             shader.geometryShader.preprocess(*shaderCompiler);
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

     RenderPipeline &FrameGraphPoolAllocator::getPipeline(const ResourceHandle<Shader> &shader, const RenderPipelineDesc &desc) {
         auto pair = PipelinePair(shader.getUri(), desc);
         usedPipelines.insert(pair);
         usedUris.insert(shader.getUri());
         auto it = pipelines.find(pair);
         if (it == pipelines.end()) {
             pipelines[pair] = device->createRenderPipeline(desc, getShader(shader));
         }
         return dynamic_cast<RenderPipeline &>(*pipelines.at(pair));
     }

     TextureBuffer &FrameGraphPoolAllocator::createTextureBuffer(const TextureBufferDesc &desc) {
         auto index = usedTextures[desc]++;
         if (textures[desc].size() <= index) {
             textures[desc].resize(usedTextures[desc]);
             textures[desc].at(index) = device->createTextureBuffer(desc);
         }
         return *textures[desc].at(index);
     }

     ShaderBuffer &FrameGraphPoolAllocator::createShaderBuffer(const ShaderBufferDesc &desc) {
         auto index = usedShaderBuffers[desc]++;
         if (shaderBuffers[desc].size() <= index) {
             shaderBuffers[desc].resize(usedShaderBuffers[desc]);
             shaderBuffers[desc].at(index) = device->createShaderBuffer(desc);
         }
         return *shaderBuffers[desc].at(index);
     }

     RenderTarget &FrameGraphPoolAllocator::createRenderTarget(const RenderTargetDesc &desc) {
         auto index = usedTargets[desc]++;
         if (targets[desc].size() <= index) {
             targets[desc].resize(usedTargets[desc]);
             targets[desc].at(index) = device->createRenderTarget(desc);
         }
         return *targets[desc].at(index);
     }

     void FrameGraphPoolAllocator::destroy(RenderObject &obj) {
         switch (obj.getType()) {
             case RenderObject::TEXTURE_BUFFER: {
                 auto &buffer = dynamic_cast<TextureBuffer&>(obj);
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
                 break;
             case RenderObject::SHADER_BUFFER: {
                 auto &buffer = dynamic_cast<ShaderBuffer&>(obj);
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
                 break;
             }
             case RenderObject::RENDER_TARGET: {
                 auto &target = dynamic_cast<RenderTarget&>(obj);
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
                 break;
             }
         }
     }*/
}