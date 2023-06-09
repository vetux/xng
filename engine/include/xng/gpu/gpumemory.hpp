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

#ifndef XENGINE_GPUMEMORY_HPP
#define XENGINE_GPUMEMORY_HPP

#include "xng/gpu/renderobject.hpp"
#include "xng/gpu/gpumemorydesc.hpp"

#include "xng/gpu/indexbuffer.hpp"
#include "xng/gpu/vertexbuffer.hpp"
#include "xng/gpu/shaderuniformbuffer.hpp"
#include "xng/gpu/texturebuffer.hpp"
#include "xng/gpu/texturearraybuffer.hpp"

namespace xng {
    /**
     * A GpuMemory object represents some region of memory on a gpu device which can be used as backing memory of RenderBuffer objects.
     *
     * The interface offers control over where the backing memory of RenderBuffer objects is located,
     * however if RenderDeviceCapability::RENDER_GPU_MEMORY is not supported the interface behaves as if there
     * is control over the memory locations but does not actually use the values specified by the user.
     *
     * Multiple objects can be allocated inside a single GpuMemory object to implement custom memory management of RenderBuffer objects.
     * This custom memory management scheme would only have an effect if the RENDER_GPU_MEMORY capability is supported
     * and otherwise only incurs additional overhead.
     */
    class GpuMemory : public RenderObject {
    public:
        Type getType() override {
            return RENDER_OBJECT_MEMORY;
        }

        virtual GpuMemoryDesc getDescription() = 0;

        /**
         * @param desc
         * @return The required size in bytes of the hardware buffer for the given description
         */
        virtual size_t getRequiredBufferSize(const VertexBufferDesc &desc) = 0;

        /**
         * @param desc
         * @return The required size in bytes of the hardware buffer for the given description
         */
        virtual size_t getRequiredBufferSize(const IndexBufferDesc &desc) = 0;

        /**
         * @param desc
         * @return The required size in bytes of the hardware buffer for the given description
         */
        virtual size_t getRequiredBufferSize(const ShaderUniformBufferDesc &desc) = 0;

        /**
         * @param desc
         * @return The required size in bytes of the hardware buffer for the given description
         */
        virtual size_t getRequiredBufferSize(const TextureBufferDesc &desc) = 0;

        /**
         * @param desc
         * @return The required size in bytes of the hardware buffer for the given description
         */
        virtual size_t getRequiredBufferSize(const TextureArrayBufferDesc &desc) = 0;

        /**
         * Allocate the buffer in the specified memory object at the specified offset.
         *
         * If objects memory overlaps and RenderDeviceCapability::RENDER_GPU_MEMORY is not available the
         * objects memory will not actually be overlapping and instead be separate objects.
         *
         * @param desc
         * @param memory
         * @param offset
         * @return
         */
        virtual std::unique_ptr<VertexBuffer> createVertexBuffer(const VertexBufferDesc &desc, size_t offset) = 0;

        virtual std::unique_ptr<IndexBuffer> createIndexBuffer(const IndexBufferDesc &desc, size_t offset) = 0;

        virtual std::unique_ptr<ShaderUniformBuffer> createShaderBuffer(const ShaderUniformBufferDesc &desc, size_t offset) = 0;

        virtual std::unique_ptr<TextureBuffer> createTextureBuffer(const TextureBufferDesc &desc, size_t offset) = 0;

        virtual std::unique_ptr<TextureArrayBuffer> createTextureArrayBuffer(const TextureArrayBufferDesc &desc,
                                                                             size_t offset) = 0;
    };
}

#endif //XENGINE_GPUMEMORY_HPP
