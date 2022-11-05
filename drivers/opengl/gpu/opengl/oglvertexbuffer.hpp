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

#ifndef XENGINE_OGLVERTEXBUFFER_HPP
#define XENGINE_OGLVERTEXBUFFER_HPP

#include "xng/gpu/vertexbuffer.hpp"

#include <utility>

#include "opengl_include.hpp"
#include "gpu/opengl/oglfence.hpp"

#include "xng/asset/mesh.hpp"

#include "xng/math/matrixmath.hpp"

namespace xng::opengl {
    class OGLVertexBuffer : public VertexBuffer {
    public:
        std::function<void(RenderObject*)> destructor;

        VertexBufferDesc desc;

        GLuint VAO = 0;
        GLuint VBO = 0;
        GLuint EBO = 0;
        GLuint instanceVBO = 0;

        size_t elementCount = 0;
        GLuint elementType = 0;

        GLsizei vertexSize = 0;
        GLsizeiptr vertexBufferSize = 0;

        GLsizei indexSize = 0;
        GLsizeiptr indexBufferSize = 0;

        GLsizei instanceSize = 0;
        GLsizeiptr instanceBufferSize = 0;

        int usage = GL_STATIC_COPY;

        bool indexed = false;
        bool instanced = false;

        OGLVertexBuffer(std::function<void(RenderObject*)> destructor, VertexBufferDesc inputDescription)
                : destructor(std::move(destructor)), desc(std::move(inputDescription)) {
            indexed = desc.numberOfIndices != 0;
            instanced = desc.numberOfInstances != 0;
            if (indexed) {
                elementCount = desc.numberOfIndices;
            } else {
                elementCount = desc.numberOfVertices;
            }
            switch (desc.primitive) {
                case POINT:
                    elementType = GL_POINTS;
                    break;
                case LINE:
                    elementType = GL_LINES;
                    break;
                case TRI:
                    elementType = GL_TRIANGLES;
                    break;
                case QUAD:
                    elementType = GL_QUADS;
                    break;
                default:
                    throw std::runtime_error("Invalid primitive");
            }
            checkGLError();
            glGenVertexArrays(1, &VAO);
            checkGLError();
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);
            if (instanced) {
                glGenBuffers(1, &instanceVBO);
            }
            checkGLError();
        }

        OGLVertexBuffer(const OGLVertexBuffer &copy) = delete;

        OGLVertexBuffer &operator=(const OGLVertexBuffer &copy) = delete;

        ~OGLVertexBuffer() override {
            glDeleteVertexArrays(1, &VAO);
            glDeleteBuffers(1, &VBO);
            glDeleteBuffers(1, &EBO);
            if (instanced) {
                glDeleteBuffers(1, &instanceVBO);
            }
            destructor(this);
        }

        const VertexBufferDesc &getDescription() override {
            return desc;
        }

        std::unique_ptr<GpuFence> upload(const uint8_t *vertexBuffer,
                                         size_t inputVertexBufferSize,
                                         const std::vector<unsigned int> &indices,
                                         const uint8_t *instanceBuffer,
                                         size_t inputInstanceBufferSize) override {
            if (desc.bufferType != HOST_VISIBLE) {
                throw std::runtime_error("Upload called on non host visible buffer.");
            }

            vertexSize = 0;
            for (auto &binding: desc.vertexLayout) {
                vertexSize += binding.stride();
            }

            instanceSize = 0;
            for (auto &binding: desc.instanceLayout) {
                instanceSize += binding.stride();
            }

            indexSize = sizeof(unsigned int);

            vertexBufferSize = vertexSize * numeric_cast<GLsizei>(desc.numberOfVertices);
            instanceBufferSize = instanceSize * numeric_cast<GLsizei>(desc.numberOfInstances);
            indexBufferSize = indexSize * numeric_cast<GLsizei>(desc.numberOfIndices);

            if (inputVertexBufferSize != vertexBufferSize) {
                throw std::runtime_error("Invalid vertex buffer size");
            } else if (inputInstanceBufferSize != instanceBufferSize) {
                throw std::runtime_error("Invalid instance buffer size");
            } else if (indices.size() != desc.numberOfIndices) {
                throw std::runtime_error("Invalid index buffer size");
            }

            checkGLError();
            glBindVertexArray(VAO);
            checkGLError();

            usage = GL_STATIC_COPY;

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER,
                         vertexBufferSize,
                         vertexBuffer,
                         usage);

            if (indexed) {
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                             indexBufferSize,
                             indices.data(),
                             usage);
            }
            checkGLError();

            for (int i = 0; i < desc.vertexLayout.size(); i++) {
                glEnableVertexAttribArray(i);
            }

            for (int i = 0; i < desc.instanceLayout.size(); i++) {
                glEnableVertexAttribArray(desc.vertexLayout.size() + i);
            }
            checkGLError();

            size_t currentOffset = 0;
            for (int i = 0; i < desc.vertexLayout.size(); i++) {
                auto &binding = desc.vertexLayout.at(i);
                glVertexAttribPointer(i,
                                      VertexAttribute::getCount(binding.type),
                                      getType(binding.component),
                                      GL_FALSE,
                                      vertexSize,
                                      (void *) (currentOffset));
                currentOffset += binding.stride();
            }
            checkGLError();

            if (instanced) {
                currentOffset = 0;

                glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
                glBufferData(GL_ARRAY_BUFFER,
                             instanceBufferSize,
                             instanceBuffer,
                             usage);

                for (int i = 0; i < desc.instanceLayout.size(); i++) {
                    auto &binding = desc.instanceLayout.at(i);
                    auto index = desc.vertexLayout.size() + i;
                    glVertexAttribPointer(index,
                                          VertexAttribute::getCount(binding.type),
                                          getType(binding.component),
                                          GL_FALSE,
                                          instanceSize,
                                          (void *) currentOffset);
                    currentOffset += binding.stride();
                }

                for (int i = 0; i < desc.instanceLayout.size(); i++) {
                    glVertexAttribDivisor(desc.vertexLayout.size() + i, 1);
                }
            }
            checkGLError();

            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            if (indexed) {
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            }

            checkGLError();

            return std::make_unique<OGLFence>();
        }

        static GLenum getType(VertexAttribute::Component c) {
            switch (c) {
                case VertexAttribute::UNSIGNED_BYTE:
                    return GL_UNSIGNED_BYTE;
                case VertexAttribute::SIGNED_BYTE:
                    return GL_BYTE;
                case VertexAttribute::UNSIGNED_INT:
                    return GL_UNSIGNED_INT;
                case VertexAttribute::SIGNED_INT:
                    return GL_INT;
                case VertexAttribute::FLOAT:
                    return GL_FLOAT;
                case VertexAttribute::DOUBLE:
                    return GL_DOUBLE;
                default:
                    throw std::runtime_error("Invalid component");
            }
        }

        std::unique_ptr<GpuFence> copy(RenderBuffer &other) override {
            auto &source = dynamic_cast<OGLVertexBuffer &>(other);
            if (source.vertexSize == 0) {
                throw std::runtime_error("Attempt to read from uninitialized vertex buffer");
            }
            if (vertexSize == 0
                || vertexBufferSize != source.vertexBufferSize
                || indexBufferSize != source.indexBufferSize
                || instanceBufferSize != source.instanceBufferSize) {
                // (Re)allocate the vertex, index and instance buffers.

                elementType = source.elementType;
                elementCount = source.elementCount;
                vertexSize = source.vertexSize;
                vertexBufferSize = source.vertexBufferSize;
                indexSize = source.indexSize;
                indexBufferSize = source.indexBufferSize;
                instanceSize = source.instanceSize;
                instanceBufferSize = source.instanceBufferSize;
                usage = source.usage;
                indexed = source.indexed;
                instanced = source.instanced;

                glBindVertexArray(VAO);

                glBindBuffer(GL_ARRAY_BUFFER, VBO);
                glBufferData(GL_ARRAY_BUFFER,
                             vertexBufferSize,
                             nullptr,
                             usage);

                if (indexed) {
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
                    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                                 numeric_cast<GLsizei>(indexBufferSize),
                                 nullptr,
                                 usage);
                }

                for (int i = 0; i < desc.vertexLayout.size(); i++) {
                    glEnableVertexAttribArray(i);
                }

                for (int i = 0; i < desc.instanceLayout.size(); i++) {
                    glEnableVertexAttribArray(desc.vertexLayout.size() + i);
                }

                size_t currentOffset = 0;
                for (int i = 0; i < desc.vertexLayout.size(); i++) {
                    auto &binding = desc.vertexLayout.at(i);
                    glVertexAttribPointer(i,
                                          VertexAttribute::getCount(binding.type),
                                          getType(binding.component),
                                          GL_FALSE,
                                          vertexSize,
                                          (void *) (currentOffset));
                    currentOffset += binding.stride();
                }

                if (instanced) {
                    currentOffset = 0;

                    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
                    glBufferData(GL_ARRAY_BUFFER,
                                 instanceBufferSize,
                                 nullptr,
                                 usage);

                    for (int i = 0; i < desc.instanceLayout.size(); i++) {
                        auto &binding = desc.instanceLayout.at(i);
                        auto index = desc.vertexLayout.size() + i;
                        glVertexAttribPointer(index,
                                              VertexAttribute::getCount(binding.type),
                                              getType(binding.component),
                                              GL_FALSE,
                                              instanceSize,
                                              (void *) currentOffset);
                        currentOffset += binding.stride();
                    }

                    for (int i = 0; i < desc.instanceLayout.size(); i++) {
                        glVertexAttribDivisor(desc.vertexLayout.size() + i, 1);
                    }
                }

                glBindVertexArray(0);

                glBindBuffer(GL_ARRAY_BUFFER, 0);

                if (indexed) {
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
                }

                checkGLError();
            }

            // Copy the vertex buffer
            glBindBuffer(GL_COPY_READ_BUFFER, source.VBO);
            glBindBuffer(GL_COPY_WRITE_BUFFER, VBO);

            glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, vertexBufferSize);

            if (indexed) {
                // Copy the index buffer
                glBindBuffer(GL_COPY_READ_BUFFER, source.EBO);
                glBindBuffer(GL_COPY_WRITE_BUFFER, EBO);
                glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, indexBufferSize);
            }

            if (instanced) {
                // Copy instance buffer
                glBindBuffer(GL_COPY_READ_BUFFER, source.instanceVBO);
                glBindBuffer(GL_COPY_WRITE_BUFFER, instanceVBO);
                glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, instanceBufferSize);
            }

            checkGLError();

            return std::make_unique<OGLFence>();
        }

        size_t getMemoryUsage() override {
            return vertexBufferSize + indexBufferSize + instanceBufferSize;
        }
    };
}

#endif //XENGINE_OGLVERTEXBUFFER_HPP
