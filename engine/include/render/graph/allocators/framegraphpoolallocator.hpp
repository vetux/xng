/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2021  Julian Zampiccoli
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

        void collectGarbage() override {
            pool.collectGarbage();
        }

        FrameGraphResource allocate(const FrameGraphAllocation &allocation) override {
            Uri uri;
            if (allocation.isUri) {
                uri = std::get<Uri>(allocation.allocationData);
            }
            switch (allocation.objectType) {
                case RenderObject::VERTEX_BUFFER:
                    if (allocation.isUri) {
                        auto &mesh = pool.getMesh(ResourceHandle<Mesh>(uri));
                        auto ret = getNewResource();
                        objects[ret] = &mesh;
                        return ret;
                    } else {
                        throw std::runtime_error("Allocation contains vertex buffer allocation without specified uri");
                    }
                case RenderObject::TEXTURE_BUFFER:
                    if (allocation.isUri) {
                        auto &tex = pool.getTexture(ResourceHandle<Texture>(uri));
                        auto ret = getNewResource();
                        objects[ret] = &tex;
                        return ret;
                    } else {
                        auto desc = std::get<TextureBufferDesc>(allocation.allocationData);
                        auto &tex = pool.createTextureBuffer(desc);
                        auto ret = getNewResource();
                        objects[ret] = &tex;
                        return ret;
                    }
                case RenderObject::SHADER_BUFFER:
                    if (allocation.isUri) {
                        throw std::runtime_error("Allocation for shader buffer with specified uri is not allowed");
                    } else {
                        auto desc = std::get<ShaderBufferDesc>(allocation.allocationData);
                        auto &buf = pool.createShaderBuffer(desc);
                        auto ret = getNewResource();
                        objects[ret] = &buf;
                        return ret;
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
                        auto ret = getNewResource();
                        objects[ret] = &target;
                        return ret;

                    }
                case RenderObject::RENDER_PIPELINE:
                    if (allocation.isUri) {
                        throw std::runtime_error("Allocation for render pipeline with specified uri is not allowed");
                    } else {
                        auto pair = std::get<std::pair<ResourceHandle<Shader>, RenderPipelineDesc>>(
                                allocation.allocationData);
                        auto &pipeline = pool.getPipeline(pair.first, pair.second);
                        auto ret = getNewResource();
                        objects[ret] = &pipeline;
                        return ret;
                    }
                case RenderObject::COMPUTE_PIPELINE:
                    throw std::runtime_error("Compute pipeline allocation is not supported yet");
                case RenderObject::RAYTRACE_PIPELINE:
                    throw std::runtime_error("Raytrace pipeline allocation is not supported yet");
            }
        }

        void deallocate(const FrameGraphResource &resource) override {
            pool.destroy(*objects.at(resource));
            objects.erase(resource);
        }

    private:
        FrameGraphResource getNewResource() {
            if (looseResources.empty()) {
                return FrameGraphResource(resourceCounter++);
            } else {
                auto v = *looseResources.begin();
                looseResources.erase(looseResources.begin());
                return v;
            }
        }

        FrameGraphPool pool;

        std::map<FrameGraphResource, RenderObject *> objects;

        size_t resourceCounter = 1;
        std::set<FrameGraphResource> looseResources;
    };
}
#endif //XENGINE_FRAMEGRAPHPOOLALLOCATOR_HPP
