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

#ifndef XENGINE_VERTEXBUFFER_HPP
#define XENGINE_VERTEXBUFFER_HPP

#include "renderbuffer.hpp"
#include "vertexbufferdesc.hpp"
#include "gpufence.hpp"

#include "xng/asset/mesh.hpp"

#include "xng/math/transform.hpp"

namespace xng {
    class XENGINE_EXPORT VertexBuffer : public RenderBuffer {
    public:
        ~VertexBuffer() override = default;

        virtual const VertexBufferDesc &getDescription() = 0;

        RenderBufferType getBufferType() override {
            return getDescription().bufferType;
        }

        Type getType() override {
            return Type::VERTEX_BUFFER;
        }

        /**
         * Upload the data from the specified buffers to the mesh buffer,
         * the layout of the passed buffers must match the layout specified in the mesh buffer description.
         *
         * @param vertexBuffer
         * @param instanceBuffer
         * @param indices
         */
        virtual std::unique_ptr<GpuFence> upload(const uint8_t *vertexBuffer,
                                                 size_t vertexBufferSize,
                                                 const std::vector<unsigned int> &indices,
                                                 const uint8_t *instanceBuffer,
                                                 size_t instanceBufferSize) = 0;

        virtual std::unique_ptr<GpuFence> upload(const Mesh &mesh) {
            return upload(reinterpret_cast<const uint8_t *>(mesh.vertices.data()),
                          sizeof(Vertex) * mesh.vertices.size(),
                          mesh.indices,
                          nullptr,
                          0);
        }

        virtual std::unique_ptr<GpuFence> upload(const Mesh &mesh,
                                                 const std::vector<Mat4f> &offsets) {
            return upload(reinterpret_cast<const uint8_t *>(mesh.vertices.data()),
                          sizeof(Vertex) * mesh.vertices.size(),
                          mesh.indices,
                          reinterpret_cast<const uint8_t *>(offsets.data()),
                          sizeof(Mat4f) * offsets.size());
        }
    };
}

#endif //XENGINE_VERTEXBUFFER_HPP
