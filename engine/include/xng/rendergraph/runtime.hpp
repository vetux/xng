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

#include "xng/rendergraph/heap.hpp"
#include "xng/rendergraph/pipelinecache.hpp"
#include "xng/rendergraph/graph.hpp"
#include "xng/rendergraph/statistics.hpp"
#include "xng/rendergraph/resourceid.hpp"

#include "xng/rendergraph/resource/texture.hpp"

#include "xng/display/window.hpp"
#include "xng/display/graphicsapi.hpp"

namespace xng::rendergraph {
    /**
     * The runtime / context represents the platform-dependent implementation of the renderer.
     *
     * The runtime manages an offscreen back buffer representing the window contents for any given execute() invocation
     * and presents the color component of the offscreen back buffer to the window / swap chain before returning from an execute() call.
     *
     * At the start of an execute() call, the contents of this offscreen back buffer are undefined. (May or may not contain previous frame data)
     *
     * On Vulkan the back buffer color image is the next image in the swap chain, and depth stencil is an offscreen image, so no texture memory is wasted.
     */
    class Runtime {
    public:
        virtual ~Runtime() = default;

        virtual GraphicsAPI getGraphicsAPI() = 0;

        /**
         * Set the window to render to.
         *
         * @param window
         */
        virtual void setWindow(std::shared_ptr<Window> window) = 0;

        virtual Window &getWindow() = 0;

        //TODO: Frames in flight conflict with swapBuffers
        /**
         * Swap the back buffer.
         *
         * The runtime implementation must catch and handle errors internally
         * if the window size changes during rendering. (E.G. OUT_OF_DATE / SUBOPTIMAL in Vulkan, No Equivalent in OpenGL)
         *
         * What exactly is drawn in the screen buffer if a swap chain / framebuffer size doesn't match is not clearly defined in graphics apis
         */
        virtual void swapBuffers() = 0;

        /**
         * The back buffer textures are write-only (No Sampling).
         *
         * @return The resource handle representing the back buffer color.
         */
        virtual Resource<Texture> getBackBufferColor() = 0;

        /**
         * @return The resource handle representing the back buffer depth stencil.
         */
        virtual Resource<Texture> getBackBufferDepthStencil() = 0;

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
         * On vulkan this basically maps to each graph running on separate queues.
         * Aliasing is isolated inside each graph.
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
