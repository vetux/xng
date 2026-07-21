/**
 *   xEngine - C++ Game Engine Library
 *   Copyright (C) 2026 Julia Zampiccoli
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the Lesser General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef XENGINE_CANVASPASS_HPP
#define XENGINE_CANVASPASS_HPP

#include "xng/renderer/renderpass.hpp"

#include "xng/rendergraph/shader/shader.hpp"
#include "xng/rendergraph/pipelinecache.hpp"

namespace xng {
    /**
     * Renders the canvas textures and presents screen space canvases to a compositing layer.
     */
    class CanvasPass final : public RenderPass {
    public:
        XENGINE_EXPORT static rg::Shader compileVertexShader();

        XENGINE_EXPORT static rg::Shader compileFragmentShader();

        CanvasPass(rg::Heap &heap, rg::PipelineCache &pipelineCache)
            : CanvasPass(heap, pipelineCache, compileVertexShader(), compileFragmentShader()) {
        }

        //TODO: Implement CanvasPass
        CanvasPass(rg::Heap &heap,
                   rg::PipelineCache &pipelineCache,
                   const rg::Shader &vertexShader,
                   const rg::Shader &fragmentShader) {
        }

        void record(rg::GraphBuilder &builder,
                    std::shared_ptr<rg::Surface> surface,
                    RenderPassRegistry &registry,
                    const RenderScene &scene) override {
            const auto colorTexture = builder.allocateTexture(rg::Texture(rg::Texture::CAPABILITY_COLOR_ATTACHMENT
                                                                          | rg::Texture::CAPABILITY_SAMPLED,
                                                                          surface->getDimensions()));

            registry.set(RenderPassRegistry::CANVAS_COLOR, colorTexture);
        }

    private:
        static rg::RasterPipeline::Configuration getPipelineConfig() {
            rg::RasterPipeline::Configuration ret;

            ret.enableDepthTest = false;
            ret.depthTestWrite = false;

            ret.enableBlending = true;

            ret.enableStencilTest = false;
            ret.enableDynamicStencilReference = false;

            std::vector<size_t> offsets;
            offsets.resize(2, 0);

            return ret;
        }

        rg::PipelineCache::Handle pipeline;
    };
}
#endif //XENGINE_CANVASPASS_HPP
