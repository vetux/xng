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

/*
#include "xng/render/texture.hpp"
#include "xng/shader/shader.hpp"

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
            persistentObjects.erase(obj);
        }

        objects.clear();

        // Resize object pools
        std::unordered_set<RenderPipelineDesc> pipelineDel;
        for (auto &pair: pipelines) {
            if (usedPipelines.find(pair.first) == usedPipelines.end()) {
                pipelineDel.insert(pair.first);
            }
        }
        for (auto &val : pipelineDel)
            pipelines.erase(val);

        std::unordered_set<RenderPassDesc> passDel;
        for (auto &pair: passes) {
            if (usedPasses.find(pair.first) == usedPasses.end()) {
                passDel.insert(pair.first);
            }
        }
        for (auto &val : passDel)
            passes.erase(val);

        std::unordered_set<VertexBufferDesc> vbDel;
        for (auto &pair: vertexBuffers) {
            if (usedVertexBuffers.find(pair.first) == usedVertexBuffers.end()) {
                vbDel.insert(pair.first);
            } else if (pair.second.size() > usedVertexBuffers.at(pair.first)) {
                pair.second.resize(usedVertexBuffers.at(pair.first));
            }
        }
        for (auto &val : vbDel)
            vertexBuffers.erase(val);

        std::unordered_set<IndexBufferDesc> ibDel;
        for (auto &pair: indexBuffers) {
            if (usedIndexBuffers.find(pair.first) == usedIndexBuffers.end()) {
                ibDel.insert(pair.first);
            } else if (pair.second.size() > usedIndexBuffers.at(pair.first)) {
                pair.second.resize(usedIndexBuffers.at(pair.first));
            }
        }
        for (auto &val : ibDel)
            indexBuffers.erase(val);

        std::unordered_set<VertexArrayObjectDesc> vaoDel;
        for (auto &pair: vertexArrayObjects) {
            if (usedVertexArrayObjects.find(pair.first) == usedVertexArrayObjects.end()) {
                vaoDel.insert(pair.first);
            } else if (pair.second.size() > usedVertexArrayObjects.at(pair.first)) {
                pair.second.resize(usedVertexArrayObjects.at(pair.first));
            }
        }
        for (auto &val : vaoDel)
            vertexArrayObjects.erase(val);

        std::unordered_set<TextureBufferDesc> texDel;
        for (auto &pair: textures) {
            if (usedTextures.find(pair.first) == usedTextures.end()) {
                texDel.insert(pair.first);
            } else if (pair.second.size() > usedTextures.at(pair.first)) {
                pair.second.resize(usedTextures.at(pair.first));
            }
        }
        for (auto &val : texDel)
            textures.erase(val);

        std::unordered_set<TextureArrayBufferDesc> texArrayDel;
        for (auto &pair: textureArrays) {
           if (usedTextureArrays.find(pair.first) == usedTextureArrays.end()) {
               texArrayDel.insert(pair.first);
            } else if (pair.second.size() > usedTextureArrays.at(pair.first)) {
                pair.second.resize(usedTextureArrays.at(pair.first));
            }
        }
        for (auto &val : texArrayDel)
            textureArrays.erase(val);

        std::unordered_set<ShaderUniformBufferDesc> sbDel;
        for (auto &pair: shaderBuffers) {
            if (usedShaderBuffers.find(pair.first) == usedShaderBuffers.end()) {
                sbDel.insert(pair.first);
            } else if (pair.second.size() > usedShaderBuffers.at(pair.first)) {
                pair.second.resize(usedShaderBuffers.at(pair.first));
            }
        }
        for (auto &val : sbDel)
            shaderBuffers.erase(val);

        std::unordered_set<ShaderStorageBufferDesc> ssboDel;
        for (auto &pair: shaderStorageBuffers) {
            if (usedShaderStorageBuffers.find(pair.first) == usedShaderStorageBuffers.end()) {
                ssboDel.insert(pair.first);
            } else if (pair.second.size() > usedShaderStorageBuffers.at(pair.first)) {
                pair.second.resize(usedShaderStorageBuffers.at(pair.first));
            }
        }
        for (auto &val : ssboDel)
            shaderStorageBuffers.erase(val);

        std::unordered_set<RenderTargetDesc> targetDel;
        for (auto &pair: targets) {
           if (usedTargets.find(pair.first) == usedTargets.end()) {
               targetDel.insert(pair.first);
            } else  if (pair.second.size() > usedTargets.at(pair.first)) {
                pair.second.resize(usedTargets.at(pair.first));
            }
        }
        for (auto &val : targetDel)
            targets.erase(val);

        if (commandBuffers.size() > usedCommandBuffers) {
            commandBuffers.resize(usedCommandBuffers);
        }

        usedPipelines.clear();
        usedPasses.clear();
        usedVertexBuffers.clear();
        usedIndexBuffers.clear();
        usedVertexArrayObjects.clear();
        usedTextures.clear();
        usedTextureArrays.clear();
        usedShaderBuffers.clear();
        usedShaderStorageBuffers.clear();
        usedTargets.clear();
        usedCommandBuffers = 0;
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

    ShaderUniformBuffer &FrameGraphPoolAllocator::createShaderUniformBuffer(const ShaderUniformBufferDesc &desc) {
        auto index = usedShaderBuffers[desc]++;
        if (shaderBuffers[desc].size() <= index) {
            shaderBuffers[desc].resize(usedShaderBuffers[desc]);
            shaderBuffers[desc].at(index) = device->createShaderUniformBuffer(desc);
        }
        return *shaderBuffers[desc].at(index);
    }

    ShaderStorageBuffer &FrameGraphPoolAllocator::createShaderStorageBuffer(const ShaderStorageBufferDesc &desc) {
        auto index = usedShaderStorageBuffers[desc]++;
        if (shaderStorageBuffers[desc].size() <= index) {
            shaderStorageBuffers[desc].resize(usedShaderStorageBuffers[desc]);
            shaderStorageBuffers[desc].at(index) = device->createShaderStorageBuffer(desc);
        }
        return *shaderStorageBuffers[desc].at(index);
    }

    RenderTarget &FrameGraphPoolAllocator::createRenderTarget(const RenderTargetDesc &desc) {
        auto index = usedTargets[desc]++;
        if (targets[desc].size() <= index) {
            targets[desc].resize(usedTargets[desc]);
            targets[desc].at(index) = device->createRenderTarget(desc);
        }
        return *targets[desc].at(index);
    }

    CommandBuffer &FrameGraphPoolAllocator::createCommandBuffer() {
        auto index = usedCommandBuffers++;
        if (commandBuffers.size() <= index) {
            commandBuffers.resize(usedCommandBuffers);
            commandBuffers.at(index) = device->createCommandBuffer();
        }
        return *commandBuffers.at(index);
    }

    std::unique_ptr<RenderObject> FrameGraphPoolAllocator::persist(RenderObject &obj) {
        switch (obj.getType()) {
            default:
                throw std::runtime_error("Invalid object type");
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
                auto ret = std::unique_ptr<RenderObject>(vertexBuffers.at(buffer.getDescription()).at(index).release());
                vertexBuffers.at(buffer.getDescription()).erase(
                        vertexBuffers.at(buffer.getDescription()).begin() + index);
                return ret;
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
                auto ret = std::unique_ptr<RenderObject>(indexBuffers.at(buffer.getDescription()).at(index).release());
                indexBuffers.at(buffer.getDescription()).erase(
                        indexBuffers.at(buffer.getDescription()).begin() + index);
                return ret;
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
                auto ret = std::unique_ptr<RenderObject>(
                        vertexArrayObjects.at(buffer.getDescription()).at(index).release());
                vertexArrayObjects.at(buffer.getDescription()).erase(
                        vertexArrayObjects.at(buffer.getDescription()).begin() + index);
                return ret;
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
                auto ret = std::unique_ptr<RenderObject>(textures.at(buffer.getDescription()).at(index).release());
                textures.at(buffer.getDescription()).erase(textures.at(buffer.getDescription()).begin() + index);
                return ret;
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
                auto ret = std::unique_ptr<RenderObject>(textureArrays.at(buffer.getDescription()).at(index).release());
                textureArrays.at(buffer.getDescription()).erase(
                        textureArrays.at(buffer.getDescription()).begin() + index);
                return ret;
            }
            case RenderObject::RENDER_OBJECT_SHADER_UNIFORM_BUFFER: {
                auto &buffer = dynamic_cast<ShaderUniformBuffer &>(obj);
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
                auto ret = std::unique_ptr<RenderObject>(shaderBuffers.at(buffer.getDescription()).at(index).release());
                shaderBuffers.at(buffer.getDescription()).erase(
                        shaderBuffers.at(buffer.getDescription()).begin() + index);
                return ret;
            }
            case RenderObject::RENDER_OBJECT_SHADER_STORAGE_BUFFER: {
                auto &buffer = dynamic_cast<ShaderStorageBuffer &>(obj);
                usedShaderStorageBuffers[buffer.getDescription()]--;
                bool found = false;
                long index = 0;
                for (auto i = 0; i < shaderStorageBuffers[buffer.getDescription()].size(); i++) {
                    if (shaderStorageBuffers[buffer.getDescription()][i].get() == &buffer) {
                        index = i;
                        found = true;
                        break;
                    }
                }
                assert(found);
                auto ret = std::unique_ptr<RenderObject>(
                        shaderStorageBuffers.at(buffer.getDescription()).at(index).release());
                shaderStorageBuffers.at(buffer.getDescription()).erase(
                        shaderStorageBuffers.at(buffer.getDescription()).begin() + index);
                return ret;
            }
            case RenderObject::RENDER_OBJECT_RENDER_TARGET: {
                auto &target = dynamic_cast<RenderTarget &>(obj);
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
                auto ret = device->createRenderTarget(target.getDescription());
                if (--usedTargets[target.getDescription()] == 0) {
                    usedTargets.erase(target.getDescription());
                    targets.at(target.getDescription()).erase(targets.at(target.getDescription()).begin() + index);
                }
                return ret;
            }
            case RenderObject::RENDER_OBJECT_RENDER_PIPELINE: {
                auto &pipeline = dynamic_cast<RenderPipeline &>(obj);
                auto ret = device->createRenderPipeline(pipeline.getDescription(), *shaderDecompiler);
                if (--usedPipelines[pipeline.getDescription()] == 0) {
                    usedPipelines.erase(pipeline.getDescription());
                    pipelines.erase(pipeline.getDescription());
                }
                return ret;
            }
            case RenderObject::RENDER_OBJECT_RENDER_PASS: {
                auto &pass = dynamic_cast<RenderPass &>(obj);
                auto ret = device->createRenderPass(pass.getDescription());
                if (--usedPasses[pass.getDescription()] == 0) {
                    usedPasses.erase(pass.getDescription());
                    passes.erase(pass.getDescription());
                }
                return ret;
            }
        }
    }
}*/