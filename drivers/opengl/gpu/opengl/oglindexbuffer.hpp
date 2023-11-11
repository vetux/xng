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
        GLuint EBO = 0;

        IndexBufferDesc desc;

        RenderStatistics &stats;

        explicit OGLIndexBuffer(
                                IndexBufferDesc desc,
                                RenderStatistics &stats)
                :desc(desc),
                  stats(stats){
            glGenBuffers(1, &EBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                         static_cast<long>(desc.size),
                         nullptr,
                         GL_DYNAMIC_COPY);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

            checkGLError();
        }

        ~OGLIndexBuffer() override {
            glDeleteBuffers(1, &EBO);
            checkGLError();
        }

        const IndexBufferDesc &getDescription() override {
            return desc;
        }

        void upload(size_t offset, const uint8_t *data, size_t dataSize) override {
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
            stats.uploadIndex += dataSize;
        }
    };
}
#endif //XENGINE_OGLINDEXBUFFER_HPP
