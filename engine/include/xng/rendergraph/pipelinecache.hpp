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

#ifndef XENGINE_PIPELINECACHE_HPP
#define XENGINE_PIPELINECACHE_HPP

#include <ostream>
#include <istream>
#include <vector>

#include "xng/rendergraph/pipeline/rasterpipeline.hpp"
#include "xng/rendergraph/pipeline/computepipeline.hpp"

namespace xng::rendergraph {
    /**
     * Stores an internal pipeline cache that can optionally be saved to disk.
     */
    class PipelineCache {
    public:
        typedef size_t Handle;

        /**
         * Destroys all allocated pipelines.
         */
        virtual ~PipelineCache() = default;

        /**
         * Creates a raster pipeline from the given description.
         *
         * This is an expensive cpu operation and blocks until the pipeline is allocated in driver-managed memory.
         *
         * @param desc
         * @return
         */
        virtual Handle create(const RasterPipeline &desc) = 0;

        /**
         * Creates a compute pipeline from the given description.
         *
         * This is an expensive cpu operation and blocks until the pipeline is allocated in driver-managed memory.
         *
         * @param desc
         * @return
         */
        virtual Handle create(const ComputePipeline &desc) = 0;

        /**
         * Destroy a pipeline and free up the corresponding gpu resources.
         *
         * Internally, the cache keeps the pipeline allocated on the cpu,
         * and subsequent create() and save() invocations will still access the cached pipeline.
         *
         * @param handle The pipeline to destroy.
         */
        virtual void destroy(Handle handle) = 0;

        /**
         * Destroy all allocated pipelines and recreate the internal cache.
         *
         * This is implicitly an expensive operation because all pipelines must be recreated from scratch.
         *
         * Apis like Vulkan do not provide per pipeline eviction from the internal cache.
         *
         * Usually this is only needed in development for features like shader hot reload.
         */
        virtual void clear() = 0;

        /**
         * Save the pipeline cache blob.
         * All previously created pipelines are saved.
         *
         * @param stream
         */
        virtual void save(std::ostream &stream) = 0;

        /**
         * Load an existing pipeline cache blob.
         *
         * @param stream The stream from which to load the cache
         */
        virtual void load(std::istream &stream) = 0;

        /**
         * Return the internal compiled shader source code for the given pipeline.
         * This is a debug method and is optional for the runtime to implement.
         * It may or may not return any valid data.
         *
         * @param handle The target pipeline
         * @return The compiled shaders
         */
        virtual std::unordered_map<Shader::Stage, std::string> getCompiledShaderSource(Handle handle) = 0;
    };
}

#endif //XENGINE_PIPELINECACHE_HPP
