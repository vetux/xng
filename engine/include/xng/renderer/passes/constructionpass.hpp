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

#ifndef XENGINE_CONSTRUCTIONPASS_HPP
#define XENGINE_CONSTRUCTIONPASS_HPP

#include <utility>

#include "xng/renderer/renderpass.hpp"
#include "xng/renderer/pipeline/indirect/renderpipelinecompilerindirect.hpp"
#include "xng/rendergraph/builder/graphbuilder.hpp"

namespace xng {
    /**
     * Generates the GBuffer for deferred shading.
     */
    class XENGINE_EXPORT ConstructionPass final : public RenderPass {
    public:
        /**
         * The geometry buffer.
         * The shading model ID is stored in the stencil buffer.
         */
        struct GBuffer {
            static std::vector<rg::ColorFormat> getColorFormats() {
                return {
                    rg::ColorFormat::RGBA32F,
                    rg::ColorFormat::RGBA32F,
                    rg::ColorFormat::RGBA32F,
                    rg::ColorFormat::RGBA8,
                    rg::ColorFormat::RGBA8,
                    rg::ColorFormat::RGBA32UI
                };
            }

            explicit GBuffer(rg::GraphBuilder &graph, Vec2u _resolution)
                : resolution(std::move(_resolution)) {
                auto desc = rg::Texture();
                desc.size = resolution;

                desc.format = rg::ColorFormat::RGBA32F;
                position = graph.allocateTexture(desc);
                normal = graph.allocateTexture(desc);
                tangent = graph.allocateTexture(desc);

                desc.format = rg::ColorFormat::RGBA8;
                roughnessMetallicAO = graph.allocateTexture(desc);
                albedo = graph.allocateTexture(desc);

                desc.format = rg::ColorFormat::RGBA32UI;
                objectIdReceiveShadows = graph.allocateTexture(desc);

                desc.format = rg::ColorFormat::DEPTH24_STENCIL8;
                depthStencil = graph.allocateTexture(desc);
            }

            void subscribe(RenderPassRegistry &registry) const {
                registry.set(RenderPassRegistry::G_BUFFER_POSITION, position);
                registry.set(RenderPassRegistry::G_BUFFER_NORMAL, normal);
                registry.set(RenderPassRegistry::G_BUFFER_TANGENT, tangent);
                registry.set(RenderPassRegistry::G_BUFFER_ROUGHNESS_METALLIC_AO, roughnessMetallicAO);
                registry.set(RenderPassRegistry::G_BUFFER_ALBEDO, albedo);
                registry.set(RenderPassRegistry::G_BUFFER_OBJECT_ID_RECEIVE_SHADOWS, objectIdReceiveShadows);
                registry.set(RenderPassRegistry::G_BUFFER_DEPTH, depthStencil);
            }

            Vec2u resolution;

            rg::Resource<rg::Texture> position;
            rg::Resource<rg::Texture> normal;
            rg::Resource<rg::Texture> tangent;
            rg::Resource<rg::Texture> roughnessMetallicAO;
            rg::Resource<rg::Texture> albedo;
            rg::Resource<rg::Texture> objectIdReceiveShadows;
            rg::Resource<rg::Texture> depthStencil;
        };

        static rg::Shader compileVertexShader();

        static rg::Shader compileFragmentShader();

        explicit ConstructionPass(const RenderPipelineCompiler &compiler)
            : ConstructionPass(compiler,
                               compileVertexShader(),
                               compileFragmentShader()) {
        }

        // This is now awkward to compile the shader for the render pipeline.
        // With the offline shader compile phase, I will have to find a clean solution here.
        ConstructionPass(const RenderPipelineCompiler &compiler,
                         const rg::Shader &vertexShader,
                         const rg::Shader &fragmentShader) {
            const auto colorFormats = GBuffer::getColorFormats();
            std::vector<RenderPipelineShader::Attachment> attachments;
            attachments.reserve(6);
            attachments.emplace_back(RenderPipelineShader::Attachment::ATTACHMENT_NATIVE,
                                     rg::ShaderPrimitiveType::vec4(),
                                     colorFormats.at(GBUFFER_POSITION));
            attachments.emplace_back(RenderPipelineShader::Attachment::ATTACHMENT_NATIVE,
                                     rg::ShaderPrimitiveType::vec4(),
                                     colorFormats.at(GBUFFER_NORMAL));
            attachments.emplace_back(RenderPipelineShader::Attachment::ATTACHMENT_NATIVE,
                                     rg::ShaderPrimitiveType::vec4(),
                                     colorFormats.at(GBUFFER_TANGENT));
            attachments.emplace_back(RenderPipelineShader::Attachment::ATTACHMENT_NATIVE,
                                     rg::ShaderPrimitiveType::vec4(),
                                     colorFormats.at(GBUFFER_ROUGHNESS_METALLIC_AO));
            attachments.emplace_back(RenderPipelineShader::Attachment::ATTACHMENT_NATIVE,
                                     rg::ShaderPrimitiveType::vec4(),
                                     colorFormats.at(GBUFFER_ALBEDO));
            attachments.emplace_back(RenderPipelineShader::Attachment::ATTACHMENT_NATIVE,
                                     rg::ShaderPrimitiveType::ivec4(),
                                     colorFormats.at(GBUFFER_OBJECT_ID_RECEIVE_SHADOWS));

            shader = compiler.compile({vertexShader, fragmentShader},
                                      getPipelineConfig(),
                                      attachments,
                                      rg::DEPTH24_STENCIL8,
                                      rg::DEPTH24_STENCIL8);
        }

        ~ConstructionPass() override = default;

        void record(rg::GraphBuilder &graph,
                    const std::shared_ptr<rg::Surface> surface,
                    RenderPassRegistry &registry,
                    const RenderScene &scene) override {
            const GBuffer gBuffer(graph, surface->getDimensions());
            gBuffer.subscribe(registry);

            std::vector<RenderPipeline::Attachment> colorAttachments;
            colorAttachments.reserve(6);
            colorAttachments.emplace_back(rg::Attachment(gBuffer.position, Vec4f(0)));
            colorAttachments.emplace_back(rg::Attachment(gBuffer.normal, Vec4f(0)));
            colorAttachments.emplace_back(rg::Attachment(gBuffer.tangent, Vec4f(0)));
            colorAttachments.emplace_back(rg::Attachment(gBuffer.roughnessMetallicAO, Vec4f(0)));
            colorAttachments.emplace_back(rg::Attachment(gBuffer.albedo, Vec4f(0)));
            colorAttachments.emplace_back(rg::Attachment(gBuffer.objectIdReceiveShadows, Vec4i(0)));

            rg::Attachment depthStencilAttachment(gBuffer.depthStencil,
                                                  rg::Texture::DepthStencilClearValue(1.0f, SHADING_MODEL_NONE));

            scene.getPbrDeferredPipeline().execute(graph,
                                                   "ConstructionPass",
                                                   *shader,
                                                   {{}, gBuffer.resolution.convert<int>()},
                                                   colorAttachments,
                                                   depthStencilAttachment,
                                                   {},
                                                   {},
                                                   {},
                                                   {},
                                                   {});
        }

    private:
        enum AttachmentIndex : int {
            GBUFFER_POSITION = 0,
            GBUFFER_NORMAL,
            GBUFFER_TANGENT,
            GBUFFER_ROUGHNESS_METALLIC_AO,
            GBUFFER_ALBEDO,
            GBUFFER_OBJECT_ID_RECEIVE_SHADOWS,
        };

        static rg::RasterPipeline::Configuration getPipelineConfig() {
            rg::RasterPipeline::Configuration ret;

            ret.enableDepthTest = true;
            ret.depthTestWrite = true;

            ret.enableStencilTest = true;
            ret.enableDynamicStencilReference = true;
            ret.stencilTestMask = 0xFF;
            ret.stencilMode = rg::RasterPipeline::StencilMode::STENCIL_ALWAYS;
            ret.stencilPass = rg::RasterPipeline::StencilAction::STENCIL_REPLACE;
            ret.stencilFail = rg::RasterPipeline::StencilAction::STENCIL_KEEP;
            ret.stencilDepthFail = rg::RasterPipeline::StencilAction::STENCIL_KEEP;

            return ret;
        }

        std::shared_ptr<RenderPipelineShader> shader;
    };
}

#endif //XENGINE_CONSTRUCTIONPASS_HPP
