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

#ifndef XENGINE_OGLVERTEXBUFFER_HPP
#define XENGINE_OGLVERTEXBUFFER_HPP

#include "xng/gpu/vertexbuffer.hpp"

#include <utility>

#include "opengl_include.hpp"
#include "gpu/opengl/oglfence.hpp"

#include "xng/render/mesh.hpp"

#include "xng/math/matrixmath.hpp"

namespace xng::opengl {
    class OGLVertexBuffer : public VertexBuffer {
    public:
        VertexBufferDesc desc;

        GLuint VBO = 0;

        RenderStatistics &stats;

        explicit OGLVertexBuffer(VertexBufferDesc desc,
                                 RenderStatistics &stats)
                : desc(desc),
                  stats(stats) {
            glGenBuffers(1, &VBO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER,
                         (long) desc.size,
                         nullptr,
                         GL_DYNAMIC_COPY);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            checkGLError();
        }

        ~OGLVertexBuffer() override {
            glDeleteBuffers(1, &VBO);
        }

        const VertexBufferDesc &getDescription() override {
            return desc;
        }

        void upload(size_t offset, const uint8_t *data, size_t dataSize) override {
            if (offset >= desc.size
                || offset + dataSize > desc.size) {
                throw std::runtime_error("Invalid upload range");
            }
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferSubData(GL_ARRAY_BUFFER,
                            static_cast<GLintptr>(offset),
                            static_cast<GLsizeiptr>(dataSize),
                            data);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            checkGLError();
            stats.uploadVertex += dataSize;
        }
    };
}

#endif //XENGINE_OGLVERTEXBUFFER_HPP
