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

#ifndef XENGINE_RASTERCONTEXT_HPP
#define XENGINE_RASTERCONTEXT_HPP

#include "xng/rendergraph/drawcall.hpp"
#include "xng/rendergraph/resourceid.hpp"
#include "xng/rendergraph/pipelinecache.hpp"
#include "xng/rendergraph/texturebinding.hpp"
#include "xng/rendergraph/indexformat.hpp"

#include "xng/rendergraph/resource/buffer.hpp"

#include "xng/rendergraph/shader/shader.hpp"

#include "xng/math/vector2.hpp"

namespace xng::rg {
    class RasterContext {
    public:
        virtual ~RasterContext() = default;

        /**
         * Bind a pipeline.
         *
         * Resets previous bindings.
         *
         * @param pipeline
         */
        virtual void bindPipeline(const PipelineCache::Handle &pipeline) = 0;

        /**
         * Bind a vertex buffer to a binding point.
         *
         * Can only be called after a pipeline has been bound.
         *
         * @param buffer The buffer to bind
         * @param bindingPoint The binding point to bind the buffer to
         * @param offset The offset into the buffer of the first element
         * @param stride The stride between elements. For single buffer packed data this is the size of one vertex.
         */
        virtual void bindVertexBuffer(const Resource<Buffer> &buffer,
                                      unsigned int bindingPoint,
                                      size_t offset,
                                      size_t stride) = 0;

        /**
         * Bind the specified index buffer.
         *
         * Can only be called after a pipeline has been bound.
         *
         * @param buffer The buffer containing the index data.
         * @param format The format of the index data.
         */
        virtual void bindIndexBuffer(const Resource<Buffer> &buffer, IndexFormat format) = 0;

        /**
         * Bind a region of a storage buffer.
         *
         * Can only be called after a pipeline has been bound.
         *
         * @param target
         * @param buffer
         * @param offset
         * @param size If 0, the region from offset to the end of the buffer is bound.
         */
        virtual void bindStorageBuffer(const std::string &target,
                                       const Resource<Buffer> &buffer,
                                       size_t offset,
                                       size_t size) = 0;

        /**
         * Bind the specified texture array.
         *
         * Every element in the vector corresponds to an entry in Shader.textureArrays
         *
         * Can only be called after a pipeline has been bound.
         *
         * @param target
         * @param textureArray
         */
        virtual void bindTexture(const std::string &target, const std::vector<TextureBinding> &textureArray) = 0;

        /**
         * Set a shader parameter. (Implemented as Push Constants on vulkan)
         *
         * Shader parameters are values that change frequently (Per Draw) and have a size limit.
         *
         * Can only be called after a pipeline has been bound.
         *
         * @param name
         * @param value
         */
        virtual void setShaderParameter(const std::string &name, const ShaderPrimitive &value) = 0;

        /**
         * @param viewportOffset The upper left corner of the viewport.
         * @param viewportSize The width / height of the viewport.
         */
        virtual void setViewport(Vec2i viewportOffset, Vec2u viewportSize) = 0;

        /**
         * Update the stencil reference value in the pipeline.
         *
         * RasterPipeline::enableDynamicStencilReference must be set.
         *
         * @param value The new stencil reference to set.
         */
        virtual void setStencilReference(int value) = 0;

        /**
         * Draw vertices from a bound vertex buffer.
         *
         * drawCall.offset specifies byte offset into the bound vertex buffer.
         *
         * @param drawCall The draw call
         */
        virtual void drawArray(const DrawCall &drawCall) = 0;

        /**
         * Draw vertices indexed by values in a bound index buffer.
         *
         * drawCall.offset specifies byte offset into the bound index buffer.
         *
         * baseIndex specifies a base index value which is added to each index read from the bound index buffer.
         *
         * The baseIndex enables indexed drawing from a single index / vertex buffer as long as the all vertex data has the same layout.
         *
         * @param drawCall The draw call
         * @param baseVertex The index offset added to each index read from the bound index buffer.
         */
        virtual void drawIndexed(const DrawCall &drawCall, int baseVertex) = 0;

        /**
         * Same as drawArray but multiple instances.
         * Shaders can access the instance index via ShaderInstruction::OpCode::GetInstanceID.
         *
         * @param drawCall
         * @param instanceCount
         */
        virtual void drawArrayInstanced(const DrawCall &drawCall, unsigned int instanceCount) = 0;

        /**
         * Same as drawIndexed but multiple instances.
         * Shaders can access the instance index via ShaderInstruction::OpCode::GetInstanceID.
         *
         * @param drawCall
         * @param baseVertex
         * @param instanceCount
         */
        virtual void drawIndexedInstanced(const DrawCall &drawCall, int baseVertex, unsigned int instanceCount) = 0;

        /**
         * Same as drawArray but multiple draw calls.
         * Shaders can access the draw index via ShaderInstruction::OpCode::GetDrawID.
         *
         * @param drawCalls
         */
        virtual void drawArrayMulti(const std::vector<DrawCall> &drawCalls) = 0;

        /**
         * Same as drawIndexed but multiple draw calls.
         * Shaders can access the draw index via ShaderInstruction::OpCode::GetDrawID.
         *
         * @param drawCalls
         */
        virtual void drawIndexedMulti(const std::vector<std::pair<DrawCall, int> > &drawCalls) = 0;

        /**
         * Draw array via draw command in a buffer.
         *
         * Format of the draw command must be:
         * {
         *      ShaderPrimitive::UInt vertexCount; -> The number of vertices to draw
         *      ShaderPrimitive::UInt instanceCount; -> The number of instances to draw
         *      ShaderPrimitive::UInt firstVertex; -> The index of the first vertex
         *      ShaderPrimitive::UInt baseInstance; -> The index of the first instance to draw (GetBaseInstance returns this value)
         * }
         *
         * @param indirectBuffer The buffer containing the draw command.
         * @param offset The byte offset into the buffer at which the draw command is present.
         */
        virtual void drawArrayIndirect(const Resource<Buffer> &indirectBuffer, size_t offset) = 0;

        /**
         * Draw indexed via draw command in a buffer.
         *
         * Format of the draw command must be:
         * {
         *      ShaderPrimitive::UInt indexCount; -> The number of indices to draw
         *      ShaderPrimitive::UInt instanceCount; -> The number of instances to draw
         *      ShaderPrimitive::UInt firstIndex; -> The index of the first index (indexBuffer[firstIndex * sizeof(indexFormat)])
         *      ShaderPrimitive::Int baseVertex; -> The value added to each index in the buffer
         *      ShaderPrimitive::UInt baseInstance; -> The index of the first instance to draw (GetBaseInstance returns this value)
         * }
         *
         * @param indirectBuffer The buffer containing the draw command.
         * @param offset The byte offset into the buffer at which the draw command is present.
         */
        virtual void drawIndexedIndirect(const Resource<Buffer> &indirectBuffer, size_t offset) = 0;

        /**
         * Multi-draw arrays via draw commands in a buffer.
         *
         * The Format of the draw commands is identical to drawArrayIndirect().
         *
         * @param indirectBuffer The buffer containing the draw commands
         * @param offset The byte offset into the buffer at which the draw commands are present.
         * @param drawCount The number of draw commands in the buffer.
         * @param stride The distance in bytes between draw commands in the buffer.
         */
        virtual void drawArrayMultiIndirect(const Resource<Buffer> &indirectBuffer,
                                            size_t offset,
                                            size_t drawCount,
                                            size_t stride) = 0;

        /**
         * Multi-draw indexed via draw commands in a buffer.
         *
         * The Format of the draw commands is identical to drawIndexedIndirect().
         *
         * @param indirectBuffer The buffer containing the draw commands
         * @param offset The byte offset into the buffer at which the draw commands are present.
         * @param drawCount The number of draw commands in the buffer.
         * @param stride The distance in bytes between draw commands in the buffer.
         */
        virtual void drawIndexedMultiIndirect(const Resource<Buffer> &indirectBuffer,
                                              size_t offset,
                                              size_t drawCount,
                                              size_t stride) = 0;

        /**
         * Multi-draw arrays via draw commands and draw count fetched from buffers.
         *
         * The Format of the draw commands is identical to drawArrayIndirect().
         *
         * The draw count buffer must store a 4-byte signed integer value specifying the draw count.
         *
         * Draw count offset must be a multiple of 4.
         *
         * @param indirectBuffer The buffer containing the draw commands.
         * @param drawCountBuffer The buffer containing the draw count value.
         * @param indirectOffset The byte offset into the buffer at which the draw commands are present.
         * @param drawCountOffset The offset into the draw count buffer at which the draw count value is present.
         * @param maxDrawCount The maximum number expected to be stored in the draw count buffer.
         * @param stride The distance in bytes between draw commands in the buffer.
         */
        virtual void drawArrayMultiIndirectCount(const Resource<Buffer> &indirectBuffer,
                                                 const Resource<Buffer> &drawCountBuffer,
                                                 size_t indirectOffset,
                                                 size_t drawCountOffset,
                                                 size_t maxDrawCount,
                                                 size_t stride) = 0;

        /**
         * Multi-draw indexed via draw commands and draw count fetched from buffers.
         *
         * The Format of the draw commands is identical to drawIndexedIndirect().
         *
         * The draw count buffer must store a 4-byte signed integer value specifying the draw count.
         *
         * Draw count offset must be a multiple of 4.
         *
         * @param indirectBuffer The buffer containing the draw commands.
         * @param drawCountBuffer The buffer containing the draw count value.
         * @param indirectOffset The byte offset into the buffer at which the draw commands are present.
         * @param drawCountOffset The offset into the draw count buffer at which the draw count value is present.
         * @param maxDrawCount The maximum number expected to be stored in the draw count buffer.
         * @param stride The distance in bytes between draw commands in the buffer.
         */
        virtual void drawIndexedMultiIndirectCount(const Resource<Buffer> &indirectBuffer,
                                                   const Resource<Buffer> &drawCountBuffer,
                                                   size_t indirectOffset,
                                                   size_t drawCountOffset,
                                                   size_t maxDrawCount,
                                                   size_t stride) = 0;
    };
}

#endif //XENGINE_RASTERCONTEXT_HPP
