/**
 *   xEngine - C++ Game Engine Library
 *   Copyright (C) 2026 Julia Zampiccoli
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the Lesser General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef XENGINE_BUFFERGL_HPP
#define XENGINE_BUFFERGL_HPP

#include "glad/glad.h"
#include "ogldebug.hpp"

#include "xng/rendergraph/resource/buffer.hpp"

namespace xng::opengl {
    class BufferGL {
    public:
        GLuint handle{};

        GLenum target{};

        rg::Buffer desc;

        explicit BufferGL(const rg::Buffer bufferDesc)
            : desc(bufferDesc) {
            if (desc.capabilityFlags & Buffer::CAPABILITY_STORAGE) {
                target = GL_SHADER_STORAGE_BUFFER;
            } else if (desc.capabilityFlags & Buffer::CAPABILITY_VERTEX) {
                target = GL_ARRAY_BUFFER;
            } else if (desc.capabilityFlags & Buffer::CAPABILITY_INDEX) {
                target = GL_ELEMENT_ARRAY_BUFFER;
            } else {
                target = GL_COPY_WRITE_BUFFER;
            }

            glGenBuffers(1, &handle);
            glBindBuffer(target, handle);

            // Because the transfer context must be able to map any buffer because of
            // the row inversion in copyBufferToTexture / copyTextureToBuffer all buffers must have mapping bits set.
            glBufferStorage(target,
                            static_cast<GLsizeiptr>(desc.size),
                            nullptr,
                            GL_MAP_READ_BIT
                            | GL_MAP_WRITE_BIT
                            | GL_MAP_PERSISTENT_BIT
                            | GL_MAP_COHERENT_BIT
                            | GL_DYNAMIC_STORAGE_BIT);
            /*
                        switch (desc.memoryType) {
                            case Buffer::MEMORY_GPU_ONLY:
                                glBufferStorage(target,
                                                static_cast<GLsizeiptr>(desc.size),
                                                nullptr,
                                                GL_DYNAMIC_STORAGE_BIT);
                                break;
                            case Buffer::MEMORY_CPU_TO_GPU:
                                glBufferStorage(target,
                                                static_cast<GLsizeiptr>(desc.size),
                                                nullptr,
                                                GL_MAP_WRITE_BIT
                                                | GL_MAP_PERSISTENT_BIT
                                                | GL_MAP_COHERENT_BIT
                                                | GL_DYNAMIC_STORAGE_BIT);
                                break;
                            case Buffer::MEMORY_GPU_TO_CPU:
                                glBufferStorage(target,
                                                static_cast<GLsizeiptr>(desc.size),
                                                nullptr,
                                                GL_MAP_READ_BIT
                                                | GL_MAP_PERSISTENT_BIT
                                                | GL_MAP_COHERENT_BIT
                                                | GL_DYNAMIC_STORAGE_BIT);
                                break;
                        }
            */
            glBindBuffer(target, 0);

            oglCheckError();
        }

        ~BufferGL() {
            glDeleteBuffers(1, &handle);
            oglCheckError();
        }

        [[nodiscard]] uint8_t *map() const {
            glBindBuffer(target, handle);

            GLenum access = GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT | GL_MAP_WRITE_BIT | GL_MAP_READ_BIT;
            /* if (desc.memoryType == Buffer::MEMORY_GPU_TO_CPU) {
                 access |= GL_MAP_READ_BIT;
             } else if (desc.memoryType == Buffer::MEMORY_CPU_TO_GPU) {
                 access |= GL_MAP_WRITE_BIT;
             }*/

            const auto ret = glMapBufferRange(target,
                                              0,
                                              static_cast<GLsizeiptr>(desc.size),
                                              access);

            glBindBuffer(target, 0);

            oglCheckError();

            return static_cast<uint8_t *>(ret);
        }

        void unmap() const {
            glBindBuffer(target, handle);
            glUnmapBuffer(target);
            glBindBuffer(target, 0);
            oglCheckError();
        }
    };
}

#endif //XENGINE_BUFFERGL_HPP
