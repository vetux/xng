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

#ifndef XENGINE_VERTEXBUFFER_HPP
#define XENGINE_VERTEXBUFFER_HPP

#include "xng/gpu/vertexbufferdesc.hpp"
#include "xng/gpu/commandfence.hpp"

#include "xng/asset/mesh.hpp"

#include "xng/math/transform.hpp"

namespace xng {
    /**
     * A VertexBuffer represents a buffer for storing vertex data.
     *
     * On OpenGL this would be a GL_ARRAY_BUFFER
     */
    class XENGINE_EXPORT VertexBuffer : public RenderObject {
    public:
        ~VertexBuffer() override = default;

        Type getType() override {
            return Type::RENDER_OBJECT_VERTEX_BUFFER;
        }

        virtual const VertexBufferDesc &getDescription() = 0;

        /**
         * Copy the data in source buffer to this buffer.
         *
         * @param source The concrete type of other must be compatible and have the same properties as this buffer.
         * @return
         */
        Command copy(VertexBuffer &source) {
            return copy(source, 0, 0, source.getDescription().size);
        }

        /**
         * Copy the data in source buffer to this buffer.
         *
         * @param source The concrete type of other must be compatible and have the same properties as this buffer.
         * @return
         */
        Command copy(VertexBuffer &source,
                     size_t readOffset,
                     size_t writeOffset,
                     size_t count) {
            return {Command::COPY_VERTEX_BUFFER,
                    VertexBufferCopy(&source, this, readOffset, writeOffset, count)};
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
    };
}

#endif //XENGINE_VERTEXBUFFER_HPP
