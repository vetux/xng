/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2023  Julian Zampiccoli
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
    FrameGraphRenderer::FrameGraphRenderer(RenderTarget &target,
                                           RenderDevice &device,
                                           std::unique_ptr<FrameGraphAllocator> allocator,
                                           ShaderCompiler &shaderCompiler,
                                           ShaderDecompiler &shaderDecompiler)
            : target(target), device(device), allocator(std::move(allocator)), shaderCompiler(shaderCompiler),
              shaderDecompiler(shaderDecompiler) {}

    void FrameGraphRenderer::render(const Scene &scene) {
        /// Setup
        frame = FrameGraphBuilder(target,
                                  device.getInfo(),
                                  scene,
                                  properties,
                                  frame.getPersistentResources(),
                                  shaderCompiler,
                                  shaderDecompiler).build(pipeline.getPasses());

        blackboard.clear();

        /// Compile
        allocator->beginFrame(frame);

        /// Execute
        for (auto &p: pipeline.getPasses()) {
            auto res = allocator->allocateNextPass();
            p->execute(res);
        }
    }
}
