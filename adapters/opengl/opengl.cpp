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

#include "xng/adapters/opengl/opengl.hpp"

#include "rendergraphruntimeogl.hpp"

namespace xng::opengl {
    RenderGraphRuntime::RenderGraphRuntime()
        : runtime(std::make_unique<RenderGraphRuntimeOGL>()) {
    }

    void RenderGraphRuntime::setWindow(const std::shared_ptr<Window> window) {
        runtime->setWindow(window);
    }

    Window & RenderGraphRuntime::getWindow() {
        return runtime->getWindow();
    }

    Vec2i RenderGraphRuntime::updateBackBuffer() {
        return runtime->updateBackBuffer();
    }

    Vec2i RenderGraphRuntime::getBackBufferSize() {
        return runtime->getBackBufferSize();
    }

    RenderGraphHandle RenderGraphRuntime::compile(RenderGraph &&graph) {
        return runtime->compile(std::move(graph));
    }

    void RenderGraphRuntime::recompile(const RenderGraphHandle handle, RenderGraph &&graph) {
        runtime->recompile(handle, std::move(graph));
    }

    RenderGraphStatistics RenderGraphRuntime::execute(const RenderGraphHandle graph) {
        return runtime->execute(graph);
    }

    RenderGraphStatistics RenderGraphRuntime::execute(const std::vector<RenderGraphHandle> &graphs) {
        return runtime->execute(graphs);
    }

    void RenderGraphRuntime::destroy(const RenderGraphHandle graph) {
        runtime->destroy(graph);
    }

    void RenderGraphRuntime::saveCache(const RenderGraphHandle graph, std::ostream &stream) {
        runtime->saveCache(graph, stream);
    }

    void RenderGraphRuntime::loadCache(const RenderGraphHandle graph, std::istream &stream) {
        runtime->loadCache(graph, stream);
    }
}
