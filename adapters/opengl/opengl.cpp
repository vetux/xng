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

    void OpenGL::setWindow(const std::shared_ptr<Window> window) {
        runtime->setWindow(window);
    }

    Window & OpenGL::getWindow() {
        return runtime->getWindow();
    }

    Vec2i OpenGL::updateBackBuffer() {
        return runtime->updateBackBuffer();
    }

    Vec2i OpenGL::getBackBufferSize() {
        return runtime->getBackBufferSize();
    }

    RenderGraphHandle OpenGL::compile(const RenderGraph &graph) {
        return runtime->compile(graph);
    }

    void OpenGL::recompile(const RenderGraphHandle handle, const RenderGraph &graph) {
        runtime->recompile(handle, graph);
    }

    void OpenGL::execute(const RenderGraphHandle graph) {
        runtime->execute(graph);
    }

    void OpenGL::execute(const std::vector<RenderGraphHandle> &graphs) {
        runtime->execute(graphs);
    }

    void OpenGL::destroy(const RenderGraphHandle graph) {
        runtime->destroy(graph);
    }

    void OpenGL::saveCache(const RenderGraphHandle graph, std::ostream &stream) {
        runtime->saveCache(graph, stream);
    }

    void OpenGL::loadCache(const RenderGraphHandle graph, std::istream &stream) {
        runtime->loadCache(graph, stream);
    }
}
