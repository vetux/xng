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

#ifndef XENGINE_RENDERGRAPH_RUNTIME_HPP
#define XENGINE_RENDERGRAPH_RUNTIME_HPP

#include "xng/display/window.hpp"

#include "xng/rendergraph/heap.hpp"
#include "xng/rendergraph/pipelinecache.hpp"
#include "xng/rendergraph/graph.hpp"
#include "xng/rendergraph/semaphore.hpp"
#include "xng/rendergraph/textureformatlimits.hpp"

/**
 * The Render Graph namespace.
 * Contains the render graph interface.
 */
namespace xng::rg {
    class Runtime {
    public:
        virtual ~Runtime() = default;

        /**
         * The passed window instance must be compatible with the runtime implementation.
         *
         * Runtimes can operate completely headless.
         *
         * On OpenGL this is a bit messy. The display environment must manage a single global context with a hidden
         * window, and all other windows are created with sub contexts of this global context.
         *
         * On Vulkan it is completely decoupled, and the windowing system interaction exists solely in this method.
         *
         * @param window The window for which to create a surface.
         * @param swapCount The number of swap chain images to create for the surface.
         * @return The Surface representing the window contents.
         */
        virtual std::shared_ptr<Surface> createSurface(std::shared_ptr<Window> window, size_t swapCount) = 0;

        /**
         * Destroying a heap resource currently referenced by a compiled graph is forbidden.
         *
         * @return The resource heap.
         */
        virtual Heap &getResourceHeap() = 0;

        /**
         * @return The pipeline cache.
         */
        virtual PipelineCache &getPipelineCache() = 0;

        /**
         * In practice all color formats except the compressed formats should be supported across all runtime implementations.
         *
         * @return The list of supported color formats.
         */
        virtual std::unordered_set<ColorFormat> getSupportedColorFormats() = 0;

        /**
         * @return The limits for the given type / colorFormat / capabilities combination.
         */
        virtual TextureFormatLimits getTextureFormatLimits(TextureType type,
                                                           ColorFormat colorFormat,
                                                           Texture::Capability capabilities) = 0;

        /**
         * Execute a single graph.
         *
         * Passes run in parallel where possible, and executions may overlap.
         *
         * The runtime guarantees resource access synchronization correctness at all times based on
         * graph ordering rules and execute() invocation order.
         *
         * The runtime will internally queue swapping operations for all referenced surfaces.
         *
         * The runtime implementation must catch and handle errors internally when the surface size changes during rendering.
         * (E.G. OUT_OF_DATE / SUBOPTIMAL in Vulkan, No Equivalent in OpenGL)
         *
         * What exactly is drawn in the screen buffer if a swap chain / framebuffer size doesn't match
         * is not clearly defined in the graphic apis.
         *
         * On Vulkan the passes internally run concurrently on their corresponding queues if available (On Modern platforms one queue per context type)
         *
         * @param graph
         *
         * @return The semaphore representing this submission.
         */
        virtual std::unique_ptr<Semaphore> execute(const Graph &graph) = 0;

        /**
         * Execute multiple graphs.
         *
         * Graphs may cross-reference heap resources in which case the graph order defines the semantic execution order
         * and the runtime inserts barriers where needed for synchronization correctness.
         *
         * Passes run in parallel where possible, and executions may overlap.
         *
         * The runtime guarantees resource access synchronization correctness at all times based on
         * graph ordering rules and execute() invocation order.
         *
         * The runtime will internally queue swapping operations for all referenced surfaces.
         *
         * The runtime implementation must catch and handle errors internally when the window size changes during rendering.
         * (E.G. OUT_OF_DATE / SUBOPTIMAL in Vulkan, No Equivalent in OpenGL)
         *
         * What exactly is drawn in the screen buffer if a swap chain / framebuffer size doesn't match
         * is not clearly defined in the graphic apis.
         *
         * @param graphs
         *
         * @return The semaphore representing this submission.
         */
        virtual std::unique_ptr<Semaphore> execute(const std::vector<Graph> &graphs) = 0;
    };
}

#endif //XENGINE_RENDERGRAPH_RUNTIME_HPP
