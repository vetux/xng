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
    class ConstructionPass final : public RenderPass {
    public:
        struct GBuffer {
            static std::vector<rg::ColorFormat> getColorFormats() {
                return {
                    rg::ColorFormat::RGBA,
                };
            }

            explicit GBuffer(rg::GraphBuilder &graph, Vec2i _resolution)
                : resolution(std::move(_resolution)) {
                auto desc = rg::Texture();
                desc.size = resolution;

                desc.format = rg::ColorFormat::RGB32F;
                position = graph.allocateTexture(desc);
                normal = graph.allocateTexture(desc);
                tangent = graph.allocateTexture(desc);

                desc.format = rg::ColorFormat::RGB;
                roughnessMetallicAO = graph.allocateTexture(desc);
                albedo = graph.allocateTexture(desc);

                desc.format = rg::ColorFormat::RGB32UI;
                objectIdReceiveShadows = graph.allocateTexture(desc);

                desc.format = rg::ColorFormat::DEPTH;
                depth = graph.allocateTexture(desc);
            }

            void subscribe(RenderPassRegistry &registry) const {
                registry.set(RenderPassRegistry::G_BUFFER_POSITION, position);
                registry.set(RenderPassRegistry::G_BUFFER_NORMAL, normal);
                registry.set(RenderPassRegistry::G_BUFFER_TANGENT, tangent);
                registry.set(RenderPassRegistry::G_BUFFER_ROUGHNESS_METALLIC_AO, roughnessMetallicAO);
                registry.set(RenderPassRegistry::G_BUFFER_ALBEDO, albedo);
                registry.set(RenderPassRegistry::G_BUFFER_OBJECT_ID_RECEIVE_SHADOWS, objectIdReceiveShadows);
                registry.set(RenderPassRegistry::G_BUFFER_DEPTH, depth);
            }

            Vec2i resolution;

            rg::Resource<rg::Texture> position;
            rg::Resource<rg::Texture> normal;
            rg::Resource<rg::Texture> tangent;
            rg::Resource<rg::Texture> roughnessMetallicAO;
            rg::Resource<rg::Texture> albedo;
            rg::Resource<rg::Texture> objectIdReceiveShadows;
            rg::Resource<rg::Texture> depth;
        };

        static rg::Shader compileFragmentShader();

        static rg::Shader compileVertexShader();

        static rg::Shader compileSkinnedVertexShader();

        explicit ConstructionPass(rg::PipelineCache &pipelineCache)
            : ConstructionPass(pipelineCache,
                               compileFragmentShader(),
                               compileVertexShader(),
                               compileSkinnedVertexShader()) {
        }

        ConstructionPass(rg::PipelineCache &pipelineCache,
                         const rg::Shader &fragmentShader,
                         const rg::Shader &vertexShader,
                         const rg::Shader &skinnedVertexShader)
            : pipelineCache(pipelineCache) {
            pipeline = pipelineCache.create(getPipeline(fragmentShader, vertexShader));
            pipelineSkinned = pipelineCache.create(getPipelineSkinned(fragmentShader, skinnedVertexShader));
        }

        ~ConstructionPass() override {
            pipelineCache.destroy(pipeline);
            pipelineCache.destroy(pipelineSkinned);
        }

        void record(rg::GraphBuilder &graph,
                    rg::Surface &surface,
                    const RenderScene &scene,
                    RenderPassRegistry &registry) override {
            const GBuffer gBuffer(graph, surface.getDimensions());

            gBuffer.subscribe(registry);

            graph.addRasterPass("ConstructionPass")
                    .attachColor(rg::Attachment(gBuffer.position, Vec4f(0)))
                    .attachColor(rg::Attachment(gBuffer.normal, Vec4f(0)))
                    .attachColor(rg::Attachment(gBuffer.tangent, Vec4f(0)))
                    .attachColor(rg::Attachment(gBuffer.roughnessMetallicAO, Vec4f(0)))
                    .attachColor(rg::Attachment(gBuffer.albedo, Vec4f(0)))
                    .attachColor(rg::Attachment(gBuffer.objectIdReceiveShadows, Vec4i(0)))
                    .attachDepth(rg::Attachment(gBuffer.depth, 1.0f))
                    .execute([this, &scene, &gBuffer](rg::RasterContext &cmd) {
                        cmd.setViewport({}, gBuffer.resolution);

                        cmd.bindStorageBuffer("camera",
                                              scene.cameraBuffer,
                                              0,
                                              scene.cameraBuffer.getDescription().size);

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
                        cmd.bindIndexBuffer(scene.indexBuffer);

                        // Bind BufferStreamer buffers indexed via shader parameters
                        cmd.bindStorageBuffer("transforms",
                                              scene.transformBuffer,
                                              0,
                                              scene.transformBuffer.getDescription().size);

                        cmd.bindPipeline(pipeline);

                        for (auto i = 0; i < scene.models.size(); i++) {
                            const auto &model = scene.models.at(i);
                            cmd.setShaderParameter("objectId", rg::ShaderPrimitive(i));
                            cmd.setShaderParameter("transformIndex", rg::ShaderPrimitive(model.transformIndex));
                            cmd.setShaderParameter("materialIndex", rg::ShaderPrimitive(model.materialIndex));
                            cmd.setShaderParameter("receiveShadows", rg::ShaderPrimitive(model.receiveShadows));
                            //TODO: Precompute MVP on cpu (Needs infrastructure in render allocator)
                            for (auto &mesh: model.meshes) {
                                cmd.setShaderParameter("boneBaseIndex", rg::ShaderPrimitive(mesh.boneBaseIndex));
                                if (mesh.indexed) {
                                    cmd.drawIndexed(mesh.drawCall, mesh.indexOffset);
                                } else {
                                    cmd.drawArray(mesh.drawCall);
                                }
                            }
                        }
                    });
        }

    private:
        static rg::RasterPipeline getPipeline(const rg::Shader &vertexShader, const rg::Shader &fragmentShader) {
            rg::RasterPipeline ret;
            ret.shaders = {vertexShader, fragmentShader};

            ret.enableDepthTest = true;
            ret.depthTestWrite = true;

            ret.colorAttachments = GBuffer::getColorFormats();
            ret.depthAttachment = rg::ColorFormat::DEPTH;

            ret.vertexFormat = rg::RasterPipeline::VertexFormat(vertexShader.inputLayout,
                                                                {
                                                                    POSITION,
                                                                    NORMAL,
                                                                    TANGENT,
                                                                    BITANGENT,
                                                                    UV,
                                                                    BONE_INDEX,
                                                                    BONE_WEIGHT
                                                                },
                                                                std::vector<size_t>(7, 0));
            return ret;
        }

        static rg::RasterPipeline getPipelineSkinned(const rg::Shader &vertexShader, const rg::Shader &fragmentShader) {
            rg::RasterPipeline ret;
            ret.shaders = {vertexShader, fragmentShader};

            ret.enableDepthTest = true;
            ret.depthTestWrite = true;

            ret.colorAttachments = GBuffer::getColorFormats();
            ret.depthAttachment = rg::ColorFormat::DEPTH;

            ret.vertexFormat = rg::RasterPipeline::VertexFormat(vertexShader.inputLayout,
                                                                {
                                                                    POSITION,
                                                                    NORMAL,
                                                                    TANGENT,
                                                                    BITANGENT,
                                                                    UV,
                                                                },
                                                                std::vector<size_t>(5, 0));
            return ret;
        }

        rg::PipelineCache &pipelineCache;
        rg::PipelineCache::Handle pipeline;
        rg::PipelineCache::Handle pipelineSkinned;
    };
}

#endif //XENGINE_CONSTRUCTIONPASS_HPP
