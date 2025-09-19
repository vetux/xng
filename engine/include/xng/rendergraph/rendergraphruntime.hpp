/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2023  Julian Zampiccoli
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

#ifndef XENGINE_RENDERGRAPHRUNTIME_HPP
#define XENGINE_RENDERGRAPHRUNTIME_HPP

#include "xng/rendergraph/rendergraph.hpp"

#include "xng/display/window.hpp"
#include "xng/display/graphicsapi.hpp"

namespace xng {
    /**
     * The runtime / context represents the platform-dependent implementation of the renderer.
     *
     * The runtime manages an offscreen back buffer representing the window contents for any given execute() invocation
     * and presents the color component of the offscreen back buffer to the window / swap chain before returning from an execute() call.
     *
     * At the start of an execute() call, the contents of this offscreen back buffer are undefined. (May or may not contain previous frame data)
     *
     * On OpenGL this is implemented as offscreen texture buffers.
     *      This means one color texture is wasted/duplicated at a minimum because the window framebuffer cannot be accessed directly through the render graph interface.
     *
     * On Vulkan the back buffer color image is the next image in the swap chain, and depth stencil is an offscreen image, so no texture memory is wasted.
     *
     * The offscreen back buffer enables cross-graph access to the screen contents.
     */
    class RenderGraphRuntime {
    public:
        virtual ~RenderGraphRuntime() = default;

        typedef int GraphHandle;

        /**
         * Set the window to render to.
         *
         * @param window
         */
        virtual void setWindow(std::shared_ptr<Window> window) = 0;

        virtual Window &getWindow() = 0;

        /**
         * Compile a graph.
         *
         * @param graph The graph to compile
         * @return The handle identifying the compiled graph
         */
        virtual GraphHandle compile(const RenderGraph &graph) = 0;

        /**
         * Recompile a graph.
         *
         * Resources allocated in a previous compile() or recompile() can be inherited by the passed graph object.
         *
         * Inherited resources and their state are preserved in the recompiled graph.
         *
         * @param handle The handle of a previously compiled graph
         * @param graph The updated graph
         */
        virtual void recompile(GraphHandle handle, const RenderGraph &graph) = 0;

        /**
         * Execute a single graph.
         *
         * Passes run in parallel where possible.
         * Resource Read/Write is synchronized according to the specified order of the passes.
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
        virtual void execute(GraphHandle graph) = 0;

        /**
         * Execute multiple graphs.
         *
         * The graphs run in parallel where possible.
         * Read/Write to the screen texture is synchronized in the specified order.
         *
         *  For Example,
         *      graphs[0] write to back buffer
         *      graphs[1] read and write the back buffer
         *      graphs[2] read the back buffer
         *      graphs[3] read and write the back buffer
         *
         *      graphs[1] will receive the changes to the back buffer from graph[0]
         *      graphs[2] will receive the changes to the back buffer from graph[1]
         *      graphs[3] will receive the changes to the back buffer from graph[1] and the read operation might run in parallel with graphs[2]
         * @param graphs
         */
        virtual void execute(const std::vector<GraphHandle> &graphs) = 0;

        virtual void destroy(GraphHandle graph) = 0;

        virtual void saveCache(GraphHandle graph, std::ostream &stream) = 0;

        virtual void loadCache(GraphHandle graph, std::istream &stream) = 0;

        virtual GraphicsAPI getGraphicsAPI() = 0;
    };
}

#endif //XENGINE_RENDERGRAPHRUNTIME_HPP
