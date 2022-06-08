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

#ifndef XENGINE_OGLMESHBUFFER_HPP
#define XENGINE_OGLMESHBUFFER_HPP

#include "graphics/meshbuffer.hpp"

#include <utility>

#include "graphics/opengl/oglbuildmacro.hpp"
#include "graphics/opengl/oglmeshbufferview.hpp"

#include "math/matrixmath.hpp"

namespace xengine ::opengl {
    class OPENGL_TYPENAME(MeshBuffer) : public MeshBuffer OPENGL_INHERIT {
    public:
        MeshBufferDesc desc;

        GLuint VAO = 0;
        GLuint VBO = 0;
        GLuint EBO = 0;

        size_t elementCount = 0;
        GLuint elementType = 0;

        bool indexed = false;

        bool instanced = false;
        size_t instanceCount = 0;
        GLuint instanceVBO = 0;

        explicit OPENGL_TYPENAME(MeshBuffer)(MeshBufferDesc desc) :
                desc(std::move(desc)) {
            initialize();
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);
            glGenBuffers(1, &instanceVBO);
        }

        OPENGL_TYPENAME(MeshBuffer)(const OPENGL_TYPENAME(MeshBuffer) &copy) = delete;

        OPENGL_TYPENAME(MeshBuffer) &operator=(const OPENGL_TYPENAME(MeshBuffer) &copy) = delete;

        ~OPENGL_TYPENAME(MeshBuffer)() override {
            glDeleteVertexArrays(1, &VAO);
            glDeleteBuffers(1, &VBO);
            glDeleteBuffers(1, &EBO);
            glDeleteBuffers(1, &instanceVBO);
        }

        const MeshBufferDesc &getDescription() override {
            return desc;
        }

        std::unique_ptr<MeshBufferView> createView() override {
            auto ret = std::make_unique<OPENGL_TYPENAME(MeshBufferView)>();
            ret->buffer = this;
            return ret;
        }

        void upload(const uint8_t *buffer,
                    size_t bufferSize,
                    const uint8_t *instanceBuffer,
                    size_t instanceBufferSize,
                    const std::vector<uint> &indices) override {
            elementType = GL_TRIANGLES;

            indexed = desc.numberOfIndices != 0;
            instanced = desc.numberOfInstances != 0;

            if (indexed) {
                elementCount = indices.size();

                int stride = 0;
                for (auto &binding: desc.vertexLayout) {
                    stride += binding.stride();
                }

                int instanceStride = 0;
                for (auto &binding: desc.instanceLayout) {
                    instanceStride += binding.stride();
                }

                glBindVertexArray(VAO);

                glBindBuffer(GL_ARRAY_BUFFER, VBO);
                glBufferData(GL_ARRAY_BUFFER,
                             numeric_cast<GLsizeiptr>(stride * desc.numberOfVertices),
                             buffer,
                             GL_STATIC_DRAW);

                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                             numeric_cast<GLsizeiptr>(sizeof(uint) * indices.size()),
                             indices.data(),
                             GL_STATIC_DRAW);

                size_t currentOffset = 0;
                for (int i = 0; i < desc.vertexLayout.size(); i++) {
                    auto index = i;
                    glEnableVertexAttribArray(index);
                    auto &binding = desc.vertexLayout.at(index);
                    glVertexAttribPointer(index,
                                          VertexAttribute::getCount(binding.type),
                                          getType(binding.component),
                                          GL_FALSE,
                                          stride,
                                          (void *) (currentOffset));
                    currentOffset += binding.stride();
                }

                glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
                glBufferData(GL_ARRAY_BUFFER,
                             numeric_cast<int>(instanceBufferSize * desc.numberOfInstances),
                             instanceBuffer,
                             GL_STATIC_DRAW);

                currentOffset = 0;

                for (int i = 0; i < desc.instanceLayout.size(); i++) {
                    auto index = desc.vertexLayout.size() + i;
                    glEnableVertexAttribArray(index);
                    auto &binding = desc.instanceLayout.at(index);
                    glVertexAttribPointer(index,
                                          VertexAttribute::getCount(binding.type),
                                          getType(binding.component),
                                          GL_FALSE,
                                          instanceStride,
                                          (void *) currentOffset);
                    glVertexAttribDivisor(index, 1);
                    currentOffset += binding.stride();
                }

                glBindVertexArray(0);

                glBindBuffer(GL_ARRAY_BUFFER, 0);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            } else {
                elementCount = desc.numberOfVertices;

                int stride = 0;
                for (auto &binding: desc.vertexLayout) {
                    stride += binding.stride();
                }

                int instanceStride = 0;
                for (auto &binding: desc.instanceLayout) {
                    instanceStride += binding.stride();
                }

                glBindVertexArray(VAO);

                glBindBuffer(GL_ARRAY_BUFFER, VBO);
                glBufferData(GL_ARRAY_BUFFER,
                             numeric_cast<GLsizeiptr>(stride * desc.numberOfVertices),
                             buffer,
                             GL_STATIC_DRAW);

                size_t currentOffset = 0;
                for (int i = 0; i < desc.vertexLayout.size(); i++) {
                    auto index = i;
                    glEnableVertexAttribArray(index);
                    auto &binding = desc.vertexLayout.at(index);
                    glVertexAttribPointer(index,
                                          VertexAttribute::getCount(binding.type),
                                          getType(binding.component),
                                          GL_FALSE,
                                          stride,
                                          (void *) (currentOffset));
                    currentOffset += binding.stride();
                }

                glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
                glBufferData(GL_ARRAY_BUFFER,
                             numeric_cast<int>(instanceBufferSize * desc.numberOfInstances),
                             instanceBuffer,
                             GL_STATIC_DRAW);

                currentOffset = 0;

                for (int i = 0; i < desc.instanceLayout.size(); i++) {
                    auto index = desc.vertexLayout.size() + i;
                    glEnableVertexAttribArray(index);
                    auto &binding = desc.instanceLayout.at(index);
                    glVertexAttribPointer(index,
                                          VertexAttribute::getCount(binding.type),
                                          getType(binding.component),
                                          GL_FALSE,
                                          instanceStride,
                                          (void *) currentOffset);
                    glVertexAttribDivisor(index, 1);
                    currentOffset += binding.stride();
                }

                glBindVertexArray(0);

                glBindBuffer(GL_ARRAY_BUFFER, 0);
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

#endif //XENGINE_OGLMESHBUFFER_HPP
