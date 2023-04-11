/**
 *  xEngine - C++ game engine library
 *  Copyright (C) 2023  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef XENGINE_SHADERSTORAGEBUFFER_HPP
#define XENGINE_SHADERSTORAGEBUFFER_HPP

#include "xng/gpu/renderobject.hpp"
#include "xng/gpu/gpufence.hpp"
#include "xng/gpu/shaderstoragebufferdesc.hpp"

namespace xng {
    /**
     * A bindable fixed size shader storage buffer.
     *      eg GLSL
     *
     *      layout(binding = 0, std140) buffer ShaderStorageBuffer
     *      {
     *         ...
     *      } buf;
     *
     *      or HLSL
     *
     *      StructuredBuffer<Type> buf : register(0);
     *
     *  Can be used for large data up to RenderDeviceInfo.storageBufferMaxSize
     */
    class XENGINE_EXPORT ShaderStorageBuffer : public RenderObject {
    public:
        ~ShaderStorageBuffer() override = default;

        Type getType() override {
            return RENDER_OBJECT_SHADER_STORAGE_BUFFER;
        }

        /**
         * Copy the data in source buffer to this buffer.
         *
         * @param source The concrete type of other must be compatible and have the same properties as this buffer.
         * @return
         */
        virtual std::unique_ptr<GpuFence> copy(ShaderStorageBuffer &source) = 0;

        /**
         * Copy the data in source buffer to this buffer.
         *
         * @param source The concrete type of other must be compatible and have the same properties as this buffer.
         * @return
         */
        virtual std::unique_ptr<GpuFence> copy(ShaderStorageBuffer &source,
                                               size_t readOffset,
                                               size_t writeOffset,
                                               size_t count) = 0;

        /**
         * Upload the given data to the buffer at the given offset.
         *
         * @param offset
         * @param data
         * @param dataSize
         * @return
         */
        virtual std::unique_ptr<GpuFence> upload(size_t offset, const uint8_t *data, size_t dataSize) = 0;

        virtual const ShaderStorageBufferDesc &getDescription() = 0;

        /**
         * Upload the given data to the shader buffer,
         *
         * @param data
         * @param size
         */
        virtual std::unique_ptr<GpuFence> upload(const uint8_t *data, size_t size) = 0;

        template<typename T>
        std::unique_ptr<GpuFence> upload(const T &data) {
            return upload(reinterpret_cast<const uint8_t *>(&data), sizeof(T));
        }
    };
}

#endif //XENGINE_SHADERSTORAGEBUFFER_HPP
