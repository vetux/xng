/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 3 of the License, or (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.

 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef XENGINE_RENDERGRAPH_CONTEXT_HPP
#define XENGINE_RENDERGRAPH_CONTEXT_HPP


#include "xng/rendergraph/drawcall.hpp"
#include "xng/rendergraph/resourceid.hpp"
#include "xng/rendergraph/image.hpp"
#include "xng/rendergraph/pipelinecache.hpp"
#include "xng/rendergraph/texturebinding.hpp"

#include "xng/rendergraph/resource/indexbuffer.hpp"
#include "xng/rendergraph/resource/storagebuffer.hpp"
#include "xng/rendergraph/resource/vertexbuffer.hpp"

#include "xng/rendergraph/shader/shader.hpp"

#include "xng/math/vector2.hpp"

namespace xng::rendergraph {
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
         * Bind the specified vertex buffer.
         *
         * Can only be called after a pipeline has been bound.
         *
         * @param buffer
         */
        virtual void bindVertexBuffer(const Resource<VertexBuffer> &buffer) = 0;

        /**
         * Bind the specified index buffer.
         *
         * Can only be called after a pipeline has been bound.
         *
         * @param buffer
         */
        virtual void bindIndexBuffer(const Resource<IndexBuffer> &buffer) = 0;

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
                                       const Resource<StorageBuffer> &buffer,
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

        virtual void setViewport(Vec2i viewportOffset, Vec2i viewportSize) = 0;

        virtual void drawArray(const DrawCall &drawCall) = 0;

        virtual void drawIndexed(const DrawCall &drawCall, size_t indexOffset) = 0;

        //TODO: Add drawInstanced
        //TODO: Add drawMulti
    };
}

#endif //XENGINE_RENDERGRAPH_CONTEXT_HPP
