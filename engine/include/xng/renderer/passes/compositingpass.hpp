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

#ifndef XENGINE_COMPOSITINGPASS_HPP
#define XENGINE_COMPOSITINGPASS_HPP

#include "xng/renderer/renderpass.hpp"

namespace xng {
    /**
     * Blends the compositing layers and presents the results on the surface.
     */
    class CompositingPass final : public RenderPass {
    public:
        XENGINE_EXPORT static rg::Shader compileVertexShader();

        XENGINE_EXPORT static rg::Shader compileFragmentShader();

        explicit CompositingPass(rg::PipelineCache &pipelineCache)
            : CompositingPass(pipelineCache, compileVertexShader(), compileFragmentShader()) {
        }

        CompositingPass(rg::PipelineCache &pipelineCache,
                        const rg::Shader &vertexShader,
                        const rg::Shader &fragmentShader) {
            pipeline = pipelineCache.create(getPipeline(vertexShader, fragmentShader));
        }

        void record(rg::GraphBuilder &builder,
                    const std::shared_ptr<rg::Surface> surface,
                    RenderPassRegistry &registry,
                    const RenderScene &scene) override {
            auto pass = rg::RasterPassBuilder("CompositingPass")
                    .attachColor(rg::Attachment(surface, Vec4f(0)))
                    .vertexRead(scene.vertexBuffers.at(POSITION))
                    .vertexRead(scene.vertexBuffers.at(UV))
                    .indexRead(scene.indexBuffer);

            std::vector<rg::Resource<rg::Texture> > textures;
            if (registry.checkTexture(RenderPassRegistry::PBR_COLOR_DEFERRED)) {
                textures.push_back(registry.getTexture(RenderPassRegistry::PBR_COLOR_DEFERRED));
            }
            if (registry.checkTexture(RenderPassRegistry::CANVAS_COLOR)) {
                textures.push_back(registry.getTexture(RenderPassRegistry::CANVAS_COLOR));
            }

            for (auto &tex: textures) {
                pass.textureSampledRead(tex, {rg::Shader::FRAGMENT});
            }

            pass.attachColor(rg::Attachment(surface, Vec4f(0)));

            builder.addPass(pass.execute([this, textures, surface, scene](rg::RasterContext &ctx) {
                ctx.bindPipeline(pipeline);
                ctx.setViewport({}, surface->getDimensions());

                // Bind Vertex Buffers
                ctx.bindVertexBuffer(scene.vertexBuffers.at(POSITION), 0, 0, getVertexAttributeSize(POSITION));
                ctx.bindVertexBuffer(scene.vertexBuffers.at(UV), 1, 0, getVertexAttributeSize(UV));

                // Bind Index Buffer
                ctx.bindIndexBuffer(scene.indexBuffer, rg::INDEX_UNSIGNED_INT);

                for (auto &tex: textures) {
                    ctx.bindTexture("texture", {rg::TextureBinding(tex)});
                    ctx.drawIndexed(scene.normalizedQuad.drawCall, scene.normalizedQuad.baseVertex);
                }
            }));
        }

    private:
        static rg::RasterPipeline getPipeline(const rg::Shader &vertexShader, const rg::Shader &fragmentShader) {
            rg::RasterPipeline ret;
            ret.shaders = {vertexShader, fragmentShader};
            ret.enableDepthTest = false;
            ret.depthTestWrite = false;
            ret.enableBlending = true;
            ret.enableStencilTest = false;
            ret.enableDynamicStencilReference = false;
            std::vector<size_t> offsets;
            offsets.resize(2, 0);

            ret.vertexFormat = rg::RasterPipeline::VertexFormat(vertexShader.inputLayout,
                                                                {
                                                                    POSITION,
                                                                    UV,
                                                                },
                                                                offsets);
            return ret;
        }

        rg::PipelineCache::Handle pipeline;
    };
}
#endif //XENGINE_COMPOSITINGPASS_HPP
