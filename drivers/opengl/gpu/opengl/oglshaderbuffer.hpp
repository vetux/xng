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

#ifndef XENGINE_OGLSHADERBUFFER_HPP
#define XENGINE_OGLSHADERBUFFER_HPP

#include "opengl_include.hpp"
#include "gpu/opengl/oglfence.hpp"

namespace xng::opengl {
    class OGLShaderBuffer : public ShaderBuffer {
    public:
        std::function<void(RenderObject * )> destructor;
        ShaderBufferDesc desc;
        GLuint ubo = 0;

        explicit OGLShaderBuffer(std::function<void(RenderObject * )> destructor,
                                 ShaderBufferDesc inputDescription)
                : destructor(std::move(destructor)), desc(inputDescription) {

            glGenBuffers(1, &ubo);

            glBindBuffer(GL_UNIFORM_BUFFER, ubo);
            glBufferData(GL_UNIFORM_BUFFER,
                         static_cast<GLsizeiptr>(desc.size),
                         nullptr,
                         GL_STATIC_DRAW);

            glBindBuffer(GL_UNIFORM_BUFFER, 0);
            checkGLError();
        }

        ~OGLShaderBuffer() override {
            glDeleteBuffers(1, &ubo);
            checkGLError();
            destructor(this);
        }

        const ShaderBufferDesc &getDescription() override {
            return desc;
        }

        std::unique_ptr<GpuFence> upload(const uint8_t *data, size_t size) override {
            if (size != desc.size)
                throw std::runtime_error("Upload size does not match buffer size");
            if (desc.bufferType != HOST_VISIBLE)
                throw std::runtime_error("Upload called on non host visible buffer.");
            glBindBuffer(GL_UNIFORM_BUFFER, ubo);
            glBufferData(GL_UNIFORM_BUFFER, numeric_cast<GLsizeiptr>(size), data, GL_STATIC_DRAW);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
            checkGLError();
            return std::make_unique<OGLFence>();
        }

        std::unique_ptr<GpuFence> copy(RenderBuffer &source) override {
            return copy(source, 0, 0, source.getSize());
        }

        std::unique_ptr<GpuFence> copy(RenderBuffer &other,
                                       size_t readOffset,
                                       size_t writeOffset,
                                       size_t count) override {
            auto &source = dynamic_cast<OGLShaderBuffer &>(other);
            if (readOffset >= source.desc.size
                || readOffset + count >= source.desc.size
                || writeOffset >= desc.size
                || writeOffset + count >= desc.size) {
                throw std::runtime_error("Invalid copy range");
            }
            glBindBuffer(GL_COPY_WRITE_BUFFER, ubo);
            glBindBuffer(GL_COPY_READ_BUFFER, source.ubo);
            glCopyBufferSubData(GL_COPY_READ_BUFFER,
                                GL_COPY_WRITE_BUFFER,
                                static_cast<GLintptr>(readOffset),
                                static_cast<GLintptr>(writeOffset),
                                numeric_cast<GLsizeiptr>(count));
            glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
            glBindBuffer(GL_COPY_READ_BUFFER, 0);
            checkGLError();
            return std::make_unique<OGLFence>();
        }

        size_t getSize() override {
            return desc.size;
        }
    };
}

#endif //XENGINE_OGLSHADERBUFFER_HPP
