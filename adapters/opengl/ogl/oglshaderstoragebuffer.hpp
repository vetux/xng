/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2023  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 3 of the License, or (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.

 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef XENGINE_OGLSHADERSTORAGEBUFFER_HPP
#define XENGINE_OGLSHADERSTORAGEBUFFER_HPP

#include "glad/glad.h"

struct OGLShaderStorageBuffer {
    GLuint SSBO = 0;

    explicit OGLShaderStorageBuffer(size_t size) {
        glGenBuffers(1, &SSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, static_cast<GLsizeiptr>(size), nullptr, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        oglCheckError();
    }
};

#endif //XENGINE_OGLSHADERSTORAGEBUFFER_HPP
