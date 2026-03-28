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

#ifndef XENGINE_RENDERGRAPH_RUNTIME_HPP
#define XENGINE_RENDERGRAPH_RUNTIME_HPP

#include "xng/display/window.hpp"

#include "xng/rendergraph/heap.hpp"
#include "xng/rendergraph/pipelinecache.hpp"
#include "xng/rendergraph/graph.hpp"
#include "xng/rendergraph/statistics.hpp"

namespace xng::rendergraph {
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
         * @return The Surface representing the window contents.
         */
        virtual std::shared_ptr<Surface> createSurface(std::shared_ptr<Window> window) = 0;

        /**
         * For each frame in flight the runtime will manage:
         *  - Copies of heap MEMORY_CPU_TO_GPU / MEMORY_GPU_TO_CPU buffers
         *  - Transient resources
         *  - Swap chain images of surfaces
         *
         * This means frames in flight come at the cost of extra memory usage.
         *
         * The in flight rendering is completely transparent to the user.
         *
         * Some implementations (OpenGL) may not perform frames in flight manually and ignore the passed value,
         * or internally cap the maximum number of frames in flight (E.g. 2/3 for vulkan)
         *
         * @param framesInFlight The maximum number of requested frames in flight.
         */
        virtual void setFramesInFlight(size_t framesInFlight) = 0;

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
         * Execute a single graph.
         *
         * Passes run in parallel where possible.
         *
         * Write-After-Write is treated as undefined-behavior.
         * Runtimes may perform Last write wins or throw an exception.
         *
         * The runtime will internally queue swapping operations for all referenced surfaces.
         *
         * The runtime implementation must catch and handle errors internally when the surface size changes during rendering.
         * (E.G. OUT_OF_DATE / SUBOPTIMAL in Vulkan, No Equivalent in OpenGL)
         *
         * What exactly is drawn in the screen buffer if a swap chain / framebuffer size doesn't match
         * is not clearly defined in the graphic apis.
         *
         *  For Example,
         *        pass[0] writes resource0
         *        pass[1] reads/writes resource0
         *        pass[2] reads resource0
         *
         *        pass[1] will receive the changes from pass[0]
         *        pass[2] will receive the changes from pass[1]
         * @param graph
         */
        virtual Statistics execute(const Graph &graph) = 0;

        /**
         * Execute multiple graphs.
         *
         * The graphs run in parallel where possible.
         *
         * The runtime will internally queue swapping operations for all referenced surfaces.
         *
         * The runtime implementation must catch and handle errors internally when the window size changes during rendering.
         * (E.G. OUT_OF_DATE / SUBOPTIMAL in Vulkan, No Equivalent in OpenGL)
         *
         * What exactly is drawn in the screen buffer if a swap chain / framebuffer size doesn't match
         * is not clearly defined in the graphic apis.
         *
         *  For Example,
         *      graphs[0] write heap
         *      graphs[1] read and write heap
         *      graphs[2] read heap
         *      graphs[3] read and write heap
         *
         *      graphs[1] will receive the changes to the heap from graph[0]
         *      graphs[2] will receive the changes to the heap from graph[1]
         *      graphs[3] will receive the changes to the heap from graph[1], and the read operation might run in parallel with graphs[2]
         * @param graphs
         */
        virtual Statistics execute(const std::vector<Graph> &graphs) = 0;
    };
}

#endif //XENGINE_RENDERGRAPH_RUNTIME_HPP
