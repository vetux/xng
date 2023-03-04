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

#ifndef XENGINE_VERTEXBUFFER_HPP
#define XENGINE_VERTEXBUFFER_HPP

#include "renderbuffer.hpp"
#include "vertexbufferdesc.hpp"
#include "gpufence.hpp"

#include "xng/asset/mesh.hpp"

#include "xng/math/transform.hpp"

namespace xng {
    /**
     * A VertexBuffer represents a buffer for storing vertex data.
     *
     * On OpenGL this would be a GL_ARRAY_BUFFER
     */
    class XENGINE_EXPORT VertexBuffer : public RenderBuffer {
    public:
        ~VertexBuffer() override = default;

        size_t getSize() override {
            return getDescription().size;
        }

        RenderBufferType getBufferType() override {
            return getDescription().bufferType;
        }

        Type getType() override {
            return Type::RENDER_OBJECT_VERTEX_BUFFER;
        }

        virtual const VertexBufferDesc &getDescription() = 0;

        virtual std::unique_ptr<GpuFence> upload(size_t offset,
                                                 const uint8_t *data,
                                                 size_t dataSize) = 0;
    };
}

#endif //XENGINE_VERTEXBUFFER_HPP
