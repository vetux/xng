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

#ifndef XENGINE_RENDERALLOCATOR_HPP
#define XENGINE_RENDERALLOCATOR_HPP

#include "xng/gpu/compute/computepipeline.hpp"
#include "xng/gpu/compute/computecommand.hpp"
#include "xng/gpu/raytrace/raytracepipeline.hpp"
#include "xng/gpu/raytrace/raytracecommand.hpp"

#include "xng/gpu/renderpipeline.hpp"
#include "xng/gpu/rendertarget.hpp"
#include "xng/gpu/texturebuffer.hpp"
#include "xng/gpu/vertexbuffer.hpp"
#include "xng/gpu/shaderbuffer.hpp"

#include "xng/gpu/vertexattribute.hpp"

#include "xng/gpu/renderpipelinedesc.hpp"
#include "xng/gpu/shaderbufferdesc.hpp"
#include "xng/gpu/vertexbufferdesc.hpp"
#include "xng/gpu/rendertargetdesc.hpp"
#include "xng/gpu/texturebufferdesc.hpp"

#include "xng/gpu/vertexarrayobject.hpp"

#include "xng/gpu/gpumemory.hpp"

#include "xng/shader/spirvdecompiler.hpp"

#include "xng/asset/mesh.hpp"

namespace xng {
    /**
     * The RenderAllocator is used to allocate RenderObjects.
     */
    class RenderAllocator {
    public:
        virtual ~RenderAllocator() = default;

        /**
          * @return The list of currently allocated render objects
          */
        virtual std::set<RenderObject *> getAllocatedObjects() = 0;

        /**
         *
         * @param desc The description of the pipeline
         * @param shader The shader to bind when rendering vertex buffers
         * @param bindings The set of buffers to bind to the shader in the given order when rendering vertex buffers
         * @return
         */
        virtual std::unique_ptr<RenderPipeline> createRenderPipeline(const RenderPipelineDesc &desc) = 0;

        virtual std::unique_ptr<RenderPipeline> createRenderPipeline(const uint8_t *cacheData, size_t size) = 0;

        virtual std::unique_ptr<ComputePipeline> createComputePipeline(const ComputePipelineDesc &desc) = 0;

        virtual std::unique_ptr<RaytracePipeline> createRaytracePipeline(const RaytracePipelineDesc &desc) = 0;

        virtual std::unique_ptr<RenderTarget> createRenderTarget(const RenderTargetDesc &desc) = 0;

        virtual std::unique_ptr<VertexArrayObject> createVertexArrayObject(const VertexArrayObjectDesc &desc) = 0;

        /**
         * Create buffer without specifying a backing GpuMemory.
         * The buffer storage is managed by the allocator.
         *
         * @param desc
         * @return
         */
        virtual std::unique_ptr<VertexBuffer> createVertexBuffer(const VertexBufferDesc &desc) = 0;

        virtual std::unique_ptr<IndexBuffer> createIndexBuffer(const IndexBufferDesc &desc) = 0;

        virtual std::unique_ptr<ShaderBuffer> createShaderBuffer(const ShaderBufferDesc &desc) = 0;

        virtual std::unique_ptr<TextureBuffer> createTextureBuffer(const TextureBufferDesc &desc) = 0;

        virtual std::unique_ptr<TextureArrayBuffer> createTextureArrayBuffer(const TextureArrayBufferDesc &desc) = 0;

        /**
         * Allocate a region of memory on the gpu device into which RenderBuffer objects can be mapped.
         *
         * @param desc
         * @return
         */
        virtual std::unique_ptr<GpuMemory> createMemory(const GpuMemoryDesc &desc) = 0;
    };
}
#endif //XENGINE_RENDERALLOCATOR_HPP
