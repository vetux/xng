/**
 *  xEngine - C++ game engine library
 *  Copyright (C) 2021  Julian Zampiccoli
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

#ifndef XENGINE_OGLVERTEXBUFFER_HPP
#define XENGINE_OGLVERTEXBUFFER_HPP

#include "graphics/vertexbuffer.hpp"

#include <utility>

#include "graphics/opengl/oglbuildmacro.hpp"

#include "asset/mesh.hpp"

#include "math/matrixmath.hpp"

namespace xengine::opengl {
    class OPENGL_TYPENAME(VertexBuffer) : public VertexBuffer OPENGL_INHERIT {
    public:
        VertexBufferDesc desc;

        GLuint VAO = 0;
        GLuint VBO = 0;
        GLuint EBO = 0;

        size_t elementCount = 0;
        GLuint elementType = 0;

        bool indexed = false;

        bool instanced = false;
        size_t instanceCount = 0;
        GLuint instanceVBO = 0;

        explicit OPENGL_TYPENAME(VertexBuffer)(VertexBufferDesc desc) :
                desc(std::move(desc)) {
            initialize();
            indexed = desc.numberOfIndices != 0;
            instanced = desc.numberOfInstances != 0;
            if (indexed) {
                elementCount = desc.numberOfIndices / desc.primitive;
            } else {
                elementCount = desc.numberOfVertices / desc.primitive;
            }
            switch (desc.primitive) {
                case Mesh::POINT:
                    elementType = GL_POINTS;
                    break;
                case Mesh::LINE:
                    elementType = GL_LINES;
                    break;
                case Mesh::TRI:
                    elementType = GL_TRIANGLES;
                    break;
                case Mesh::QUAD:
                    elementType = GL_QUADS;
                    break;
                default:
                    throw std::runtime_error("Invalid primitive");
            }
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);
            if (instanced) {
                glGenBuffers(1, &instanceVBO);
            }
        }

        OPENGL_TYPENAME(VertexBuffer)(const OPENGL_TYPENAME(VertexBuffer) &copy) = delete;

        OPENGL_TYPENAME(VertexBuffer) &operator=(const OPENGL_TYPENAME(VertexBuffer) &copy) = delete;

        ~OPENGL_TYPENAME(VertexBuffer)() override {
            glDeleteVertexArrays(1, &VAO);
            glDeleteBuffers(1, &VBO);
            glDeleteBuffers(1, &EBO);
            if (instanced) {
                glDeleteBuffers(1, &instanceVBO);
            }
        }

        void pinGpuMemory() override {}

        void unpinGpuMemory() override {}

        const VertexBufferDesc &getDescription() override {
            return desc;
        }

        void upload(const uint8_t *vertexBuffer,
                    size_t vertexBufferSize,
                    const uint8_t *instanceBuffer,
                    size_t instanceBufferSize,
                    const std::vector<uint> &indices) override {
            int vertexStride = 0;
            for (auto &binding: desc.vertexLayout) {
                vertexStride += binding.stride();
            }

            int instanceStride = 0;
            for (auto &binding: desc.instanceLayout) {
                instanceStride += binding.stride();
            }

            auto indexBufferSize = indices.size() * sizeof(uint);

            auto vertexByteCount = numeric_cast<GLsizei>(vertexStride * desc.numberOfVertices);
            auto instanceByteCount = numeric_cast<GLsizei>(instanceStride * desc.numberOfInstances);
            auto indexByteCount = numeric_cast<GLsizei>(sizeof(uint) * desc.numberOfIndices);

            if (vertexBufferSize != vertexByteCount) {
                throw std::runtime_error("Invalid vertex buffer size");
            } else if (instanceBufferSize != instanceByteCount) {
                throw std::runtime_error("Invalid instance buffer size");
            } else if (indexBufferSize != indexByteCount) {
                throw std::runtime_error("Invalid index buffer size");
            }

            glBindVertexArray(VAO);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER,
                         vertexByteCount,
                         vertexBuffer,
                         GL_STATIC_DRAW);

            if (indexed) {
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                             indexByteCount,
                             indices.data(),
                             GL_STATIC_DRAW);
            }

            size_t currentOffset = 0;
            for (int i = 0; i < desc.vertexLayout.size(); i++) {
                auto &binding = desc.vertexLayout.at(i);
                glEnableVertexAttribArray(i);
                glVertexAttribPointer(i,
                                      VertexAttribute::getCount(binding.type),
                                      getType(binding.component),
                                      GL_FALSE,
                                      vertexStride,
                                      (void *) (currentOffset));
                currentOffset += binding.stride();
            }

            if (instanced) {
                currentOffset = 0;

                glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
                glBufferData(GL_ARRAY_BUFFER,
                             numeric_cast<int>(instanceByteCount),
                             instanceBuffer,
                             GL_STATIC_DRAW);

                for (int i = 0; i < desc.instanceLayout.size(); i++) {
                    auto &binding = desc.instanceLayout.at(i);
                    auto index = desc.vertexLayout.size() + i;
                    glEnableVertexAttribArray(index);
                    glVertexAttribPointer(index,
                                          VertexAttribute::getCount(binding.type),
                                          getType(binding.component),
                                          GL_FALSE,
                                          instanceStride,
                                          (void *) currentOffset);
                    glVertexAttribDivisor(index, 1);
                    currentOffset += binding.stride();
                }
            }

            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            if (indexed) {
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            }

            checkGLError("OGLRenderAllocator::createMeshBuffer");
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

        OPENGL_MEMBERS

        OPENGL_CONVERSION_MEMBERS
    };
}

#endif //XENGINE_OGLVERTEXBUFFER_HPP
