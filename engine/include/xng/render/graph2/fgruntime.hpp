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

#ifndef XENGINE_FG2RUNTIME_HPP
#define XENGINE_FG2RUNTIME_HPP

#include "xng/render/graph2/fggraph.hpp"

#include "xng/display/window.hpp"

namespace xng {
    /**
     * The runtime / context represents the platform-dependent implementation of the renderer.
     */
    class FGRuntime {
    public:
        typedef int GraphHandle;

        /**
         * Set the window to render to.
         *
         * @param window
         */
        virtual void setWindow(const Window &window) = 0;

        virtual GraphHandle compile(const FGGraph &graph) = 0;

        /**
         * Execute the graph.
         *
         * Performs cleanup of unused exported resources.
         *
         * @param graph
         */
        virtual void execute(GraphHandle graph) = 0;

        /**
         * Execute multiple graphs.
         *
         * The order in which the graphs execute is determined by the defined dependencies between the graphs.
         *
         * The graphs could be executed in parallel if the platform supports it.
         *
         * Performs cleanup of unused exported resources.
         *
         * @param graphs
         */
        virtual void execute(std::vector<GraphHandle> graphs) = 0;

        virtual void saveCache(GraphHandle graph, std::ostream &stream) = 0;

        virtual void loadCache(GraphHandle graph, std::istream &stream) = 0;
    };
}

#endif //XENGINE_FG2RUNTIME_HPP
