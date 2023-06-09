/**
 *  xEngine - C++ game engine library
 *  Copyright (C) 2023  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef XENGINE_OGLSHADERSTORAGEBUFFER_HPP
#define XENGINE_OGLSHADERSTORAGEBUFFER_HPP

#include "opengl_include.hpp"

#include "xng/gpu/shaderstoragebuffer.hpp"
#include "gpu/opengl/oglfence.hpp"

namespace xng::opengl {
    class OGLShaderStorageBuffer : public ShaderStorageBuffer {
    public:
        std::function<void(RenderObject * )> destructor;
        ShaderStorageBufferDesc desc;
        GLuint ssbo = 0;

        explicit OGLShaderStorageBuffer(std::function<void(RenderObject * )> destructor,
                                        ShaderStorageBufferDesc inputDescription)
                : destructor(std::move(destructor)), desc(inputDescription) {

            glGenBuffers(1, &ssbo);

            glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
            glBufferData(GL_SHADER_STORAGE_BUFFER,
                         static_cast<GLsizeiptr>(desc.size),
                         nullptr,
                         GL_STATIC_DRAW);

            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
            checkGLError();
        }

        ~OGLShaderStorageBuffer() override {
            glDeleteBuffers(1, &ssbo);
            checkGLError();
            destructor(this);
        }

        const ShaderStorageBufferDesc &getDescription() override {
            return desc;
        }

        void upload(const uint8_t *data, size_t size) override {
            if (size != desc.size)
                throw std::runtime_error("Upload size does not match buffer size");
            if (desc.bufferType != HOST_VISIBLE)
                throw std::runtime_error("Upload called on non host visible buffer.");
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
            glBufferSubData(GL_SHADER_STORAGE_BUFFER,
                            0,
                            static_cast<GLsizeiptr>(size),
                            data);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
            checkGLError();
        }

        void upload(size_t offset, const uint8_t *data, size_t size) override {
            if (size + offset > desc.size)
                throw std::runtime_error("Upload size overflow");
            if (desc.bufferType != HOST_VISIBLE)
                throw std::runtime_error("Upload called on non host visible buffer.");
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
            glBufferSubData(GL_SHADER_STORAGE_BUFFER,
                            static_cast<GLsizeiptr>(offset),
                            static_cast<GLsizeiptr>(size),
                            data);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
            checkGLError();
        }

        std::vector<uint8_t> download(size_t offset, size_t size) override {
            if (size + offset > desc.size)
                throw std::runtime_error("Download size overflow");
            if (desc.bufferType != HOST_VISIBLE)
                throw std::runtime_error("Download called on non host visible buffer.");
            std::vector<uint8_t> ret(size);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
            glGetBufferSubData(GL_SHADER_STORAGE_BUFFER,
                               static_cast<GLintptr>(offset),
                               static_cast<GLsizeiptr>(size),
                               ret.data());
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
            return ret;
        }
    };
}

#endif //XENGINE_OGLSHADERSTORAGEBUFFER_HPP
