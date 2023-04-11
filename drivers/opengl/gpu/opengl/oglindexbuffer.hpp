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

#ifndef XENGINE_OGLINDEXBUFFER_HPP
#define XENGINE_OGLINDEXBUFFER_HPP

#include "xng/gpu/indexbuffer.hpp"

#include <utility>

#include "opengl_include.hpp"
#include "opengl_checkerror.hpp"

#include "gpu/opengl/oglfence.hpp"

namespace xng::opengl {
    class OGLIndexBuffer : public IndexBuffer {
    public:
        std::function<void(RenderObject * )> destructor;

        GLuint EBO = 0;

        IndexBufferDesc desc;

        explicit OGLIndexBuffer(std::function<void(RenderObject * )> destructor,
                                IndexBufferDesc desc)
                : destructor(std::move(destructor)),
                  desc(desc) {
            glGenBuffers(1, &EBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                         static_cast<long>(desc.size),
                         nullptr,
                         GL_STATIC_COPY);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

            checkGLError();
        }

        ~OGLIndexBuffer() override {
            glDeleteBuffers(1, &EBO);
            checkGLError();

            destructor(this);
        }

        std::unique_ptr<GpuFence> copy(IndexBuffer &source) override {
            return copy(source, 0, 0, source.getDescription().size);
        }

        std::unique_ptr<GpuFence> copy(IndexBuffer &source,
                                       size_t readOffset,
                                       size_t writeOffset,
                                       size_t count) override {
            auto buf = dynamic_cast<OGLIndexBuffer &>(source);
            if (readOffset >= buf.desc.size
                || readOffset + count > buf.desc.size
                || writeOffset >= desc.size
                || writeOffset + count > desc.size) {
                throw std::runtime_error("Invalid copy range");
            }
            glBindBuffer(GL_COPY_READ_BUFFER, buf.EBO);
            glBindBuffer(GL_COPY_WRITE_BUFFER, EBO);
            glCopyBufferSubData(GL_COPY_READ_BUFFER,
                                GL_COPY_WRITE_BUFFER,
                                static_cast<GLintptr>(readOffset),
                                static_cast<GLintptr>(writeOffset),
                                static_cast<GLsizeiptr>(count));
            glBindBuffer(GL_COPY_READ_BUFFER, 0);
            glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
            checkGLError();
            return std::make_unique<OGLFence>();
        }

        const IndexBufferDesc &getDescription() override {
            return desc;
        }

        std::unique_ptr<GpuFence> upload(size_t offset, const uint8_t *data, size_t dataSize) override {
            if (offset >= desc.size
                || offset + dataSize > desc.size) {
                throw std::runtime_error("Invalid upload range");
            }
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,
                            static_cast<GLintptr>(offset),
                            static_cast<GLsizeiptr>(dataSize),
                            data);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            checkGLError();
            return std::make_unique<OGLFence>();
        }
    };
}
#endif //XENGINE_OGLINDEXBUFFER_HPP
