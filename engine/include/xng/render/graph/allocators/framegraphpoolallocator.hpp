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

#ifndef XENGINE_FRAMEGRAPHPOOLALLOCATOR_HPP
#define XENGINE_FRAMEGRAPHPOOLALLOCATOR_HPP

#include <unordered_set>

#include "xng/render/graph/framegraphallocator.hpp"

namespace xng {
    /**
     * The FrameGraphPoolAllocator using object pool pattern for allocating the render objects.
     */
    class XENGINE_EXPORT FrameGraphPoolAllocator : public FrameGraphAllocator {
    public:
        FrameGraphPoolAllocator(RenderDevice &device,
                                ShaderCompiler &shaderCompiler,
                                ShaderDecompiler &shaderDecompiler,
                                RenderTarget &backBuffer,
                                size_t poolCacheSize = 0)
                : device(&device),
                  shaderCompiler(&shaderCompiler),
                  shaderDecompiler(&shaderDecompiler),
                  poolCacheSize(poolCacheSize),
                  backBuffer(&backBuffer) {}

        FrameGraphPoolAllocator(const FrameGraphPoolAllocator &) = delete;

        FrameGraphAllocator & operator=(const FrameGraphPoolAllocator &) = delete;

        void beginFrame(const FrameGraph &value) override {
            frame = value;
            collectGarbage();
            currentStage = 0;
        }

        FrameGraphPassResources allocateNextPass() override {
            // Deallocate resources which are not used in any subsequent pass read / write declaration
            if (currentStage != 0) {
                std::set<FrameGraphResource> delObjects;
                for (auto &pair: objects) {
                    bool deallocate = true;
                    for (auto i = currentStage; i < frame.stages.size(); i++) {
                        auto &stage = frame.stages.at(i);
                        if (stage.reads.find(pair.first) != stage.reads.end()
                            || stage.writes.find(pair.first) != stage.writes.end()) {
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
            }

            auto &stage = frame.stages.at(currentStage);

            std::map<FrameGraphResource, RenderObject *> res;

            // Allocate resources created by this stage
            for (auto &alloc: stage.allocations) {
                allocate(alloc, frame.allocations.at(alloc));
            }

            // Persist resources
            for (auto &pRes: stage.persists) {
                persist(pRes);
            }

            // Add Read / Write accesses
            for (auto &read: stage.reads) {
                if (read.index == 0) {
                    res[read] = backBuffer;
                } else {
                    auto it = persistentObjects.find(read);
                    if (it != persistentObjects.end()) {
                        res[read] = persistentObjects.at(read).get();
                    } else {
                        res[read] = objects.at(read);
                    }
                }
            }

            for (auto &write: stage.writes) {
                if (write.index == 0) {
                    res[write] = backBuffer;
                } else {
                    auto it = persistentObjects.find(write);
                    if (it != persistentObjects.end()) {
                        res[write] = persistentObjects.at(write).get();
                    } else {
                        res[write] = objects.at(write);
                    }
                }
            }

            currentStage++;

            return FrameGraphPassResources(res);
        }

    private:
        RenderObject &allocate(const FrameGraphResource &res, const FrameGraphAllocation &allocation) {
            if (objects.find(res) != objects.end() || persistentObjects.find(res) != persistentObjects.end()) {
                throw std::runtime_error("Object already allocated for given resource handle");
            }

            switch (allocation.objectType) {
                case RenderObject::RENDER_OBJECT_VERTEX_BUFFER: {
                    auto desc = std::get<VertexBufferDesc>(allocation.allocationData);
                    auto &tex = createVertexBuffer(desc);
                    objects[res] = &tex;
                    return tex;
                }
                case RenderObject::RENDER_OBJECT_INDEX_BUFFER: {
                    auto desc = std::get<IndexBufferDesc>(allocation.allocationData);
                    auto &tex = createIndexBuffer(desc);
                    objects[res] = &tex;
                    return tex;
                }
                case RenderObject::RENDER_OBJECT_VERTEX_ARRAY_OBJECT: {
                    auto desc = std::get<VertexArrayObjectDesc>(allocation.allocationData);
                    auto &tex = createVertexArrayObject(desc);
                    objects[res] = &tex;
                    return tex;
                }
                case RenderObject::RENDER_OBJECT_TEXTURE_BUFFER: {
                    auto desc = std::get<TextureBufferDesc>(allocation.allocationData);
                    auto &tex = createTextureBuffer(desc);
                    objects[res] = &tex;
                    return tex;
                }
                case RenderObject::RENDER_OBJECT_TEXTURE_ARRAY_BUFFER: {
                    auto desc = std::get<TextureArrayBufferDesc>(allocation.allocationData);
                    auto &tex = createTextureArrayBuffer(desc);
                    objects[res] = &tex;
                    return tex;
                }
                case RenderObject::RENDER_OBJECT_SHADER_UNIFORM_BUFFER: {
                    auto desc = std::get<ShaderUniformBufferDesc>(allocation.allocationData);
                    auto &buf = createShaderUniformBuffer(desc);
                    objects[res] = &buf;
                    return buf;
                }
                case RenderObject::RENDER_OBJECT_SHADER_STORAGE_BUFFER: {
                    auto desc = std::get<ShaderStorageBufferDesc>(allocation.allocationData);
                    auto &buf = createShaderStorageBuffer(desc);
                    objects[res] = &buf;
                    return buf;
                }
                case RenderObject::RENDER_OBJECT_RENDER_TARGET: {
                    auto desc = std::get<RenderTargetDesc>(allocation.allocationData);
                    auto &target = createRenderTarget(desc);
                    objects[res] = &target;
                    return target;
                }
                case RenderObject::RENDER_OBJECT_RENDER_PIPELINE: {
                    auto desc = std::get<RenderPipelineDesc>(allocation.allocationData);
                    auto &pipeline = getPipeline(desc);
                    objects[res] = &pipeline;
                    return pipeline;
                }
                case RenderObject::RENDER_OBJECT_RENDER_PASS: {
                    auto desc = std::get<RenderPassDesc>(allocation.allocationData);
                    auto &tex = getRenderPass(desc);
                    objects[res] = &tex;
                    return tex;
                }
                default:
                    throw std::runtime_error("Invalid render object type");
            }
        }

        void deallocate(const FrameGraphResource &resource) {
            destroy(*objects.at(resource));
            objects.erase(resource);
        }

        void persist(const FrameGraphResource &resource) {
            if (persistentObjects.find(resource) == persistentObjects.end()) {
                persistentObjects[resource] = persist(*objects.at(resource));
                objects.erase(resource);
            }
        }

        void collectGarbage();

        RenderPipeline &getPipeline(const RenderPipelineDesc &desc);

        RenderPass &getRenderPass(const RenderPassDesc &desc);

        VertexBuffer &createVertexBuffer(const VertexBufferDesc &desc);

        IndexBuffer &createIndexBuffer(const IndexBufferDesc &desc);

        VertexArrayObject &createVertexArrayObject(const VertexArrayObjectDesc &desc);

        TextureBuffer &createTextureBuffer(const TextureBufferDesc &desc);

        TextureArrayBuffer &createTextureArrayBuffer(const TextureArrayBufferDesc &desc);

        ShaderUniformBuffer &createShaderUniformBuffer(const ShaderUniformBufferDesc &desc);

        ShaderStorageBuffer &createShaderStorageBuffer(const ShaderStorageBufferDesc &desc);

        RenderTarget &createRenderTarget(const RenderTargetDesc &desc);

        void destroy(RenderObject &obj);

        std::unique_ptr<RenderObject> persist(RenderObject &obj);

        RenderDevice *device = nullptr;
        ShaderCompiler *shaderCompiler = nullptr;
        ShaderDecompiler *shaderDecompiler = nullptr;
        RenderTarget *backBuffer = nullptr;

        std::unordered_map<RenderPipelineDesc, std::unique_ptr<RenderPipeline>> pipelines;
        std::unordered_map<RenderPassDesc, std::unique_ptr<RenderPass>> passes;
        std::unordered_map<VertexBufferDesc, std::vector<std::unique_ptr<VertexBuffer>>> vertexBuffers;
        std::unordered_map<IndexBufferDesc, std::vector<std::unique_ptr<IndexBuffer>>> indexBuffers;
        std::unordered_map<VertexArrayObjectDesc, std::vector<std::unique_ptr<VertexArrayObject>>> vertexArrayObjects;
        std::unordered_map<TextureBufferDesc, std::vector<std::unique_ptr<TextureBuffer>>> textures;
        std::unordered_map<TextureArrayBufferDesc, std::vector<std::unique_ptr<TextureArrayBuffer>>> textureArrays;
        std::unordered_map<ShaderUniformBufferDesc, std::vector<std::unique_ptr<ShaderUniformBuffer>>> shaderBuffers;
        std::unordered_map<ShaderStorageBufferDesc, std::vector<std::unique_ptr<ShaderStorageBuffer>>> shaderStorageBuffers;
        std::unordered_map<RenderTargetDesc, std::vector<std::unique_ptr<RenderTarget>>> targets;

        std::unordered_map<RenderPipelineDesc, int> usedPipelines;
        std::unordered_map<RenderPassDesc, int> usedPasses;
        std::unordered_map<VertexBufferDesc, int> usedVertexBuffers;
        std::unordered_map<IndexBufferDesc, int> usedIndexBuffers;
        std::unordered_map<VertexArrayObjectDesc, int> usedVertexArrayObjects;
        std::unordered_map<TextureBufferDesc, int> usedTextures;
        std::unordered_map<TextureArrayBufferDesc, int> usedTextureArrays;
        std::unordered_map<ShaderUniformBufferDesc, int> usedShaderBuffers;
        std::unordered_map<ShaderStorageBufferDesc, int> usedShaderStorageBuffers;
        std::unordered_map<RenderTargetDesc, int> usedTargets;

        FrameGraph frame;

        std::map<FrameGraphResource, RenderObject *> objects;
        std::map<FrameGraphResource, std::unique_ptr<RenderObject>> persistentObjects;
        size_t currentStage = 0;

        size_t poolCacheSize{};
    };
}

#endif //XENGINE_FRAMEGRAPHPOOLALLOCATOR_HPP
