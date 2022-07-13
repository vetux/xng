/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2021  Julian Zampiccoli
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

#include "gpu/opengl/oglbuildmacro.hpp"

namespace xng::opengl {
    class OPENGL_TYPENAME(ShaderBuffer) : public ShaderBuffer OPENGL_INHERIT {
    public:
        ShaderBufferDesc desc;
        GLuint ubo;

        explicit OPENGL_TYPENAME(ShaderBuffer)(ShaderBufferDesc inputDescription)
                : desc(std::move(inputDescription)) {
            initialize();
            glGenBuffers(1, &ubo);
            glBindBuffer(GL_UNIFORM_BUFFER, ubo);
            glBufferData(GL_UNIFORM_BUFFER, numeric_cast<GLsizeiptr>(desc.size), NULL, GL_STATIC_DRAW);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
        }

        ~OPENGL_TYPENAME(ShaderBuffer)() override {
            glDeleteBuffers(1, &ubo);
        }

        void pinGpuMemory() override {}

        void unpinGpuMemory() override {}

        const ShaderBufferDesc &getDescription() override {
            return desc;
        }

        void upload(const uint8_t *data, size_t size) override {
            if (size != desc.size)
                throw std::runtime_error("Upload size does not match buffer size");
            glBindBuffer(GL_UNIFORM_BUFFER, ubo);
            glBufferData(GL_UNIFORM_BUFFER, numeric_cast<GLsizeiptr>(size), data, GL_STATIC_DRAW);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
        }

        OPENGL_MEMBERS
    };
}

#endif //XENGINE_OGLSHADERBUFFER_HPP
