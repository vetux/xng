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

#ifndef XENGINE_OGLINDEXBUFFER_HPP
#define XENGINE_OGLINDEXBUFFER_HPP

#include <cstddef>

#include "glad/glad.h"

struct OGLIndexBuffer {
    GLuint EBO = 0;

    size_t size = 0;

    explicit OGLIndexBuffer(size_t size)
        : size(size) {
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     static_cast<GLsizeiptr>(size),
                     nullptr,
                     GL_DYNAMIC_COPY);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        oglCheckError();
    }

    ~OGLIndexBuffer() {
        glDeleteBuffers(1, &EBO);
        oglCheckError();
    }
};

#endif //XENGINE_OGLINDEXBUFFER_HPP
