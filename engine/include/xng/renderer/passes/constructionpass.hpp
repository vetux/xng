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

        explicit ConstructionPass(rg::PipelineCache &pipelineCache)
            : ConstructionPass(pipelineCache,
                               compileVertexShader(),
                               compileFragmentShader()) {
        }

        ConstructionPass(rg::PipelineCache &pipelineCache,
                         const rg::Shader &vertexShader,
                         const rg::Shader &fragmentShader)
            : pipelineCache(pipelineCache) {
            pipeline = pipelineCache.create(getPipeline(vertexShader, fragmentShader));
        }

        ~ConstructionPass() override {
            pipelineCache.destroy(pipeline);
        }

        void record(rg::GraphBuilder &graph,
                    const std::shared_ptr<rg::Surface> surface,
                    RenderPassRegistry &registry,
                    const RenderScene &scene) override {
            const GBuffer gBuffer(graph, surface->getDimensions());
            gBuffer.subscribe(registry);
            graph.addPass(createPass(scene, gBuffer));
        }

    private:
        [[nodiscard]] rg::RenderPass createPass(const RenderScene &scene, const GBuffer &gBuffer) const {
            rg::RenderPassBuilder builder("ConstructionPass");

            // Set Attachments
            builder.attachColor(rg::Attachment(gBuffer.position, Vec4f(0)))
                    .attachColor(rg::Attachment(gBuffer.normal, Vec4f(0)))
                    .attachColor(rg::Attachment(gBuffer.tangent, Vec4f(0)))
                    .attachColor(rg::Attachment(gBuffer.roughnessMetallicAO, Vec4f(0)))
                    .attachColor(rg::Attachment(gBuffer.albedo, Vec4f(0)))
                    .attachColor(rg::Attachment(gBuffer.objectIdReceiveShadows, Vec4i(0)))
                    .attachDepthStencil(rg::Attachment(gBuffer.depthStencil,
                                                       rg::Texture::DepthStencilClearValue(1.0f, SHADING_MODEL_NONE)));

            // Declare Accesses
            builder.storageRead(scene.cameraBuffer, {rg::Shader::VERTEX});

            size_t totalDrawCount = 0;
            for (auto &batch: scene.drawList) {
                if (batch.renderPath != RENDER_PATH_DEFERRED) {
                    continue;
                }

                totalDrawCount++;

                for (auto &access: batch.drawBufferAccesses) {
                    builder.storageRead(scene.drawBuffer, {rg::Shader::VERTEX}, access.offset, access.size);
                }

                for (auto &access: batch.transformBufferAccesses) {
                    builder.storageRead(scene.transformBuffer, {rg::Shader::VERTEX}, access.offset, access.size);
                }

                for (auto &access: batch.materialBufferAccesses) {
                    builder.storageRead(scene.materialBuffer, {rg::Shader::FRAGMENT}, access.offset, access.size);
                }

                for (auto &pair: batch.vertexBufferAccesses) {
                    for (auto &access: pair.second) {
                        builder.vertexRead(scene.vertexBuffers.at(pair.first), access.offset, access.size);
                    }
                }

                for (auto &access: batch.indexBufferAccesses) {
                    builder.indexRead(scene.indexBuffer, access.offset, access.size);
                }

                for (auto &layer: batch.textureAccesses) {
                    builder.textureSampledRead(scene.textureAtlas,
                                               {rg::Shader::FRAGMENT},
                                               rg::TextureBinding::Range(0,
                                                                         scene.textureAtlas.
                                                                         getDescription().
                                                                         mipLevels,
                                                                         layer,
                                                                         1));
                }

                builder.storageRead(scene.tileMapBuffer, {rg::Shader::FRAGMENT});
                builder.storageRead(scene.tileMapOffsetsBuffer, {rg::Shader::FRAGMENT});
                builder.storageRead(scene.residencyMapBuffer, {rg::Shader::FRAGMENT});
                builder.storageRead(scene.residencyMapOffsetsBuffer, {rg::Shader::FRAGMENT});
                builder.storageWrite(scene.readbackBuffer, {rg::Shader::FRAGMENT});
            }

            if (totalDrawCount <= 0) {
                return builder.execute([](rg::RasterContext &cmd) {
                });
            }

            return builder.execute([this, &scene, gBuffer](rg::RasterContext &cmd) {
                // Bind pipeline
                cmd.bindPipeline(pipeline);

                cmd.setViewport({}, gBuffer.resolution);

                // Bind Vertex Buffers
                for (auto attr = ATTRIBUTE_BEGIN;
                     attr <= ATTRIBUTE_END;
                     attr = static_cast<VertexAttribute>(attr + 1)) {
                    cmd.bindVertexBuffer(scene.vertexBuffers.at(attr),
                                         attr,
                                         0,
                                         getVertexAttributeSize(attr));
                }

                // Bind Index Buffer
                cmd.bindIndexBuffer(scene.indexBuffer, rg::INDEX_UNSIGNED_INT);

                // Bind storage buffers
                cmd.bindStorageBuffer("drawBuffer",
                                      scene.drawBuffer,
                                      0,
                                      scene.drawBuffer.getDescription().size);

                cmd.bindStorageBuffer("transforms",
                                      scene.transformBuffer,
                                      0,
                                      scene.transformBuffer.getDescription().size);

                cmd.bindStorageBuffer("materials",
                                      scene.materialBuffer,
                                      0,
                                      scene.materialBuffer.getDescription().size);

                cmd.bindTexture("atlasTexture", {rg::TextureBinding(scene.textureAtlas)});
                cmd.bindStorageBuffer("tileMap", scene.tileMapBuffer, 0, 0);
                cmd.bindStorageBuffer("tileMapOffsets", scene.tileMapOffsetsBuffer, 0, 0);
                cmd.bindStorageBuffer("residencyMap", scene.residencyMapBuffer, 0, 0);
                cmd.bindStorageBuffer("residencyMapOffsets", scene.residencyMapOffsetsBuffer, 0, 0);
                cmd.bindStorageBuffer("readbackBuffer", scene.readbackBuffer, 0, 0);

                cmd.setShaderParameter("atlasSize", rg::ShaderPrimitive(scene.atlasSize));
                cmd.setShaderParameter("tileSize", rg::ShaderPrimitive(scene.tileSize));
                cmd.setShaderParameter("tileBorder", rg::ShaderPrimitive(scene.tileBorder));
                cmd.setShaderParameter("maxAnisotropy", rg::ShaderPrimitive(scene.maxAnisotropy));

                for (auto &batch: scene.drawList) {
                    if (batch.renderPath != RENDER_PATH_DEFERRED) {
                        continue;
                    }
                    cmd.setStencilReference(batch.shadingModel);
                    cmd.drawIndexedMultiIndirectCount(batch.indirectBuffer,
                                                      batch.indirectCountBuffer,
                                                      batch.indirectBufferOffset,
                                                      batch.indirectCountBufferOffset,
                                                      batch.batchSize,
                                                      batch.stride);
                }
            });
        }

        static rg::RasterPipeline getPipeline(const rg::Shader &vertexShader, const rg::Shader &fragmentShader) {
            rg::RasterPipeline ret;
            ret.shaders = {vertexShader, fragmentShader};

            ret.enableDepthTest = true;
            ret.depthTestWrite = true;

            ret.enableStencilTest = true;
            ret.enableDynamicStencilReference = true;
            ret.stencilTestMask = 0xFF;
            ret.stencilMode = rg::RasterPipeline::StencilMode::STENCIL_ALWAYS;
            ret.stencilPass = rg::RasterPipeline::StencilAction::STENCIL_REPLACE;
            ret.stencilFail = rg::RasterPipeline::StencilAction::STENCIL_KEEP;
            ret.stencilDepthFail = rg::RasterPipeline::StencilAction::STENCIL_KEEP;

            ret.colorAttachments = GBuffer::getColorFormats();
            ret.depthAttachment = rg::ColorFormat::DEPTH24_STENCIL8;
            ret.stencilAttachment = rg::ColorFormat::DEPTH24_STENCIL8;

            std::vector<size_t> offsets;
            offsets.resize(5, 0);

            ret.vertexFormat = rg::RasterPipeline::VertexFormat(vertexShader.inputLayout,
                                                                {
                                                                    POSITION,
                                                                    NORMAL,
                                                                    UV,
                                                                    TANGENT,
                                                                    BITANGENT,
                                                                },
                                                                offsets);
            return ret;
        }

        rg::PipelineCache &pipelineCache;
        rg::PipelineCache::Handle pipeline;
    };
}

#endif //XENGINE_CONSTRUCTIONPASS_HPP
