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

#ifndef XENGINE_RENDERPASS_HPP
#define XENGINE_RENDERPASS_HPP

#include "xng/gpu/renderobject.hpp"
#include "xng/gpu/renderpassdesc.hpp"
#include "xng/gpu/vertexbuffer.hpp"
#include "xng/gpu/rendertarget.hpp"
#include "xng/gpu/gpufence.hpp"
#include "xng/gpu/vertexarrayobject.hpp"
#include "xng/gpu/texturearraybuffer.hpp"

namespace xng {
    class RenderPass : public RenderObject {
    public:
        typedef std::variant<
        std::reference_wrapper<TextureBuffer>,
        std::reference_wrapper<TextureArrayBuffer>,
        std::reference_wrapper<ShaderUniformBuffer>
        > ShaderData;

        static RenderPipelineBindingType getShaderDataType(const ShaderData &data){
            switch(data.index()){
                case 0:
                    return BIND_TEXTURE_BUFFER;
                case 1:
                    return BIND_TEXTURE_ARRAY_BUFFER;
                case 2:
                    return BIND_SHADER_BUFFER;
                default:
                    throw std::runtime_error("Invalid data index");
            }
        }

        /**
         * A DrawCall specifies which portion of the bound index or vertex buffer to draw.
         */
        struct DrawCall {
            size_t offset = 0; // The offset into the index or vertex buffer at which to begin reading indices or vertices in BYTES
            size_t count = 0; // The number of indices or vertices to draw.
            IndexBuffer::IndexType indexType = IndexBuffer::UNSIGNED_INT; // The type of the indices, ignored when not indexing
        };

        Type getType() override {
            return RENDER_OBJECT_RENDER_PASS;
        }

        virtual const RenderPassDesc &getDescription() = 0;

        /**
         * Must be called before using any of the methods.
         *
         * @param target
         * @param viewportOffset
         * @param viewportSize
         */
        virtual void beginRenderPass(RenderTarget &target,
                                 Vec2i viewportOffset,
                                 Vec2i viewportSize) = 0;

        /**
         * Run the recorded drawing operations.
         *
         * @return The fence indicating when the render operation has finished.
         */
        virtual std::unique_ptr<GpuFence> endRenderPass() = 0;

        virtual void setViewport(Vec2i viewportOffset, Vec2i viewportSize) = 0;

        virtual void clearColorAttachments(ColorRGBA clearColor) = 0;

        virtual void clearDepthAttachments(float clearDepthValue) = 0;

        /**
         * Previously bound VAO or shader data is rebound when binding a different pipeline.
         *
         * @param pipeline
         */
        virtual void bindPipeline(RenderPipeline &pipeline) = 0;

        /**
         * The bound vertex array object is used in subsequent draw calls.
         *
         * In one render operation multiple vertex array objects can be bound and used similar to opengl the currently
         * bound object will be used for drawing.
         *
         * Unlike opengl the drawing operations are deferred until the fence
         * returned by renderPresent returns and all data required for a render operation is supplied between renderBegin/Present
         * calls which allows vulkan implementations to use more efficient ways to handle the draw operations.
         *
         * Must be called before using any draw calls.
         *
         * @param vertexArrayObject
         */
        virtual void bindVertexArrayObject(VertexArrayObject &vertexArrayObject) = 0;

        /**
         * Must be called before binding a pipeline with a different vertex layout
         */
        virtual void unbindVertexArrayObject() = 0;

        /**
         * The bound shader data is made available to shaders in subsequent draw calls.
         *
         * @param bindings
         */
        virtual void bindShaderData(const std::vector<ShaderData> &bindings) = 0;

        /**
         * Must be called before binding a pipeline with a different shader binding layout
         */
        virtual void unbindShaderData() = 0;

        /**
         * Draw without indexing.
         *
         * @param drawCall
         */
        virtual void drawArray(const DrawCall &drawCall) = 0;

        /**
         * Draw with indexing.
         *
         * @param drawCall
         */
        virtual void drawIndexed(const DrawCall &drawCall) = 0;

        /**
         * Draw using instancing.
         *
         * Requires RenderDeviceCapability.RENDER_PIPELINE_INSTANCING
         *
         * gl_InstanceID can be used in shaders to access the current instance index
         *
         * @param drawCall
         * @param numberOfInstances
         */
        virtual void instancedDrawArray(const DrawCall &drawCall, size_t numberOfInstances) = 0;

        /**
         * Draw using instancing.
         *
         * Requires RenderDeviceCapability.RENDER_PIPELINE_INSTANCING
         *
         * gl_InstanceID can be used in shaders to access the current instance index
         *
         * @param drawCall
         * @param numberOfInstances
         */
        virtual void instancedDrawIndexed(const DrawCall &drawCall, size_t numberOfInstances) = 0;

        /**
         * Draw multiple commands with one draw call.
         *
         * Requires RenderDeviceCapability.RENDER_PIPELINE_MULTI_DRAW
         *
         * gl_DrawID can be used in shaders to access the current command index
         *
         * @param drawCalls
         */
        virtual void multiDrawArray(const std::vector<DrawCall> &drawCalls) = 0;

        /**
         * Draw multiple commands with one draw call.
         *
         * Requires RenderDeviceCapability.RENDER_PIPELINE_MULTI_DRAW
         *
         * gl_DrawID can be used in shaders to access the current command index
         *
         * @param drawCalls
         */
        virtual void multiDrawIndexed(const std::vector<DrawCall> &drawCalls) = 0;

        /**
         * Draw with indexing and optional offset to apply when indexing into the vertex buffer.
         *
         * The baseVertex is an offset that is applied to each index read from the index buffer before indexing the vertex buffer.
         *
         * Requires RenderDeviceCapability.RENDER_PIPELINE_BASE_VERTEX
         *
         * @param drawCall
         * @param baseVertex
         */
        virtual void drawIndexed(const DrawCall &drawCall, size_t baseVertex) = 0;

        /**
         * Draw using instancing and optional offset to apply when indexing into the vertex buffer.
         *
         * The baseVertex is an offset that is applied to each index read from the index buffer before indexing the vertex buffer.
         *
         * Requires RenderDeviceCapability.RENDER_PIPELINE_BASE_VERTEX and RenderDeviceCapability.RENDER_PIPELINE_INSTANCING
         *
         * gl_InstanceID can be used in shaders to access the current instance index
         *
         * @param drawCall
         * @param numberOfInstances
         * @param baseVertex
         */
        virtual void instancedDrawIndexed(const DrawCall &drawCall, size_t numberOfInstances, size_t baseVertex) = 0;

        /**
         * Draw multiple commands with one draw call and optional offset to apply when indexing into the vertex buffer.
         *
         * The baseVertex is an offset that is applied to each index read from the index buffer before indexing the vertex buffer.
         *
         * Requires RenderDeviceCapability.RENDER_PIPELINE_BASE_VERTEX and RenderDeviceCapability.RENDER_PIPELINE_INSTANCING
         *
         * gl_DrawID can be used in shaders to access the current command index
         *
         * @param drawCalls
         * @param baseVertices
         */
        virtual void multiDrawIndexed(const std::vector<DrawCall> &drawCalls, std::vector<size_t> baseVertices) = 0;
    };
}

#endif //XENGINE_RENDERPASS_HPP
