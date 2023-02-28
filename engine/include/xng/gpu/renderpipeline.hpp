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

#ifndef XENGINE_RENDERPIPELINE_HPP
#define XENGINE_RENDERPIPELINE_HPP

#include "xng/gpu/renderobject.hpp"
#include "xng/gpu/renderpipelinedesc.hpp"
#include "xng/gpu/vertexbuffer.hpp"
#include "xng/gpu/rendertarget.hpp"
#include "xng/gpu/gpufence.hpp"
#include "xng/gpu/vertexarrayobject.hpp"
#include "xng/gpu/texturearraybuffer.hpp"

namespace xng {
    class XENGINE_EXPORT RenderPipeline : public RenderObject {
    public:
        typedef std::variant<
                std::reference_wrapper<TextureBuffer>,
                std::reference_wrapper<TextureArrayBuffer>,
                std::reference_wrapper<ShaderBuffer>
        > Binding;

        /**
         * A DrawCall specifies which portion of the bound index or vertex buffer to draw.
         */
        struct DrawCall {
            size_t offset = 0; // The offset into the index or vertex buffer at which to begin reading indices or vertices
            size_t count = 0; // The number of indices or vertices to draw.
            IndexBuffer::IndexType indexType = IndexBuffer::UNSIGNED_INT; // The type of the indices, ignored when not indexing
        };

        Type getType() override {
            return RENDER_PIPELINE;
        }

        /**
         * Must be called before using the drawing methods.
         *
         * @param target
         * @param viewportOffset
         * @param viewportSize
         */
        virtual void renderBegin(RenderTarget &target, Vec2i viewportOffset, Vec2i viewportSize) = 0;

        /**
         * Run the recorded drawing operations.
         *
         * @return The fence indicating when the drawing operations have finished.
         */
        virtual std::unique_ptr<GpuFence> renderPresent() = 0;

        /**
         * Draw without indexing.
         *
         * @param target
         * @param vertexArrayObject
         * @param command
         * @param bindings
         * @return
         */
        virtual void drawArray(const DrawCall &drawCall,
                               const std::vector<Binding> &bindings,
                               VertexArrayObject &vertexArrayObject) = 0;

        /**
         * Draw with indexing.
         *
         * @param target
         * @param vertexArrayObject
         * @param command
         * @param bindings
         * @return
         */
        virtual void drawIndexed(const DrawCall &drawCall,
                                 const std::vector<Binding> &bindings,
                                 VertexArrayObject &vertexArrayObject) = 0;

        /**
         * Draw using instancing.
         *
         * Requires RenderDeviceCapability.RENDER_PIPELINE_INSTANCING
         *
         * gl_InstanceID can be used in shaders to access the current instance index
         *
         * @param target
         * @param vertexArrayObject
         * @param command
         * @param bindings
         * @param numberOfInstances
         * @return
         */
        virtual void instancedDrawArray(const DrawCall &drawCall,
                                        const std::vector<Binding> &bindings,
                                        VertexArrayObject &vertexArrayObject,
                                        size_t numberOfInstances) = 0;

        /**
         * Draw using instancing.
         *
         * Requires RenderDeviceCapability.RENDER_PIPELINE_INSTANCING
         *
         * gl_InstanceID can be used in shaders to access the current instance index
         *
         * @param target
         * @param vertexArrayObject
         * @param command
         * @param bindings
         * @param numberOfInstances
         * @return
         */
        virtual void instancedDrawIndexed(const DrawCall &drawCall,
                                          const std::vector<Binding> &bindings,
                                          VertexArrayObject &vertexArrayObject,
                                          size_t numberOfInstances) = 0;

        /**
         * Draw multiple commands with one draw call.
         *
         * Requires RenderDeviceCapability.RENDER_PIPELINE_MULTI_DRAW
         *
         * gl_DrawID can be used in shaders to access the current command index
         *
         * @param target
         * @param vertexArrayObject
         * @param commands
         * @param bindings
         * @return
         */
        virtual void multiDrawArray(const std::vector<DrawCall> &drawCalls,
                                    const std::vector<Binding> &bindings,
                                    VertexArrayObject &vertexArrayObject) = 0;

        /**
         * Draw multiple commands with one draw call.
         *
         * Requires RenderDeviceCapability.RENDER_PIPELINE_MULTI_DRAW
         *
         * gl_DrawID can be used in shaders to access the current command index
         *
         * @param target
         * @param vertexArrayObject
         * @param commands
         * @param bindings
         * @return
         */
        virtual void multiDrawIndexed(const std::vector<DrawCall> &drawCalls,
                                      const std::vector<Binding> &bindings,
                                      VertexArrayObject &vertexArrayObject) = 0;

        /**
         * Draw with indexing and optional offset to apply when indexing into the vertex buffer.
         *
         * The baseVertex is an offset that is applied to each index read from the index buffer before indexing the vertex buffer.
         *
         * Requires RenderDeviceCapability.RENDER_PIPELINE_BASE_VERTEX
         *
         * @param target
         * @param vertexArrayObject
         * @param command
         * @param bindings
         * @return
         */
        virtual void drawIndexedBaseVertex(const DrawCall &drawCall,
                                           const std::vector<Binding> &bindings,
                                           VertexArrayObject &vertexArrayObject,
                                           size_t baseVertex) = 0;

        /**
         * Draw using instancing and optional offset to apply when indexing into the vertex buffer.
         *
         * The baseVertex is an offset that is applied to each index read from the index buffer before indexing the vertex buffer.
         *
         * Requires RenderDeviceCapability.RENDER_PIPELINE_BASE_VERTEX and RenderDeviceCapability.RENDER_PIPELINE_INSTANCING
         *
         * gl_InstanceID can be used in shaders to access the current instance index
         *
         * @param target
         * @param vertexArrayObject
         * @param command
         * @param bindings
         * @param numberOfInstances
         * @return
         */
        virtual void instancedDrawIndexedBaseVertex(const DrawCall &drawCall,
                                                    const std::vector<Binding> &bindings,
                                                    VertexArrayObject &vertexArrayObject,
                                                    size_t numberOfInstances,
                                                    size_t baseVertex) = 0;

        /**
         * Draw multiple commands with one draw call and optional offset to apply when indexing into the vertex buffer.
         *
         * The baseVertex is an offset that is applied to each index read from the index buffer before indexing the vertex buffer.
         *
         * Requires RenderDeviceCapability.RENDER_PIPELINE_BASE_VERTEX and RenderDeviceCapability.RENDER_PIPELINE_INSTANCING
         *
         * gl_DrawID can be used in shaders to access the current command index
         *
         * @param target
         * @param vertexArrayObject
         * @param commands
         * @param bindings
         * @return
         */
        virtual void multiDrawIndexedBaseVertex(const std::vector<DrawCall> &drawCalls,
                                                const std::vector<Binding> &bindings,
                                                VertexArrayObject &vertexArrayObject,
                                                size_t baseVertex) = 0;

        virtual std::vector<uint8_t> cache() = 0;

        virtual const RenderPipelineDesc &getDescription() = 0;
    };
}

#endif //XENGINE_RENDERPIPELINE_HPP
