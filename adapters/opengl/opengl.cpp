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

#include "xng/adapters/opengl/opengl.hpp"

#include "glsl/shadercompilerglsl.hpp"
#include "contextgl.hpp"

namespace xng::opengl {
    struct GraphContext {
        RenderGraph graph;
        std::unordered_map<RenderGraphResource, RenderGraphPipeline, RenderGraphResourceHash> pipelines;
        std::unordered_map<RenderGraphResource, CompiledPipeline, RenderGraphResourceHash> compiledPipelines;
    };

    struct State {
        std::unordered_map<RenderGraphRuntime::GraphHandle, GraphContext> contexts;
    };

    OpenGL::OpenGL() {
        state = std::make_unique<State>();
    }

    OpenGL::~OpenGL() {
    }

    void OpenGL::setWindow(const Window &window) {
    }

    RenderGraphRuntime::GraphHandle OpenGL::compile(const RenderGraph &graph) {
        return compileGraph(graph);
    }

    void OpenGL::recompile(GraphHandle handle, const RenderGraph &graph) {
    }

    void OpenGL::execute(GraphHandle graph) {
        ContextGL context(state->contexts.at(graph).compiledPipelines);
        for (auto pass: state->contexts[graph].graph.passes) {
            pass.pass(context);
        }
    }

    void OpenGL::execute(std::vector<GraphHandle> graphs) {
        for (auto graph: graphs) {
            execute(graph);
        }
    }

    void OpenGL::saveCache(GraphHandle graph, std::ostream &stream) {
    }

    void OpenGL::loadCache(GraphHandle graph, std::istream &stream) {
    }

    RenderGraphRuntime::GraphHandle OpenGL::compileGraph(const RenderGraph &graph) {
        const auto handle = graphCounter++;

        auto context = GraphContext();

        context.graph = graph;

        ShaderCompilerGLSL compiler;
        for (auto pair: graph.pipelineAllocation) {
            context.pipelines[pair.first] = pair.second;
            context.compiledPipelines[pair.first] = compiler.compile(pair.second.shaders);
        }

        state->contexts[handle] = context;

        return handle;
    }
}
