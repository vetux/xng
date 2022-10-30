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

#ifndef XENGINE_FRAMEGRAPHPOOLALLOCATOR_HPP
#define XENGINE_FRAMEGRAPHPOOLALLOCATOR_HPP

#include "render/graph/framegraphallocator.hpp"
#include "render/graph/framegraphpool.hpp"

namespace xng {
    class FrameGraphPoolAllocator : public FrameGraphAllocator {
    public:
        FrameGraphPoolAllocator(RenderDevice &device, SPIRVCompiler &shaderCompiler, SPIRVDecompiler &spirvDecompiler)
                : pool(device, shaderCompiler, spirvDecompiler) {}

        void setFrame(const FrameGraph &value) override {
            frame = value;
            pool.collectGarbage();
            currentStage = 0;
        }

        FrameGraphPassResources allocateNext() override {
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
            Uri uri;
            if (allocation.isUri) {
                uri = std::get<Uri>(allocation.allocationData);
            }
            switch (allocation.objectType) {
                case RenderObject::VERTEX_BUFFER:
                    if (allocation.isUri) {
                        auto &mesh = pool.getMesh(ResourceHandle<Mesh>(uri));
                        objects[res] = &mesh;
                        return mesh;
                    } else {
                        throw std::runtime_error("Allocation contains vertex buffer allocation without specified uri");
                    }
                case RenderObject::TEXTURE_BUFFER:
                    if (allocation.isUri) {
                        auto &tex = pool.getTexture(ResourceHandle<Texture>(uri));
                        objects[res] = &tex;
                        return tex;
                    } else {
                        auto desc = std::get<TextureBufferDesc>(allocation.allocationData);
                        auto &tex = pool.createTextureBuffer(desc);
                        objects[res] = &tex;
                        return tex;
                    }
                case RenderObject::SHADER_BUFFER:
                    if (allocation.isUri) {
                        throw std::runtime_error("Allocation for shader buffer with specified uri is not allowed");
                    } else {
                        auto desc = std::get<ShaderBufferDesc>(allocation.allocationData);
                        auto &buf = pool.createShaderBuffer(desc);
                        objects[res] = &buf;
                        return buf;
                    }
                case RenderObject::SHADER_PROGRAM:
                    throw std::runtime_error(
                            "Shader programs cannot be explicitly allocated, Create a render pipeline instead.");
                case RenderObject::RENDER_TARGET:
                    if (allocation.isUri) {
                        throw std::runtime_error("Allocation for render target with specified uri is not allowed");
                    } else {
                        auto desc = std::get<RenderTargetDesc>(allocation.allocationData);
                        auto &target = pool.createRenderTarget(desc);
                        objects[res] = &target;
                        return target;

                    }
                case RenderObject::RENDER_PIPELINE:
                    if (allocation.isUri) {
                        throw std::runtime_error("Allocation for render pipeline with specified uri is not allowed");
                    } else {
                        auto pair = std::get<std::pair<ResourceHandle<Shader>, RenderPipelineDesc>>(
                                allocation.allocationData);
                        auto &pipeline = pool.getPipeline(pair.first, pair.second);
                        objects[res] = &pipeline;
                        return pipeline;
                    }
                case RenderObject::COMPUTE_PIPELINE:
                    throw std::runtime_error("Compute pipeline allocation is not supported yet");
                case RenderObject::RAYTRACE_PIPELINE:
                    throw std::runtime_error("Raytrace pipeline allocation is not supported yet");
            }
        }

        void deallocate(const FrameGraphResource &resource) {
            pool.destroy(*objects.at(resource));
            objects.erase(resource);
        }

        FrameGraph frame;
        FrameGraphPool pool;
        std::map<FrameGraphResource, RenderObject *> objects;
        size_t currentStage = 0;
    };
}
#endif //XENGINE_FRAMEGRAPHPOOLALLOCATOR_HPP
