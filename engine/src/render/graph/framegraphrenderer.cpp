/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2024  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "xng/render/graph/framegraphrenderer.hpp"
#include "xng/render/graph/framegraphbuilder.hpp"

namespace xng {
    FrameGraphRenderer::FrameGraphRenderer(std::unique_ptr<FrameGraphRuntime> runtime)
            : runtime(std::move(runtime)) {}

    void FrameGraphRenderer::render(const Scene &scene) {
        /// Setup
        auto graph = FrameGraphBuilder(runtime->getBackBufferDesc(),
                                    runtime->getRenderDeviceInfo(),
                                    scene,
                                    settings,
                                    persistentResources).build(pipeline.getPasses());

        persistentResources = graph.getPersistentResources();

        /// Compile / Execute
        runtime->execute(graph);
    }
}
