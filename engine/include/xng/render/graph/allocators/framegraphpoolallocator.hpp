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

#ifndef XENGINE_FRAMEGRAPHPOOLALLOCATOR_HPP
#define XENGINE_FRAMEGRAPHPOOLALLOCATOR_HPP

#include <unordered_set>

#include "xng/render/graph/framegraphallocator.hpp"

namespace xng {
    /**
     * The FrameGraphPoolAllocator using object pool pattern for allocating the render objects.
     */
  /*  class FrameGraphPoolAllocator : public FrameGraphAllocator {
    public:
        FrameGraphPoolAllocator(RenderDevice &device, SPIRVCompiler &shaderCompiler, SPIRVDecompiler &shaderDecompiler)
                : device(&device), shaderCompiler(&shaderCompiler), shaderDecompiler(&shaderDecompiler) {}

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

            // Allocate resources created by this stage
            std::map<FrameGraphResource, RenderObject *> res;
            for (auto &alloc: stage.resources) {
                allocate(alloc, frame.allocations.at(alloc));
                res[alloc] = objects.at(alloc);
            }

            currentStage++;

            return FrameGraphPassResources(res);
        }

    private:
        RenderObject &allocate(const FrameGraphResource &res, const FrameGraphAllocation &allocation) {
            switch (allocation.objectType) {
                case RenderObject::VERTEX_BUFFER:
                    if (allocation.isUri) {
                        auto &mesh = getMesh(ResourceHandle<Mesh>(uri));
                        objects[res] = &mesh;
                        return mesh;
                    } else {
                        throw std::runtime_error("Allocation contains vertex buffer allocation without specified uri");
                    }
                case RenderObject::TEXTURE_BUFFER:
                    if (allocation.isUri) {
                        auto &tex = getTexture(ResourceHandle<Texture>(uri));
                        objects[res] = &tex;
                        return tex;
                    } else {
                        auto desc = std::get<TextureBufferDesc>(allocation.allocationData);
                        auto &tex = createTextureBuffer(desc);
                        objects[res] = &tex;
                        return tex;
                    }
                case RenderObject::SHADER_BUFFER:
                    if (allocation.isUri) {
                        throw std::runtime_error("Allocation for shader buffer with specified uri is not allowed");
                    } else {
                        auto desc = std::get<ShaderBufferDesc>(allocation.allocationData);
                        auto &buf = createShaderBuffer(desc);
                        objects[res] = &buf;
                        return buf;
                    }
                case RenderObject::RENDER_TARGET:
                    if (allocation.isUri) {
                        throw std::runtime_error("Allocation for render target with specified uri is not allowed");
                    } else {
                        auto desc = std::get<RenderTargetDesc>(allocation.allocationData);
                        auto &target = createRenderTarget(desc);
                        objects[res] = &target;
                        return target;

                    }
                case RenderObject::RENDER_PIPELINE:
                    if (allocation.isUri) {
                        throw std::runtime_error("Allocation for render pipeline with specified uri is not allowed");
                    } else {
                        auto desc = std::get<RenderPipelineDesc>(allocation.allocationData);
                        auto &pipeline = getPipeline(desc);
                        objects[res] = &pipeline;
                        return pipeline;
                    }
                case RenderObject::COMPUTE_PIPELINE:
                    throw std::runtime_error("Compute pipeline allocation is not supported yet");
                case RenderObject::RAYTRACE_PIPELINE:
                    throw std::runtime_error("Raytrace pipeline allocation is not supported yet");
                default:
                    throw std::runtime_error("Invalid render object type");
            }
        }

        void deallocate(const FrameGraphResource &resource) {
            destroy(*objects.at(resource));
            objects.erase(resource);
        }

        void collectGarbage();

        RenderPipeline &getPipeline(const RenderPipelineDesc &desc);

        TextureBuffer &createTextureBuffer(const TextureBufferDesc &desc);

        ShaderBuffer &createShaderBuffer(const ShaderBufferDesc &desc);

        RenderTarget &createRenderTarget(const RenderTargetDesc &desc);

        void destroy(RenderObject &obj);

        struct PipelinePair {
            Uri uri;
            RenderPipelineDesc desc;

            PipelinePair() = default;

            PipelinePair(Uri uri, RenderPipelineDesc desc) : uri(std::move(uri)), desc(std::move(desc)) {}

            ~PipelinePair() = default;

            bool operator==(const PipelinePair &other) const {
                return uri == other.uri && desc == other.desc;
            }
        };

        class PipelinePairHash {
        public:
            std::size_t operator()(const PipelinePair &k) const {
                size_t ret = 0;
                hash_combine(ret, k.uri);
                hash_combine(ret, k.desc);
                return ret;
            }
        };

        RenderDevice *device = nullptr;
        ShaderCompiler *shaderCompiler = nullptr;
        ShaderDecompiler *shaderDecompiler = nullptr;

        std::unordered_map<Uri, std::unique_ptr<RenderObject>> uriObjects;
        std::unordered_map<PipelinePair, std::unique_ptr<RenderPipeline>, PipelinePairHash> pipelines;

        std::unordered_map<TextureBufferDesc, std::vector<std::unique_ptr<TextureBuffer>>> textures;
        std::unordered_map<ShaderBufferDesc, std::vector<std::unique_ptr<ShaderBuffer>>> shaderBuffers;
        std::unordered_map<RenderTargetDesc, std::vector<std::unique_ptr<RenderTarget>>> targets;

        std::set<Uri> usedUris;
        std::unordered_set<PipelinePair, PipelinePairHash> usedPipelines;

        std::unordered_map<TextureBufferDesc, int> usedTextures;
        std::unordered_map<ShaderBufferDesc, int> usedShaderBuffers;
        std::unordered_map<RenderTargetDesc, int> usedTargets;

        FrameGraph frame;

        std::map<FrameGraphResource, RenderObject *> objects;
        size_t currentStage = 0;
    };*/
}

#endif //XENGINE_FRAMEGRAPHPOOLALLOCATOR_HPP
