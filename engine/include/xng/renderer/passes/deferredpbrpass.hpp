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

#ifndef XENGINE_DEFERREDPBRPASS_HPP
#define XENGINE_DEFERREDPBRPASS_HPP

#include "xng/renderer/renderpass.hpp"

namespace xng {
    /**
     * Performs deferred shading of the PBR shading model using the outputs from the
     * Construction / Shadow, and IBL passes and stores the result in a compositing layer.
     */
    class XENGINE_EXPORT DeferredPBRPass final : public RenderPass {
    public:
        static rg::Shader compileVertexShader();

        static rg::Shader compileFragmentShader();

        DeferredPBRPass(rg::Heap &heap, rg::PipelineCache &pipelineCache)
            : DeferredPBRPass(heap, pipelineCache, compileVertexShader(), compileFragmentShader()) {
        }

        DeferredPBRPass(rg::Heap &heap,
                        rg::PipelineCache &pipelineCache,
                        const rg::Shader &vertexShader,
                        const rg::Shader &fragmentShader)
            : pipelineCache(pipelineCache),
              pipeline(pipelineCache.create(getPipeline(vertexShader, fragmentShader))) {
            createUnitQuad(heap);
        }

        void record(rg::GraphBuilder &builder,
                    std::shared_ptr<rg::Surface> surface,
                    RenderPassRegistry &registry,
                    const RenderScene &scene) override {
            auto &gPosition = registry.getTexture(RenderPassRegistry::G_BUFFER_POSITION);
            auto &gNormal = registry.getTexture(RenderPassRegistry::G_BUFFER_NORMAL);
            auto &gTangent = registry.getTexture(RenderPassRegistry::G_BUFFER_TANGENT);
            auto &gRoughnessMetallicAO = registry.getTexture(RenderPassRegistry::G_BUFFER_ROUGHNESS_METALLIC_AO);
            auto &gAlbedo = registry.getTexture(RenderPassRegistry::G_BUFFER_ALBEDO);
            auto &gObjectIdReceiveShadows = registry.getTexture(RenderPassRegistry::G_BUFFER_OBJECT_ID_RECEIVE_SHADOWS);
            auto &gDepth = registry.getTexture(RenderPassRegistry::G_BUFFER_DEPTH);

            const auto colorTexture = builder.allocateTexture(rg::Texture(rg::Texture::CAPABILITY_COLOR_ATTACHMENT,
                                                                          surface->getDimensions()));

            registry.set(RenderPassRegistry::PBR_COLOR_DEFERRED, colorTexture);

            builder.addPass(rg::RasterPassBuilder("DeferredPBRPass")
                .attachColor(rg::Attachment(colorTexture, Vec4f(0)))
                .attachDepthStencil(rg::Attachment(gDepth))
                .storageRead(scene.cameraBuffer, {rg::Shader::FRAGMENT})
                .storageRead(scene.configBuffer, {rg::Shader::FRAGMENT})
                .storageRead(scene.pointLightBuffer, {rg::Shader::FRAGMENT})
                .storageRead(scene.directionalLightBuffer, {rg::Shader::FRAGMENT})
                .storageRead(scene.spotLightBuffer, {rg::Shader::FRAGMENT})
                .textureSampledRead(gPosition, {rg::Shader::FRAGMENT})
                .textureSampledRead(gNormal, {rg::Shader::FRAGMENT})
                .textureSampledRead(gTangent, {rg::Shader::FRAGMENT})
                .textureSampledRead(gRoughnessMetallicAO, {rg::Shader::FRAGMENT})
                .textureSampledRead(gAlbedo, {rg::Shader::FRAGMENT})
                .textureSampledRead(gObjectIdReceiveShadows, {rg::Shader::FRAGMENT})
                .execute(
                    [this,
                        &scene,
                        surface,
                        gPosition,
                        gNormal,
                        gTangent,
                        gRoughnessMetallicAO,
                        gAlbedo,
                        gObjectIdReceiveShadows,
                        gDepth](rg::RasterContext &ctx) {
                        ctx.bindPipeline(pipeline);

                        ctx.setViewport({}, surface->getDimensions());

                        ctx.bindVertexBuffer(unitQuad, 0, 0, (3 * sizeof(float)) + (2 * sizeof(float)));

                        ctx.bindStorageBuffer("camera", scene.cameraBuffer, 0, 0);
                        ctx.bindStorageBuffer("config", scene.configBuffer, 0, 0);

                        ctx.bindStorageBuffer("pointLights", scene.pointLightBuffer, 0, 0);
                        ctx.bindStorageBuffer("directionalLights", scene.directionalLightBuffer, 0, 0);
                        ctx.bindStorageBuffer("spotLights", scene.spotLightBuffer, 0, 0);

                        ctx.bindTexture("gPosition", {
                                            rg::TextureBinding(gPosition,
                                                               rg::TextureBinding::Range(0, 1, 0, 1),
                                                               rg::TextureBinding::Automatic)
                                        });
                        ctx.bindTexture("gNormal", {
                                            rg::TextureBinding(gNormal,
                                                               rg::TextureBinding::Range(0, 1, 0, 1),
                                                               rg::TextureBinding::Automatic)
                                        });
                        ctx.bindTexture("gTangent", {
                                            rg::TextureBinding(gTangent,
                                                               rg::TextureBinding::Range(0, 1, 0, 1),
                                                               rg::TextureBinding::Automatic)
                                        });
                        ctx.bindTexture("gRoughnessMetallicAO", {
                                            rg::TextureBinding(gRoughnessMetallicAO,
                                                               rg::TextureBinding::Range(0, 1, 0, 1),
                                                               rg::TextureBinding::Automatic)
                                        });
                        ctx.bindTexture("gAlbedo", {
                                            rg::TextureBinding(gAlbedo,
                                                               rg::TextureBinding::Range(0, 1, 0, 1),
                                                               rg::TextureBinding::Automatic)
                                        });
                        ctx.bindTexture("gObjectIdReceiveShadows", {
                                            rg::TextureBinding(gObjectIdReceiveShadows,
                                                               rg::TextureBinding::Range(0, 1, 0, 1),
                                                               rg::TextureBinding::Automatic)
                                        });

                        ctx.drawArray(unitQuadDrawCall);
                    }));
        }

    private:
        static rg::RasterPipeline getPipeline(const rg::Shader &vertexShader, const rg::Shader &fragmentShader) {
            rg::RasterPipeline ret;
            ret.enableDepthTest = false;
            ret.depthTestWrite = false;

            ret.enableBlending = false;

            ret.enableStencilTest = false;
            ret.enableDynamicStencilReference = false;
            ret.stencilTestMask = 0x00;
            ret.stencilMode = rg::RasterPipeline::StencilMode::STENCIL_EQUAL;
            ret.stencilReference = SHADING_MODEL_PBR;

            ret.vertexFormat = rg::RasterPipeline::VertexFormat(vertexShader.inputLayout,
                                                                {0, 0},
                                                                {0, 3 * sizeof(float)});

            ret.colorAttachments = {rg::ColorFormat::RGBA8};
            ret.depthAttachment = rg::ColorFormat::DEPTH24_STENCIL8;
            ret.stencilAttachment = rg::ColorFormat::DEPTH24_STENCIL8;

            ret.shaders = {vertexShader, fragmentShader};

            return ret;
        }

        void createUnitQuad(rg::Heap &heap) {
            const auto mesh = Mesh::normalizedQuad();
            unitQuadDrawCall.offset = 0;
            unitQuadDrawCall.count = mesh.positions.size();
            VertexBuilder builder;
            for (auto i = 0; i < mesh.positions.size(); i++) {
                builder.addVec3(mesh.positions[i]);
                builder.addVec2(mesh.uvs[i]);
            }
            const auto data = builder.build();

            //TODO: Double buffer unit quad
            unitQuad = heap.allocateBuffer(rg::Buffer(data.size(),
                                                      rg::Buffer::CAPABILITY_VERTEX |
                                                      rg::Buffer::CAPABILITY_TRANSFER_DST,
                                                      rg::Buffer::MEMORY_CPU_TO_GPU));
            const auto mapping = heap.map(unitQuad);
            mapping->copyFrom(data, 0, 0, data.size());
        }

        rg::PipelineCache &pipelineCache;
        rg::PipelineCache::Handle pipeline;
        rg::HeapResource<rg::Buffer> unitQuad;
        rg::DrawCall unitQuadDrawCall;
    };
}

#endif //XENGINE_DEFERREDPBRPASS_HPP
