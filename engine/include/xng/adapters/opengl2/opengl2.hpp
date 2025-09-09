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

#ifndef XENGINE_OPENGL2_HPP
#define XENGINE_OPENGL2_HPP

#include "xng/render/graph2/fgruntime.hpp"

namespace xng::opengl2 {
    struct State;

    class XENGINE_EXPORT OpenGL2 final : public FGRuntime {
    public:
        OpenGL2();

        ~OpenGL2() override;

        void setWindow(const Window &window) override;

        GraphHandle compile(const FGGraph &graph) override;

        void recompile(GraphHandle handle, const FGGraph &graph) override;

        void execute(GraphHandle graph) override;

        void execute(std::vector<GraphHandle> graphs) override;

        void saveCache(GraphHandle graph, std::ostream &stream) override;

        void loadCache(GraphHandle graph, std::istream &stream) override;

    private:
        GraphHandle compileGraph(const FGGraph &graph);

        GraphHandle graphCounter = 0;

        std::unordered_map<GraphHandle, FGGraph> graphs;
        std::unordered_map<GraphHandle, std::unordered_map<FGResource, FGShaderSource>> shaders;

        std::unique_ptr<State> state;
    };
}

#endif //XENGINE_OPENGL2_HPP
