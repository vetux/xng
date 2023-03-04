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

#ifndef XENGINE_OGLVERTEXBUFFER_HPP
#define XENGINE_OGLVERTEXBUFFER_HPP

#include "xng/gpu/vertexbuffer.hpp"

#include <utility>

#include "opengl_include.hpp"
#include "gpu/opengl/oglfence.hpp"

#include "xng/asset/mesh.hpp"

#include "xng/math/matrixmath.hpp"

namespace xng::opengl {
    class OGLVertexBuffer : public VertexBuffer {
    public:
        std::function<void(RenderObject * )> destructor;

        VertexBufferDesc desc;

        GLuint VBO = 0;

        explicit OGLVertexBuffer(std::function<void(RenderObject * )> destructor,
                                 VertexBufferDesc desc)
                : destructor(std::move(destructor)),
                  desc(desc) {
            glGenBuffers(1, &VBO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER,
                         (long) desc.size,
                         nullptr,
                         GL_STATIC_COPY);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            checkGLError();
        }

        ~OGLVertexBuffer() override {
            glDeleteBuffers(1, &VBO);
            destructor(this);
        }

        std::unique_ptr<GpuFence> copy(RenderBuffer &source) override {
            return copy(source, 0, 0, source.getSize());
        }

        std::unique_ptr<GpuFence> copy(RenderBuffer &source,
                                       size_t readOffset,
                                       size_t writeOffset,
                                       size_t count) override {
            auto buf = dynamic_cast<OGLVertexBuffer &>(source);
            if (readOffset >= buf.desc.size
                || readOffset + count >= buf.desc.size
                || writeOffset >= desc.size
                || writeOffset + count >= desc.size) {
                throw std::runtime_error("Invalid copy range");
            }
            glBindBuffer(GL_COPY_READ_BUFFER, buf.VBO);
            glBindBuffer(GL_COPY_WRITE_BUFFER, VBO);
            glCopyBufferSubData(GL_COPY_READ_BUFFER,
                                GL_COPY_WRITE_BUFFER,
                                static_cast<GLintptr>(readOffset),
                                static_cast<GLintptr>(writeOffset),
                                static_cast<GLsizeiptr>(count));
            glBindBuffer(GL_COPY_READ_BUFFER, 0);
            glBindBuffer(GL_COPY_WRITE_BUFFER, 0);

            return std::make_unique<OGLFence>();
        }

        const VertexBufferDesc &getDescription() override {
            return desc;
        }

        std::unique_ptr<GpuFence> upload(size_t offset, const uint8_t *data, size_t dataSize) override {
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
            return std::make_unique<OGLFence>();
        }
    };
}

#endif //XENGINE_OGLVERTEXBUFFER_HPP
