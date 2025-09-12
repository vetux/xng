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

#ifndef XENGINE_OPENGL_HPP
#define XENGINE_OPENGL_HPP

#include "xng/rendergraph/rendergraphruntime.hpp"

namespace xng::opengl {
    struct State;

    class XENGINE_EXPORT OpenGL final : public RenderGraphRuntime {
    public:
        OpenGL();

        ~OpenGL() override;

        void setWindow(const Window &window) override;

        GraphHandle compile(const RenderGraph &graph) override;

        void recompile(GraphHandle handle, const RenderGraph &graph) override;

        void execute(GraphHandle graph) override;

        void execute(std::vector<GraphHandle> graphs) override;

        void saveCache(GraphHandle graph, std::ostream &stream) override;

        void loadCache(GraphHandle graph, std::istream &stream) override;

    private:
        GraphHandle compileGraph(const RenderGraph &graph);

        GraphHandle graphCounter = 0;

        std::unordered_map<GraphHandle, RenderGraph> graphs;
        std::unordered_map<GraphHandle, std::unordered_map<RenderGraphResource, ShaderStage>> shaders;

        std::unique_ptr<State> state;
    };
}

#endif //XENGINE_OPENGL_HPP
