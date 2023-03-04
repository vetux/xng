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

#ifndef XENGINE_VERTEXARRAYOBJECT_HPP
#define XENGINE_VERTEXARRAYOBJECT_HPP

#include "xng/gpu/renderobject.hpp"

#include "xng/gpu/vertexbuffer.hpp"
#include "xng/gpu/indexbuffer.hpp"
#include "xng/gpu/vertexarrayobjectdesc.hpp"

namespace xng {
    /**
     * The array object accesses vertex data from a set of buffers and uses a layout to specify
     * how the buffers should be used to generate the vertex attributes for the vertex shader.
     *
     * This solves the discrepancy between vulkan and opengl where in vulkan the vertex buffer attributes are specified
     * by the pipeline and in OpenGL it must be specified in a vertex array object.
     *
     * On OpenGL this would represent a vertex array (glBindVertexArray) with the corresponding vertex attribute pointer setup.
     * On vulkan this would hold the buffers for the pipeline to bind when drawing.
     */
    class VertexArrayObject : public RenderObject {
    public:
        Type getType() override {
            return RENDER_OBJECT_VERTEX_ARRAY_OBJECT;
        }

        virtual const VertexArrayObjectDesc &getDescription() = 0;

        virtual VertexBuffer *getVertexBuffer() = 0;

        /**
         * Optional index buffer for specifying indices into the vertex buffer for elements instead of reading
         * the elements directly from the vertex buffer.
         *
         * @return
         */
        virtual IndexBuffer *getIndexBuffer() = 0;

        /**
         * The instance buffer is vertex attribute data that is updated for each instance instead of each vertex
         * by the graphics driver and accessed using the instanceArrayLayout specified in the vertex array object description.
         *
         * Shader attribute bindings can access the instance attributes by using indices beginning at vertexLayout.size
         *
         * @return
         */
        virtual VertexBuffer *getInstanceBuffer() = 0;

        virtual void bindBuffers(VertexBuffer &vertexBuffer) = 0;

        virtual void bindBuffers(VertexBuffer &vertexBuffer,
                                 IndexBuffer &indexBuffer) = 0;

        /**
         * Set the buffers to bind when this array object is bound to a pipeline.
         *
         * @param vertexBuffer
         * @param indexBuffer
         * @param instanceBuffer
         * @return
         */
        virtual void bindBuffers(VertexBuffer &vertexBuffer,
                                 IndexBuffer &indexBuffer,
                                 VertexBuffer &instanceBuffer) = 0;
    };
}

#endif //XENGINE_VERTEXARRAYOBJECT_HPP
