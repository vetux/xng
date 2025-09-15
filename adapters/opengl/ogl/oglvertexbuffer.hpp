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

#ifndef XENGINE_OGLVERTEXBUFFER_HPP
#define XENGINE_OGLVERTEXBUFFER_HPP

#include <cstddef>

#include "glad/glad.h"

struct OGLVertexBuffer {
    GLuint VBO = 0;

    size_t size = 0;

    explicit OGLVertexBuffer(size_t size)
        : size(size) {
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(size), nullptr, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        oglCheckError();
    }

    ~OGLVertexBuffer() {
        glDeleteBuffers(1, &VBO);
        oglCheckError();
    }
};

#endif //XENGINE_OGLVERTEXBUFFER_HPP
