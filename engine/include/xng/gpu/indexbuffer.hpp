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

#ifndef XENGINE_INDEXBUFFER_HPP
#define XENGINE_INDEXBUFFER_HPP

#include "xng/gpu/renderbuffer.hpp"

#include "xng/gpu/indexbufferdesc.hpp"

namespace xng {
    /**
     * IndexBuffer represents a buffer for storing indices.
     *
     * On OpenGL this woule be a GL_ELEMENT_ARRAY_BUFFER
     */
    class IndexBuffer : public RenderBuffer {
    public:
        enum IndexType {
            UNSIGNED_INT
        };

        size_t getMemoryUsage() override {
            return getDescription().size;
        }

        RenderBufferType getBufferType() override {
            return getDescription().type;
        }

        Type getType() override {
            return RENDER_OBJECT_INDEX_BUFFER;
        }

        virtual const IndexBufferDesc &getDescription() = 0;

        virtual std::unique_ptr<GpuFence> upload(size_t offset,
                                                 const uint8_t *data,
                                                 size_t dataSize) = 0;
    };
}
#endif //XENGINE_INDEXBUFFER_HPP
