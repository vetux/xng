/**
 *  This file is part of xEngine, a C++ game engine library.
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

#ifndef XENGINE_OGLVERTEXARRAYOBJECT_HPP
#define XENGINE_OGLVERTEXARRAYOBJECT_HPP

#include "xng/gpu/vertexarrayobject.hpp"

#include "gpu/opengl/oglvertexbuffer.hpp"
#include "gpu/opengl/oglindexbuffer.hpp"

#include <utility>

namespace xng::opengl {
    class OGLVertexArrayObject : public VertexArrayObject {
    public:
        std::function<void(RenderObject * )> destructor;
        VertexArrayObjectDesc desc;

        OGLVertexBuffer *mVertexBuffer = nullptr;
        OGLIndexBuffer *mIndexBuffer = nullptr;
        OGLVertexBuffer *mInstanceBuffer = nullptr;

        GLuint VAO = 0;

        OGLVertexArrayObject(std::function<void(RenderObject * )> destructor,
                             VertexArrayObjectDesc desc)
                : destructor(std::move(destructor)),
                  desc(std::move(desc)) {
            glGenVertexArrays(1, &VAO);
            checkGLError();
        }

        ~OGLVertexArrayObject() override {
            glDeleteVertexArrays(1, &VAO);
            destructor(this);
            checkGLError();
        }

        VertexArrayObjectDesc &getDescription() override {
            return desc;
        }

        VertexBuffer *getVertexBuffer() override {
            return mVertexBuffer;
        }

        IndexBuffer *getIndexBuffer() override {
            return mIndexBuffer;
        }

        VertexBuffer *getInstanceBuffer() override {
            return mInstanceBuffer;
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

        void bindBuffers(VertexBuffer &vertexBuffer) override {
            mVertexBuffer = dynamic_cast<OGLVertexBuffer *>(&vertexBuffer);
            mIndexBuffer = nullptr;
            mInstanceBuffer = nullptr;

            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer->VBO);

            GLsizei vertexStride = 0;
            for (auto &layout: desc.vertexLayout.attributes) {
                vertexStride += layout.stride();
            }

            size_t currentOffset = 0;
            for (int i = 0; i < desc.vertexLayout.attributes.size(); i++) {
                auto &binding = desc.vertexLayout.attributes.at(i);
                glEnableVertexAttribArray(i);
                glVertexAttribPointer(i,
                                      VertexAttribute::getCount(binding.type),
                                      getType(binding.component),
                                      GL_FALSE,
                                      vertexStride,
                                      (void *) (currentOffset));
                currentOffset += binding.stride();
            }

            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            checkGLError();
        }

        void bindBuffers(VertexBuffer &vertexBuffer, IndexBuffer &indexBuffer) override {
            mVertexBuffer = dynamic_cast<OGLVertexBuffer *>(&vertexBuffer);
            mIndexBuffer = dynamic_cast<OGLIndexBuffer *>(&indexBuffer);
            mInstanceBuffer = nullptr;

            glBindVertexArray(VAO);

            // Vertex Buffer
            glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer->VBO);
            checkGLError();

            // Index Buffer
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer->EBO);
            checkGLError();

            // Vertex Attributes
            GLsizei vertexStride = 0;
            for (auto &layout: desc.vertexLayout.attributes) {
                vertexStride += layout.stride();
            }

            size_t currentOffset = 0;
            for (int i = 0; i < desc.vertexLayout.attributes.size(); i++) {
                auto &attribute = desc.vertexLayout.attributes.at(i);
                glEnableVertexAttribArray(i);
                glVertexAttribPointer(i,
                                      VertexAttribute::getCount(attribute.type),
                                      getType(attribute.component),
                                      GL_FALSE,
                                      vertexStride,
                                      (void *) (currentOffset));
                currentOffset += attribute.stride();
            }
            checkGLError();

            glBindVertexArray(0);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            checkGLError();
        }

        void bindBuffers(VertexBuffer &vertexBuffer, IndexBuffer &indexBuffer, VertexBuffer &instanceBuffer) override {
            mVertexBuffer = dynamic_cast<OGLVertexBuffer *>(&vertexBuffer);
            mIndexBuffer = dynamic_cast<OGLIndexBuffer *>(&indexBuffer);
            mInstanceBuffer = dynamic_cast<OGLVertexBuffer *>(&instanceBuffer);

            glBindVertexArray(VAO);

            // Vertex Buffer
            glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer->VBO);

            // Index Buffer
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer->EBO);

            // Vertex Attributes
            GLsizei vertexStride;
            for (auto &layout: desc.vertexLayout.attributes) {
                vertexStride += layout.stride();
            }

            size_t currentOffset = 0;
            for (int i = 0; i < desc.vertexLayout.attributes.size(); i++) {
                auto &binding = desc.vertexLayout.attributes.at(i);
                glEnableVertexAttribArray(i);
                glVertexAttribPointer(i,
                                      VertexAttribute::getCount(binding.type),
                                      getType(binding.component),
                                      GL_FALSE,
                                      vertexStride,
                                      (void *) (currentOffset));
                currentOffset += binding.stride();
            }

            // Instance Buffer
            glBindBuffer(GL_ARRAY_BUFFER, mInstanceBuffer->VBO);

            // Instance Attributes
            GLsizei instanceStride;
            for (auto &layout: desc.instanceArrayLayout.attributes) {
                instanceStride += layout.stride();
            }

            currentOffset = 0;
            for (int i = 0; i < desc.instanceArrayLayout.attributes.size(); i++) {
                auto &binding = desc.instanceArrayLayout.attributes.at(i);
                auto index = desc.vertexLayout.attributes.size() + i;
                glVertexAttribPointer(index,
                                      VertexAttribute::getCount(binding.type),
                                      getType(binding.component),
                                      GL_FALSE,
                                      instanceStride,
                                      (void *) currentOffset);
                currentOffset += binding.stride();
            }

            for (int i = 0; i < desc.instanceArrayLayout.attributes.size(); i++) {
                glVertexAttribDivisor(desc.vertexLayout.attributes.size() + i, 1);
            }

            glBindVertexArray(0);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            checkGLError();
        }
    };
}

#endif //XENGINE_OGLVERTEXARRAYOBJECT_HPP
