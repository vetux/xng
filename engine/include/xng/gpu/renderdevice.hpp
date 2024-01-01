/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2024  Julian Zampiccoli
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

#ifndef XENGINE_RENDERDEVICE_HPP
#define XENGINE_RENDERDEVICE_HPP

#include <map>
#include <functional>

#include "computepipeline.hpp"
#include "raytracepipeline.hpp"

#include "xng/gpu/renderpipeline.hpp"
#include "xng/gpu/rendertarget.hpp"
#include "xng/gpu/texturebuffer.hpp"
#include "xng/gpu/vertexbuffer.hpp"
#include "xng/gpu/shaderuniformbuffer.hpp"
#include "xng/gpu/shaderstoragebuffer.hpp"
#include "xng/gpu/renderpass.hpp"

#include "xng/gpu/commandbuffer.hpp"
#include "xng/gpu/commandqueue.hpp"

#include "xng/gpu/renderpipelinedesc.hpp"
#include "xng/gpu/shaderuniformbufferdesc.hpp"
#include "xng/gpu/vertexbufferdesc.hpp"
#include "xng/gpu/rendertargetdesc.hpp"
#include "xng/gpu/texturebufferdesc.hpp"

#include "xng/gpu/vertexarrayobject.hpp"

#include "xng/gpu/videomemory.hpp"

#include "xng/shader/shaderdecompiler.hpp"

#include "xng/render/scene/mesh.hpp"
#include "xng/gpu/renderdeviceinfo.hpp"

namespace xng {
    /**
     * The render device allocates gpu resources.
     *
     * Smart pointers are used as return type to ensure that user programs are memory safe by default,
     * the user can unbox the returned pointers to use different memory management patterns.
     */
    class XENGINE_EXPORT RenderDevice {
    public:
        /**
         * Deallocates all objects belonging to this device which have not been deallocated yet.
         * Deallocating objects returned by this device after calling this destructor results in undefined behaviour.
         */
        virtual ~RenderDevice() = default;

        /**
         * @return The information about this device.
         */
        virtual const RenderDeviceInfo &getInfo() = 0;

        virtual std::vector<std::reference_wrapper<CommandQueue>> getRenderCommandQueues() = 0;

        virtual std::vector<std::reference_wrapper<CommandQueue>> getComputeCommandQueues() = 0;

        virtual std::vector<std::reference_wrapper<CommandQueue>> getTransferCommandQueues() = 0;

        virtual std::unique_ptr<CommandBuffer> createCommandBuffer() = 0;

        virtual std::shared_ptr<CommandSemaphore> createSemaphore() = 0;

        /**
         *
         * @param desc The description of the pipeline
         * @param shader The shader to bind when rendering vertex buffers
         * @param bindings The set of buffers to bind to the shader in the given order when rendering vertex buffers
         * @return
         */
        virtual std::unique_ptr<RenderPipeline> createRenderPipeline(const RenderPipelineDesc &desc,
                                                                     ShaderDecompiler &decompiler) = 0;

        virtual std::unique_ptr<RenderPipeline> createRenderPipeline(const uint8_t *cacheData, size_t size) = 0;

        virtual std::unique_ptr<ComputePipeline> createComputePipeline(const ComputePipelineDesc &desc,
                                                                       ShaderDecompiler &decompiler) = 0;

        virtual std::unique_ptr<RaytracePipeline> createRaytracePipeline(const RaytracePipelineDesc &desc) = 0;

        virtual std::unique_ptr<RenderTarget> createRenderTarget(const RenderTargetDesc &desc) = 0;

        virtual std::unique_ptr<VertexArrayObject> createVertexArrayObject(const VertexArrayObjectDesc &desc) = 0;

        virtual std::unique_ptr<RenderPass> createRenderPass(const RenderPassDesc &desc) = 0;

        /**
         * Create buffer without specifying a backing VideoMemory.
         * The buffer storage is managed by the allocator.
         *
         * @param desc
         * @return
         */
        virtual std::unique_ptr<VertexBuffer> createVertexBuffer(const VertexBufferDesc &desc) = 0;

        virtual std::unique_ptr<IndexBuffer> createIndexBuffer(const IndexBufferDesc &desc) = 0;

        virtual std::unique_ptr<ShaderUniformBuffer> createShaderUniformBuffer(const ShaderUniformBufferDesc &desc) = 0;

        virtual std::unique_ptr<ShaderStorageBuffer> createShaderStorageBuffer(const ShaderStorageBufferDesc &desc) = 0;

        virtual std::unique_ptr<TextureBuffer> createTextureBuffer(const TextureBufferDesc &desc) = 0;

        virtual std::unique_ptr<TextureArrayBuffer> createTextureArrayBuffer(const TextureArrayBufferDesc &desc) = 0;

        /**
         * Allocate a region of memory on the gpu device into which RenderBuffer objects can be mapped.
         *
         * @param desc
         * @return
         */
        virtual std::unique_ptr<VideoMemory> createMemory(const VideoMemoryDesc &desc) = 0;

        virtual void setDebugCallback(const std::function<void(const std::string &)> &callback) = 0;

        /**
         * Return the statistics for the time since the last call to getFrameStats and reset the internal render statistics object
         *
         * @return
         */
        virtual RenderStatistics getFrameStats() = 0;
    };
}

#endif //XENGINE_RENDERDEVICE_HPP
