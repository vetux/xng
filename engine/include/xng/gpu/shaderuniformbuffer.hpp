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

#ifndef XENGINE_SHADERUNIFORMBUFFER_HPP
#define XENGINE_SHADERUNIFORMBUFFER_HPP

#include "xng/gpu/command.hpp"

#include "xng/gpu/shaderuniformbufferdesc.hpp"

namespace xng {
    /**
     * A bindable fixed size shader uniform buffer.
     *      eg GLSL
     *
     *      layout(binding = 0, std140) uniform ShaderUniformBuffer
     *      {
     *         ...
     *      };
     *
     *      or HLSL
     *
     *      cbuffer ShaderUniformBuffer : register(b0)
     *      {
     *          ...
     *      };
     *
     * Should only be used for very small data because it must fit in directx constant buffers (4096 vectors with 32bit components)
     * as well as opengl uniform buffers (16kb)
     */
    class XENGINE_EXPORT ShaderUniformBuffer : public RenderObject {
    public:
        ~ShaderUniformBuffer() override = default;

        Type getType() override {
            return RENDER_OBJECT_SHADER_UNIFORM_BUFFER;
        }

        virtual const ShaderUniformBufferDesc &getDescription() = 0;

        /**
         * Copy the data in source buffer to this buffer.
         *
         * @param source The concrete type of other must be compatible and have the same properties as this buffer.
         * @return
         */
        Command copy(ShaderUniformBuffer &source) {
            return copy(source, 0, 0, source.getDescription().size);
        }

        /**
         * Copy the data in source buffer to this buffer.
         *
         * @param source The concrete type of other must be compatible and have the same properties as this buffer.
         * @return
         */
        Command copy(ShaderUniformBuffer &source,
                     size_t readOffset,
                     size_t writeOffset,
                     size_t count) {
            return {Command::COPY_SHADER_UNIFORM_BUFFER,
                    ShaderUniformBufferCopy(&source, this, readOffset, writeOffset, count)};
        }

        /**
         * Upload the given data to the buffer at the given offset.
         *
         * @param offset
         * @param data
         * @param dataSize
         * @return
         */
        virtual void upload(size_t offset, const uint8_t *data, size_t dataSize) = 0;

        /**
         * Upload the given data to the shader buffer,
         *
         * @param data
         * @param size must match the size specified at creation of the shader buffer
         */
        virtual void upload(const uint8_t *data, size_t size) = 0;

        template<typename T>
        void upload(const T &data) {
            return upload(reinterpret_cast<const uint8_t *>(&data), sizeof(T));
        }
    };
}

#endif //XENGINE_SHADERUNIFORMBUFFER_HPP
