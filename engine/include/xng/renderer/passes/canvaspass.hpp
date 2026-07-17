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
#include "xng/renderer/objects/paint/renderpainttext.hpp"

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

        CanvasPass(rg::Heap &heap,
                   rg::PipelineCache &pipelineCache,
                   const rg::Shader &vertexShader,
                   const rg::Shader &fragmentShader)
            : pipeline(pipelineCache.create(getPipeline(vertexShader, fragmentShader))) {
        }

        void record(rg::GraphBuilder &builder,
                    std::shared_ptr<rg::Surface> surface,
                    RenderPassRegistry &registry,
                    const RenderScene &scene) override {
            const auto colorTexture = builder.allocateTexture(rg::Texture(rg::Texture::CAPABILITY_COLOR_ATTACHMENT
                                                                          | rg::Texture::CAPABILITY_SAMPLED,
                                                                          surface->getDimensions()));

            registry.set(RenderPassRegistry::CANVAS_COLOR, colorTexture);

            for (auto &canvas: scene.canvases) {
                if (canvas->getTexture()) {
                    continue;
                }

                auto pass = rg::RenderPassBuilder("CanvasPass");

                pass.vertexRead(scene.vertexBuffers.at(POSITION));
                pass.vertexRead(scene.vertexBuffers.at(UV));
                pass.indexRead(scene.indexBuffer);

                pass.textureSampledRead(scene.textureAtlas, {rg::Shader::FRAGMENT});

                pass.attachColor(rg::Attachment(colorTexture, Vec4f(0)));

                builder.addPass(pass.execute([this, scene, canvas, colorTexture](rg::RasterContext &ctx) {
                    ctx.bindPipeline(pipeline);
                    ctx.setViewport({}, colorTexture.getDescription().size);

                    // Bind Vertex Buffers
                    ctx.bindVertexBuffer(scene.vertexBuffers.at(POSITION), 0, 0, getVertexAttributeSize(POSITION));
                    ctx.bindVertexBuffer(scene.vertexBuffers.at(UV), 1, 0, getVertexAttributeSize(UV));

                    // Bind Index Buffer
                    ctx.bindIndexBuffer(scene.indexBuffer, rg::INDEX_UNSIGNED_INT);

                    ctx.bindTexture("atlasTexture", {rg::TextureBinding(scene.textureAtlas)});
                    ctx.bindStorageBuffer("tileMap", scene.tileMapBuffer, 0, 0);
                    ctx.bindStorageBuffer("tileMapOffsets", scene.tileMapOffsetsBuffer, 0, 0);
                    ctx.bindStorageBuffer("residencyMap", scene.residencyMapBuffer, 0, 0);
                    ctx.bindStorageBuffer("residencyMapOffsets", scene.residencyMapOffsetsBuffer, 0, 0);
                    ctx.bindStorageBuffer("readbackBuffer", scene.readbackBuffer, 0, 0);

                    ctx.setShaderParameter("atlasSize", rg::ShaderPrimitive(scene.atlasSize));
                    ctx.setShaderParameter("tileSize", rg::ShaderPrimitive(scene.tileSize));
                    ctx.setShaderParameter("tileBorder", rg::ShaderPrimitive(scene.tileBorder));
                    ctx.setShaderParameter("maxAnisotropy", rg::ShaderPrimitive(scene.maxAnisotropy));

                    Mat4f viewProjection = canvas->getLocalProjection();

                    for (auto &paint: canvas->getPaints()) {
                        switch (paint->getType()) {
                            default:
                                throw std::runtime_error("Unsupported paint type");
                            case RenderObject::OBJECT_PAINT_TEXT: {
                                const auto &text = down_cast<RenderPaintText &>(*paint);
                                ctx.setShaderParameter("color", rg::ShaderPrimitive(text.getColor().divide()));
                                auto minFilter_magFilter_mipFilter_wrap = Vec4i(text.getSamplingProperties().minFilter,
                                    text.getSamplingProperties().magFilter,
                                    text.getSamplingProperties().mipFilter,
                                    text.getSamplingProperties().wrapping);
                                ctx.setShaderParameter("minFilter_magFilter_mipFilter_wrap",
                                                       rg::ShaderPrimitive(minFilter_magFilter_mipFilter_wrap));
                                for (auto &c: text.getChars()) {
                                    if (c.textureSize.x == 0 || c.textureSize.y == 0) {
                                        continue;
                                    }

                                    auto textureSize_textureID_maxMip = Vec4i(c.textureSize.x, c.textureSize.y,
                                                                              c.texture,
                                                                              1);
                                    ctx.setShaderParameter("textureSize_textureID_maxMip",
                                                           rg::ShaderPrimitive(textureSize_textureID_maxMip));
                                    ctx.setShaderParameter("mvp", rg::ShaderPrimitive(viewProjection * c.modelMatrix));
                                    ctx.setShaderParameter("grayscale", rg::ShaderPrimitive(c.grayscale));
                                    ctx.drawIndexed(scene.normalizedQuad.drawCall, scene.normalizedQuad.baseVertex);
                                }
                                break;
                            }
                        }
                    }
                }));
            }
        }

    private:
        static rg::RasterPipeline getPipeline(const rg::Shader &vertexShader, const rg::Shader &fragmentShader) {
            rg::RasterPipeline ret;
            ret.colorAttachments = {rg::ColorFormat::RGBA8};
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
#endif //XENGINE_CANVASPASS_HPP
