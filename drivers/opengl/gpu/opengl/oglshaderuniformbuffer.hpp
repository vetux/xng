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

#ifndef XENGINE_OGLSHADERUNIFORMBUFFER_HPP
#define XENGINE_OGLSHADERUNIFORMBUFFER_HPP

#include "opengl_include.hpp"
#include "gpu/opengl/oglfence.hpp"

namespace xng::opengl {
    class OGLShaderUniformBuffer : public ShaderUniformBuffer {
    public:
        std::function<void(RenderObject * )> destructor;
        ShaderUniformBufferDesc desc;
        GLuint ubo = 0;

        explicit OGLShaderUniformBuffer(std::function<void(RenderObject * )> destructor,
                                        ShaderUniformBufferDesc inputDescription)
                : destructor(std::move(destructor)), desc(inputDescription) {

            glGenBuffers(1, &ubo);

            glBindBuffer(GL_UNIFORM_BUFFER, ubo);
            glBufferData(GL_UNIFORM_BUFFER,
                         static_cast<GLsizeiptr>(desc.size),
                         nullptr,
                         GL_DYNAMIC_DRAW);

            glBindBuffer(GL_UNIFORM_BUFFER, 0);
            checkGLError();
        }

        ~OGLShaderUniformBuffer() override {
            glDeleteBuffers(1, &ubo);
            checkGLError();
            destructor(this);
        }

        const ShaderUniformBufferDesc &getDescription() override {
            return desc;
        }

        void upload(const uint8_t *data, size_t size) override {
            if (size != desc.size)
                throw std::runtime_error("Upload size does not match buffer size");
            if (desc.bufferType != HOST_VISIBLE)
                throw std::runtime_error("Upload called on non host visible buffer.");
            glBindBuffer(GL_UNIFORM_BUFFER, ubo);
            glBufferSubData(GL_UNIFORM_BUFFER, 0, static_cast<GLsizeiptr>(size), data);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
            checkGLError();
        }

        void upload(size_t offset, const uint8_t *data, size_t dataSize) override {
            if (dataSize + offset > desc.size)
                throw std::runtime_error("Upload size overflow");
            if (desc.bufferType != HOST_VISIBLE)
                throw std::runtime_error("Upload called on non host visible buffer.");
            glBindBuffer(GL_UNIFORM_BUFFER, ubo);
            glBufferSubData(GL_UNIFORM_BUFFER,
                            static_cast<GLsizeiptr>(offset),
                            static_cast<GLsizeiptr>(dataSize),
                            data);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
            checkGLError();
        }
    };
}

#endif //XENGINE_OGLSHADERUNIFORMBUFFER_HPP
