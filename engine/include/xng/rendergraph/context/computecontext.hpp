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

#ifndef XENGINE_COMPUTECONTEXT_HPP
#define XENGINE_COMPUTECONTEXT_HPP

#include "xng/rendergraph/resourceid.hpp"
#include "xng/rendergraph/pipelinecache.hpp"
#include "xng/rendergraph/texturebinding.hpp"

#include "xng/rendergraph/resource/storagebuffer.hpp"

#include "xng/math/vector3.hpp"

namespace xng::rendergraph {
    class ComputeContext {
    public:
        virtual ~ComputeContext() = default;

        /**
         * Bind a pipeline.
         *
         * Resets previous bindings.
         *
         * @param pipeline
         */
        virtual void bindPipeline(const PipelineCache::Handle &pipeline) = 0;

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

        /**
         * Dispatch workgroups.
         *
         * @param groupCount The number of workgroups to dispatch
         */
        virtual void dispatch(Vec3u groupCount) = 0;
    };
}

#endif //XENGINE_COMPUTECONTEXT_HPP
