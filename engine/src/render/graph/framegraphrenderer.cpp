/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2022  Julian Zampiccoli
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

#include "render/graph/framegraphrenderer.hpp"
#include "render/graph/framegraphbuilder.hpp"

#include "render/graph/framegraphproperties.hpp"

namespace xng {
    FrameGraphRenderer::FrameGraphRenderer(std::unique_ptr<FrameGraphAllocator> allocator)
            : allocator(std::move(allocator)) {}

    void FrameGraphRenderer::render(RenderTarget &target,
                                    const Scene &scene) {
        /// Setup
        auto frame = FrameGraphBuilder(target, scene, properties).build(layout);

        blackboard.clear();

        /// Compile
        allocator->setFrame(frame);

        /// Execute
        for (auto &p: layout.getOrderedPasses()) {
            auto res = allocator->allocateNext();
            p.get().execute(res);
        }
    }

    void FrameGraphRenderer::setRenderResolution(Vec2i res) {
        properties.set(FrameGraphProperties::RENDER_RESOLUTION, res);
    }

    void FrameGraphRenderer::setRenderSamples(int samples) {
        properties.set(FrameGraphProperties::RENDER_SAMPLES, samples);
    }
}
