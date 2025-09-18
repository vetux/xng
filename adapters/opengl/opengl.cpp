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

#include "rendergraphruntimeogl.hpp"

namespace xng::opengl {
    OpenGL::OpenGL()
        : runtime(std::make_unique<RenderGraphRuntimeOGL>()) {
    }

    void OpenGL::setWindow(Window &window) {
        runtime->setWindow(window);
    }

    Window & OpenGL::getWindow() {
        return runtime->getWindow();
    }

    RenderGraphRuntime::GraphHandle OpenGL::compile(const RenderGraph &graph) {
        return runtime->compile(graph);
    }

    void OpenGL::recompile(const GraphHandle handle, const RenderGraph &graph) {
        runtime->recompile(handle, graph);
    }

    void OpenGL::execute(const GraphHandle graph) {
        runtime->execute(graph);
    }

    void OpenGL::execute(const std::vector<GraphHandle> &graphs) {
        runtime->execute(graphs);
    }

    void OpenGL::destroy(const GraphHandle graph) {
        runtime->destroy(graph);
    }

    void OpenGL::saveCache(const GraphHandle graph, std::ostream &stream) {
        runtime->saveCache(graph, stream);
    }

    void OpenGL::loadCache(const GraphHandle graph, std::istream &stream) {
        runtime->loadCache(graph, stream);
    }
}
