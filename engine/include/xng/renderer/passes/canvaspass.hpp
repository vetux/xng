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

        explicit CanvasPass(const RenderPipelineCompiler &compiler)
            : CanvasPass(compiler, compileVertexShader(), compileFragmentShader()) {
        }

        CanvasPass(const RenderPipelineCompiler &compiler,
                   const rg::Shader &vertexShader,
                   const rg::Shader &fragmentShader) {
            std::vector<RenderPipelineShader::Attachment> attachments;
            attachments.emplace_back(RenderPipelineShader::Attachment::ATTACHMENT_NATIVE,
                                     rg::ShaderPrimitiveType::vec4(),
                                     rg::RGBA8);

            shader = compiler.compile({vertexShader, fragmentShader},
                                      getPipelineConfig(),
                                      attachments,
                                      {},
                                      {});
        }

        void record(rg::GraphBuilder &builder,
                    std::shared_ptr<rg::Surface> surface,
                    RenderPassRegistry &registry,
                    const RenderScene &scene) override {
            const auto colorTexture = builder.allocateTexture(rg::Texture(rg::Texture::CAPABILITY_COLOR_ATTACHMENT
                                                                          | rg::Texture::CAPABILITY_SAMPLED,
                                                                          surface->getDimensions()));

            registry.set(RenderPassRegistry::CANVAS_COLOR, colorTexture);

            for (auto &pair: scene.getCanvases()) {
                std::vector<RenderPipeline::Attachment> colorAttachments;
                if (pair.second.getTexture().isAssigned()) {
                    // TODO: Texture Canvas path
                    continue;
                } else {
                    colorAttachments.emplace_back(rg::Attachment(colorTexture, Vec4f(0)));
                }

                pair.second.getPipeline().execute(builder,
                                                  "CanvasPass",
                                                  *shader,
                                                  {{}, colorTexture.getDescription().size.convert<int>()},
                                                  colorAttachments,
                                                  {},
                                                  {},
                                                  {},
                                                  {},
                                                  {},
                                                  {});
            }
        }

    private:
        static rg::RasterPipeline::Configuration getPipelineConfig() {
            rg::RasterPipeline::Configuration ret;

            ret.enableDepthTest = false;
            ret.depthTestWrite = false;

            ret.enableBlending = true;

            ret.enableStencilTest = false;
            ret.enableDynamicStencilReference = false;

            return ret;
        }

        std::shared_ptr<RenderPipelineShader> shader;
    };
}
#endif //XENGINE_CANVASPASS_HPP
