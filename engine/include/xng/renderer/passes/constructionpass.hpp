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

            createPass(graph, scene, gBuffer)
                    .execute([this, &scene, &gBuffer](rg::RasterContext &cmd) {
                        cmd.setViewport({}, gBuffer.resolution);

                        // Draw static meshes
                        cmd.bindPipeline(pipeline);

                        bindBuffers(cmd, scene);

                        for (auto i = 0; i < scene.models.size(); i++) {
                            const auto &model = scene.models.at(i);
                            cmd.setShaderParameter("objectId", rg::ShaderPrimitive(i));
                            cmd.setShaderParameter("transformIndex", rg::ShaderPrimitive(model.transformIndex));
                            cmd.setShaderParameter("materialIndex", rg::ShaderPrimitive(model.materialIndex));
                            cmd.setShaderParameter("receiveShadows", rg::ShaderPrimitive(model.receiveShadows));
                            //TODO: Precompute MVP on cpu (Needs infrastructure in render allocator)
                            for (auto &mesh: model.meshes) {
                                if (mesh.boneCount > 0) {
                                    continue;
                                }
                                cmd.setShaderParameter("boneBaseIndex", rg::ShaderPrimitive(mesh.boneBaseIndex));
                                if (mesh.indexed) {
                                    cmd.drawIndexed(mesh.drawCall, mesh.indexOffset);
                                } else {
                                    cmd.drawArray(mesh.drawCall);
                                }
                            }
                        }

                        // Draw skinned meshes
                        cmd.bindPipeline(pipelineSkinned);

                        bindBuffers(cmd, scene);

                        for (auto i = 0; i < scene.models.size(); i++) {
                            const auto &model = scene.models.at(i);
                            cmd.setShaderParameter("objectId", rg::ShaderPrimitive(i));
                            cmd.setShaderParameter("transformIndex", rg::ShaderPrimitive(model.transformIndex));
                            cmd.setShaderParameter("materialIndex", rg::ShaderPrimitive(model.materialIndex));
                            cmd.setShaderParameter("receiveShadows", rg::ShaderPrimitive(model.receiveShadows));
                            //TODO: Precompute MVP on cpu (Needs infrastructure in render allocator)
                            for (auto &mesh: model.meshes) {
                                if (mesh.boneCount <= 0) {
                                    continue;
                                }
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
        static void bindBuffers(rg::RasterContext &cmd, const RenderScene &scene) {
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

            // Bind storage buffers
            cmd.bindStorageBuffer("camera",
                                  scene.cameraBuffer,
                                  0,
                                  scene.cameraBuffer.getDescription().size);

            cmd.bindStorageBuffer("transforms",
                                  scene.transformBuffer,
                                  0,
                                  scene.transformBuffer.getDescription().size);

            cmd.bindStorageBuffer("materials",
                                  scene.materialBuffer,
                                  0,
                                  scene.materialBuffer.getDescription().size);

            cmd.bindStorageBuffer("bones",
                                  scene.boneBuffer,
                                  0,
                                  scene.boneBuffer.getDescription().size);

            // Bind Textures
            std::vector<rg::TextureBinding> textureBindings;
            for (auto res = RESOLUTION_BEGIN;
                 res <= RESOLUTION_END;
                 res = static_cast<TextureResolution>(res + 1)) {
                textureBindings.emplace_back(rg::Resource(scene.textures.at(res)));
            }
            cmd.bindTexture("textures", textureBindings);
        }

        static rg::RasterPassBuilder &createPass(rg::GraphBuilder &builder,
                                                 const RenderScene &scene,
                                                 const GBuffer &gBuffer) {
            auto &ret = builder.addRasterPass("ConstructionPass")
                    .attachColor(rg::Attachment(gBuffer.position, Vec4f(0)))
                    .attachColor(rg::Attachment(gBuffer.normal, Vec4f(0)))
                    .attachColor(rg::Attachment(gBuffer.tangent, Vec4f(0)))
                    .attachColor(rg::Attachment(gBuffer.roughnessMetallicAO, Vec4f(0)))
                    .attachColor(rg::Attachment(gBuffer.albedo, Vec4f(0)))
                    .attachColor(rg::Attachment(gBuffer.objectIdReceiveShadows, Vec4i(0)))
                    .attachDepth(rg::Attachment(gBuffer.depth, 1.0f));

            // Declare camera buffer access
            ret.storageRead(scene.cameraBuffer, {rg::Shader::VERTEX});

            for (auto &model: scene.models) {
                //TODO: Deduplicate access declarations for meshes / materials referenced in multiple models.

                // Declare transform buffer accesses
                ret.storageRead(scene.transformBuffer,
                                {rg::Shader::VERTEX},
                                sizeof(ShaderTransform::CPU) * model.transformIndex,
                                sizeof(ShaderTransform::CPU));

                // Declare material buffer accesses
                ret.storageRead(scene.materialBuffer,
                                {rg::Shader::FRAGMENT},
                                sizeof(ShaderMaterial::CPU) * model.materialIndex,
                                sizeof(ShaderMaterial::CPU));

                // Declare texture buffer accesses
                for (auto &pair: model.materialTextureIndices) {
                    const auto &texture = scene.textures.at(pair.first);
                    for (auto &arrayLayer: pair.second) {
                        ret.textureSampledRead(texture,
                                               {rg::Shader::FRAGMENT},
                                               rg::TextureBinding::Range(0,
                                                                         texture.getDescription().mipLevels,
                                                                         arrayLayer,
                                                                         1));
                    }
                }

                for (auto &mesh: model.meshes) {
                    // Declare vertex buffer accesses
                    for (auto attr = ATTRIBUTE_BEGIN;
                         attr <= ATTRIBUTE_END;
                         attr = static_cast<VertexAttribute>(attr + 1)) {
                        ret.vertexRead(scene.vertexBuffers.at(attr),
                                       getVertexAttributeSize(attr) * mesh.indexOffset,
                                       getVertexAttributeSize(attr) * mesh.indexCount);
                    }

                    if (mesh.indexed) {
                        // Declare index buffer access
                        ret.indexRead(scene.indexBuffer, mesh.indexOffset, sizeof(uint32_t) * mesh.indexCount);
                    }

                    if (mesh.boneCount > 0) {
                        // Declare bone buffer accesses
                        ret.storageRead(scene.boneBuffer,
                                        {rg::Shader::VERTEX},
                                        sizeof(ShaderTransform::CPU) * mesh.boneBaseIndex,
                                        sizeof(ShaderTransform::CPU) * mesh.boneCount);
                    }
                }
            }
            return ret;
        }

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
