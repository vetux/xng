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

#include "xng/adapters/opengl2/opengl2.hpp"

#include "glsl/shadercompilerglsl.hpp"
#include "contextgl.hpp"

namespace xng::opengl2 {
    OpenGL2::OpenGL2() {
    }

    OpenGL2::~OpenGL2() {
    }

    void OpenGL2::setWindow(const Window &window) {
    }

    FGRuntime::GraphHandle OpenGL2::compile(const FGGraph &graph) {
        return compileGraph(graph);
    }

    void OpenGL2::recompile(GraphHandle handle, const FGGraph &graph) {
    }

    void OpenGL2::execute(GraphHandle graph) {
        ContextGL context(shaders.at(graph));
        for (auto pass : graphs.at(graph).passes) {
            pass.pass(context);
        }
    }

    void OpenGL2::execute(std::vector<GraphHandle> graphs) {
        for (auto graph : graphs) {
            execute(graph);
        }
    }

    void OpenGL2::saveCache(GraphHandle graph, std::ostream &stream) {
    }

    void OpenGL2::loadCache(GraphHandle graph, std::istream &stream) {
    }

    FGRuntime::GraphHandle OpenGL2::compileGraph(const FGGraph &graph) {
        const auto handle = graphCounter++;
        graphs[handle] = graph;

        ShaderCompilerGLSL compiler;
        for (auto pair : graph.shaderAllocation) {
            shaders[handle][pair.first] = compiler.compile(pair.second);
        }
        return handle;
    }
}
