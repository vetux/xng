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

#include "xng/renderer/pipeline/indirect/renderpipelineindirect.hpp"

#include "xng/shaderscript/indirectbuffers.hpp"

namespace xng {
    rg::PipelineCache::Handle RenderPipelineIndirect::compilePrePassPipeline(rg::Runtime &runtime, rg::Shader shader) {
        rg::ComputePipeline pip;
        pip.shader = std::move(shader);
        return runtime.getPipelineCache().create(pip);
    }

    RenderPipelineIndirect::~RenderPipelineIndirect() = default;

    std::shared_ptr<RenderPipelineTransform> RenderPipelineIndirect::createTransform() {
        return std::make_shared<RenderPipelineTransformIndirect>(transformStreamer, transformStreamer.create());
    }

    std::shared_ptr<RenderPipelineMaterial> RenderPipelineIndirect::createMaterial() {
        return std::make_shared<RenderPipelineMaterialIndirect>(materialStreamer,
                                                                materialStreamer.create(),
                                                                materialLayout);
    }

    RenderPipeline::DrawID RenderPipelineIndirect::addDrawCall(std::shared_ptr<RenderPipelineTransform> transform,
                                                               std::shared_ptr<RenderPipelineMaterial> material,
                                                               const std::vector<RenderObjectHandle<RenderMesh> > &mesh,
                                                               const int sortPriority) {
        const auto id = allocateID();
        DrawCall drawCall(transform, material, mesh, sortPriority);

        if (drawLists.find(sortPriority) == drawLists.end()) {
            drawLists.emplace(sortPriority, std::move(DrawList(heap, chunkStreamer)));
        }

        bool pending = true;
        if (drawCall.isUploadComplete()) {
            drawCall.loadData();
            pending = false;
        }

        drawCalls.emplace(id, drawCall);
        if (pending) {
            pendingDrawCalls.insert(id);
        } else {
            drawLists.at(drawCall.sortPriority).updateDrawCallBuffer = true;
        }

        return id;
    }

    RenderPipeline::DrawID RenderPipelineIndirect::addDrawCall(std::shared_ptr<RenderPipelineTransform> transform,
                                                               const std::vector<RenderObjectHandle<RenderMesh> > &
                                                               meshes,
                                                               int sortPriority) {
        //TODO: Material-less draw calls.
        throw std::runtime_error("Material-Less draw calls not supported yet.");
    }

    void RenderPipelineIndirect::removeDrawCall(const DrawID id) {
        const auto &drawCall = drawCalls.at(id);
        if (pendingDrawCalls.find(id) == pendingDrawCalls.end()) {
            drawLists.at(drawCall.sortPriority).drawCalls.erase(id);
            drawLists.at(drawCall.sortPriority).updateDrawCallBuffer = true;
        }
        pendingDrawCalls.erase(id);
        drawCalls.erase(id);
    }

    void RenderPipelineIndirect::setCamera(const Vec3f &position, const Mat4f &view, const Mat4f &projection) {
        if (cameraResident) {
            cameraBuffer.release(cameraBufferHandle);
        }
        RenderPipelineCompilerIndirect::ShaderCamera::CPU camera;
        camera.projection = projection;
        camera.view = view;
        camera.viewPosition = Vec4f(position.x, position.y, position.z, 1.0f);
        cameraBufferHandle = cameraBuffer.upload(reinterpret_cast<const uint8_t *>(&camera),
                                                 sizeof(RenderPipelineCompilerIndirect::ShaderCamera::CPU),
                                                 0);
        cameraBuffer.flush(cameraBufferHandle);
        cameraResident = true;
    }

    void RenderPipelineIndirect::setEnableDistanceSort(bool enable) {
        //TODO: Distance sort
    }

    void RenderPipelineIndirect::setEnableDrawCulling(bool enable) {
        // TODO: Volume Culling (Meshlets)
    }

    void RenderPipelineIndirect::commit(RenderQueue &queue) {
        // Merge pending draw calls that finished uploading.
        std::unordered_set<DrawID> mergedDrawCalls;
        for (auto &id: pendingDrawCalls) {
            auto &drawCall = drawCalls.at(id);
            if (drawCall.isUploadComplete()) {
                auto it = drawLists.find(drawCall.sortPriority);
                if (it == drawLists.end()) {
                    drawLists.emplace(drawCall.sortPriority, DrawList(heap, chunkStreamer));
                    it = drawLists.find(drawCall.sortPriority);
                }
                it->second.drawCalls.insert(id);
                it->second.updateDrawCallBuffer = true;
                drawCall.loadData();
                mergedDrawCalls.insert(id);
            }
        }
        for (auto &id: mergedDrawCalls) {
            pendingDrawCalls.erase(id);
        }

        // Commit draw lists
        cameraBuffer.commit(queue);
        transformStreamer.commit(queue);
        materialStreamer.commit(queue);
        for (auto &pair: drawLists) {
            pair.second.commit(queue, heap, drawCalls);
        }
    }

    void RenderPipelineIndirect::prepare(RenderQueue &queue) {
        // Record Prepasses
        for (auto &pair: drawLists) {
            recordPrePass(queue, pair.second);
        }
    }

    void RenderPipelineIndirect::execute(rg::GraphBuilder &graph,
                                         const std::string &passName,
                                         const RenderPipelineShader &shader,
                                         const Recti &viewport,
                                         const std::vector<Attachment> &colorAttachments,
                                         std::optional<rg::Attachment> depthStencilAttachment,
                                         std::optional<rg::Attachment> depthAttachment,
                                         std::optional<rg::Attachment> stencilAttachment,
                                         const std::unordered_map<std::string, rg::ShaderPrimitive> &parameters,
                                         const std::unordered_map<std::string, BufferBinding> &storageBuffers,
                                         const std::unordered_map<std::string, std::vector<rg::TextureBinding> > &
                                         textureArrays) const {
        // Bind and execute
        rg::GraphicsPassBuilder builder(passName);

        std::vector<rg::Attachment> cAttachments;

        // Declare Accesses
        // Attachments
        for (auto &attachment: colorAttachments) {
            if (std::holds_alternative<rg::Attachment>(attachment)) {
                const auto &att = std::get<rg::Attachment>(attachment);
                if (std::holds_alternative<rg::Resource<rg::Texture> >(att.target)) {
                    builder.textureAttachmentColor(std::get<rg::Resource<rg::Texture> >(att.target));
                } else {
                    builder.surfaceAttachmentColor(std::get<std::shared_ptr<rg::Surface> >(att.target));
                }
                cAttachments.emplace_back(att);
            } else {
                throw std::runtime_error("RenderTexture attachment support not implemented yet.");
            }
        }

        if (depthStencilAttachment.has_value()) {
            if (std::holds_alternative<rg::Resource<rg::Texture> >(depthStencilAttachment.value().target)) {
                builder.textureAttachmentDepthStencil(
                    std::get<rg::Resource<rg::Texture> >(depthStencilAttachment.value().target));
            } else {
                builder.surfaceAttachmentDepthStencil(
                    std::get<std::shared_ptr<rg::Surface> >(depthStencilAttachment.value().target));
            }
        } else {
            if (depthAttachment.has_value()) {
                if (std::holds_alternative<rg::Resource<rg::Texture> >(depthAttachment.value().target)) {
                    builder.textureAttachmentDepthStencil(
                        std::get<rg::Resource<rg::Texture> >(depthAttachment.value().target));
                } else {
                    builder.surfaceAttachmentDepthStencil(
                        std::get<std::shared_ptr<rg::Surface> >(depthAttachment.value().target));
                }
            }
            if (stencilAttachment.has_value()) {
                if (std::holds_alternative<rg::Resource<rg::Texture> >(stencilAttachment.value().target)) {
                    builder.textureAttachmentDepthStencil(
                        std::get<rg::Resource<rg::Texture> >(stencilAttachment.value().target));
                } else {
                    builder.surfaceAttachmentDepthStencil(
                        std::get<std::shared_ptr<rg::Surface> >(stencilAttachment.value().target));
                }
            }
        }

        // Buffers
        //TODO: Shader access stage tracking
        for (const auto &pair: drawLists) {
            for (const auto &range: pair.second.transformAccesses) {
                builder.storageRead(transformStreamer.getBuffer(),
                                    {rg::Shader::VERTEX, rg::Shader::FRAGMENT},
                                    range.offset,
                                    range.size);
            }
            for (const auto &range: pair.second.materialAccesses) {
                builder.storageRead(materialStreamer.getBuffer(),
                                    {rg::Shader::VERTEX, rg::Shader::FRAGMENT},
                                    range.offset,
                                    range.size);
            }
            const auto vertexBuffers = meshStreamer.getVertexBuffers();
            for (const auto &vbAccess: pair.second.vertexBufferAccesses) {
                const auto &buf = vertexBuffers.at(vbAccess.first);
                for (const auto &range: vbAccess.second) {
                    builder.vertexRead(buf, range.offset, range.size);
                }
            }
            for (const auto &range: pair.second.indexBufferAccesses) {
                builder.indexRead(meshStreamer.getIndexBuffer(), range.offset, range.size);
            }

            builder.storageRead(pair.second.drawMeshBuffer, {rg::Shader::VERTEX, rg::Shader::FRAGMENT});
        }

        // Virtual Texture Atlas
        const auto &atlasTex = virtualTextureStreamer.getAtlasTexture();
        builder.textureSampledRead(atlasTex,
                                   {rg::Shader::VERTEX, rg::Shader::FRAGMENT},
                                   rg::TextureBinding::Range(0,
                                                             1,
                                                             0,
                                                             atlasTex.getDescription().arrayLayers));

        builder.storageRead(virtualTextureStreamer.getTileMapBuffer(),
                            {rg::Shader::VERTEX, rg::Shader::FRAGMENT});
        builder.storageRead(virtualTextureStreamer.getTileMapOffsetsBuffer(),
                            {rg::Shader::VERTEX, rg::Shader::FRAGMENT});
        builder.storageRead(virtualTextureStreamer.getResidencyMapBuffer(),
                            {rg::Shader::VERTEX, rg::Shader::FRAGMENT});
        builder.storageRead(virtualTextureStreamer.getResidencyMapOffsetsBuffer(),
                            {rg::Shader::VERTEX, rg::Shader::FRAGMENT});
        builder.storageWrite(virtualTextureStreamer.getReadbackBuffer(),
                             {rg::Shader::VERTEX, rg::Shader::FRAGMENT});

        builder.storageRead(cameraBuffer.getBuffer(), {rg::Shader::VERTEX, rg::Shader::FRAGMENT});

        // User Buffers
        for (const auto &pair: storageBuffers) {
            builder.storageRead(pair.second.buffer,
                                {rg::Shader::VERTEX, rg::Shader::FRAGMENT},
                                pair.second.offset,
                                pair.second.size);
        }

        // User Textures
        for (const auto &pair: textureArrays) {
            for (auto &tex: pair.second) {
                builder.textureSampledRead(tex.texture, {
                                               rg::Shader::VERTEX,
                                               rg::Shader::FRAGMENT
                                           },
                                           tex.range,
                                           tex.aspect);
            }
        }

        graph.addPass(builder.execute([this,
                &shader,
                viewport,
                depthStencilAttachment,
                depthAttachment,
                stencilAttachment,
                parameters,
                storageBuffers,
                textureArrays,
                cAttachments](rg::RasterContext &cmd,
                              rg::TransferContext &,
                              rg::ComputeContext &) {
                if (depthStencilAttachment.has_value()) {
                    cmd.beginRenderPass(cAttachments, depthStencilAttachment.value());
                } else {
                    cmd.beginRenderPass(cAttachments, depthAttachment, stencilAttachment);
                }

                // Bind pipeline
                cmd.bindPipeline(shader.getPipeline());

                cmd.setViewport(viewport.position, viewport.dimensions.convert<unsigned int>());

                // Bind Vertex Buffers
                const auto vertexBuffers = meshStreamer.getVertexBuffers();
                for (auto attr = ATTRIBUTE_BEGIN;
                     attr <= ATTRIBUTE_END;
                     attr = static_cast<VertexAttribute>(attr + 1)) {
                    cmd.bindVertexBuffer(vertexBuffers.at(attr),
                                         attr,
                                         0,
                                         getVertexAttributeSize(attr));
                }

                // Bind Index Buffer
                cmd.bindIndexBuffer(meshStreamer.getIndexBuffer(), rg::INDEX_UNSIGNED_INT);

                // Bind storage buffers
                cmd.bindStorageBuffer(RenderPipelineCompilerIndirect::transformBufferName,
                                      transformStreamer.getBuffer(),
                                      0,
                                      0);

                cmd.bindStorageBuffer(RenderPipelineCompilerIndirect::materialBufferName,
                                      materialStreamer.getBuffer(),
                                      0,
                                      0);

                // Bind Virtual Texture setup
                cmd.bindTexture(RenderPipelineCompilerIndirect::virtualAtlasTextureName,
                                {rg::TextureBinding(virtualTextureStreamer.getAtlasTexture())});
                cmd.bindStorageBuffer(RenderPipelineCompilerIndirect::virtualTileMapName,
                                      virtualTextureStreamer.getTileMapBuffer(),
                                      0,
                                      0);
                cmd.bindStorageBuffer(RenderPipelineCompilerIndirect::virtualTileMapOffsetsName,
                                      virtualTextureStreamer.getTileMapOffsetsBuffer(),
                                      0,
                                      0);
                cmd.bindStorageBuffer(RenderPipelineCompilerIndirect::virtualResidencyMapName,
                                      virtualTextureStreamer.getResidencyMapBuffer(),
                                      0,
                                      0);
                cmd.bindStorageBuffer(RenderPipelineCompilerIndirect::virtualResidencyMapOffsetsName,
                                      virtualTextureStreamer.getResidencyMapOffsetsBuffer(),
                                      0,
                                      0);
                cmd.bindStorageBuffer(RenderPipelineCompilerIndirect::virtualReadbackBufferName,
                                      virtualTextureStreamer.getReadbackBuffer(),
                                      0,
                                      0);

                cmd.setShaderParameter(RenderPipelineCompilerIndirect::virtualAtlasSizeName,
                                       rg::ShaderPrimitive(
                                           virtualTextureStreamer.getAtlasTexture().getDescription().size.x));
                cmd.setShaderParameter(RenderPipelineCompilerIndirect::virtualTileSizeName,
                                       rg::ShaderPrimitive(virtualTextureStreamer.getTileSize()));
                cmd.setShaderParameter(RenderPipelineCompilerIndirect::virtualTileBorderName,
                                       rg::ShaderPrimitive(virtualTextureStreamer.getTileBorder()));
                cmd.setShaderParameter(RenderPipelineCompilerIndirect::virtualMaxAnisotropyName,
                                       rg::ShaderPrimitive(virtualTextureStreamer.getMaxAnisotropy()));

                // Bind user buffers
                for (const auto &pair: storageBuffers) {
                    cmd.bindStorageBuffer(pair.first,
                                          pair.second.buffer,
                                          pair.second.offset,
                                          pair.second.size);
                }

                // Bind User Textures
                for (const auto &pair: textureArrays) {
                    cmd.bindTexture(pair.first, pair.second);
                }

                // Set User Parameters
                for (const auto &pair: parameters) {
                    cmd.setShaderParameter(pair.first, pair.second);
                }

                for (const auto &pair: drawLists) {
                    if (pair.second.drawCalls.empty()) {
                        continue;
                    }
                    cmd.bindStorageBuffer(RenderPipelineCompilerIndirect::drawMeshBufferName,
                                          pair.second.drawMeshBuffer,
                                          0,
                                          0);

                    cmd.drawIndexedMultiIndirectCount(pair.second.indirectBuffer,
                                                      pair.second.indirectCountBuffer,
                                                      0,
                                                      0,
                                                      pair.second.getDrawCallCount(drawCalls),
                                                      sizeof(ShaderScript::ShaderDrawIndirectIndexed::CPU));
                }

                cmd.endRenderPass();
            }));
    }

    RenderPipelineIndirect::DrawList::DrawList(rg::Heap &resourceHeap,
                                               ChunkStreamer &chunkStreamer)
        : drawCallBuffer(resourceHeap, chunkStreamer, rg::Buffer::CAPABILITY_STORAGE) {
        const rg::Buffer desc(sizeof(int),
                              rg::Buffer::CAPABILITY_STORAGE | rg::Buffer::CAPABILITY_INDIRECT,
                              rg::Buffer::MEMORY_GPU_ONLY);
        indirectCountBuffer = resourceHeap.allocateBuffer(desc);
    }

    void RenderPipelineIndirect::DrawList::commit(RenderQueue &queue,
                                                  rg::Heap &resourceHeap,
                                                  const std::unordered_map<DrawID, DrawCall> &callMap) {
        if (!updateDrawCallBuffer) {
            return;
        }

        updateDrawCallBuffer = false;

        if (residentDrawCallBuffer) {
            drawCallBuffer.release(drawCallBufferHandle);
        }

        std::unordered_set<BufferAccessRange, BufferAccessRangeHasher> transformAccess;
        std::unordered_set<BufferAccessRange, BufferAccessRangeHasher> materialAccess;
        std::unordered_map<VertexAttribute, std::unordered_set<BufferAccessRange, BufferAccessRangeHasher> >
                vertexBufferAccess;
        std::unordered_set<BufferAccessRange, BufferAccessRangeHasher> indexBufferAccess;

        // Fetch draw call data and accesses
        std::vector<ShaderDrawCall::CPU> drawCallData;
        for (auto &id: drawCalls) {
            auto &drawCall = callMap.at(id);
            drawCallData.insert(drawCallData.end(), drawCall.drawCallData.begin(), drawCall.drawCallData.end());
            transformAccess.insert(drawCall.transformAccessRanges.begin(), drawCall.transformAccessRanges.end());
            materialAccess.insert(drawCall.materialAccessRanges.begin(), drawCall.materialAccessRanges.end());
            for (auto &pair: drawCall.vertexBufferAccessRanges) {
                vertexBufferAccess[pair.first].insert(pair.second.begin(), pair.second.end());
            }
            indexBufferAccess.insert(drawCall.indexBufferAccessRanges.begin(), drawCall.indexBufferAccessRanges.end());
        }

        // Upload draw call buffer
        drawCallBufferHandle = drawCallBuffer.upload(reinterpret_cast<const uint8_t *>(drawCallData.data()),
                                                     drawCallData.size() * sizeof(ShaderDrawCall::CPU),
                                                     0);
        drawCallBuffer.flush(drawCallBufferHandle);
        residentDrawCallBuffer = true;

        drawCallBuffer.commit(queue);

        // Update indirect / drawMesh buffers
        const auto indirectBufferSize = drawCallData.size() * sizeof(ShaderScript::ShaderDrawIndirectIndexed::CPU);
        if (indirectBuffer.getDescription().size != indirectBufferSize && indirectBufferSize > 0) {
            const rg::Buffer desc(indirectBufferSize,
                                  rg::Buffer::CAPABILITY_STORAGE | rg::Buffer::CAPABILITY_INDIRECT,
                                  rg::Buffer::MEMORY_GPU_ONLY);
            indirectBuffer = resourceHeap.allocateBuffer(desc);
        }

        const auto drawMeshBufferSize = drawCallData.size()
                                        * sizeof(RenderPipelineCompilerIndirect::ShaderDrawMesh::CPU);

        if (drawMeshBuffer.getDescription().size != drawMeshBufferSize && drawMeshBufferSize > 0) {
            const rg::Buffer desc(drawMeshBufferSize,
                                  rg::Buffer::CAPABILITY_STORAGE,
                                  rg::Buffer::MEMORY_GPU_ONLY);
            drawMeshBuffer = resourceHeap.allocateBuffer(desc);
        }

        // Update accesses
        transformAccesses.clear();
        transformAccesses.reserve(transformAccess.size());
        for (auto &range: transformAccess) {
            transformAccesses.emplace_back(range);
        }

        materialAccesses.clear();
        materialAccesses.reserve(materialAccess.size());
        for (auto &range: materialAccess) {
            materialAccesses.emplace_back(range);
        }

        vertexBufferAccesses.clear();
        for (auto &pair: vertexBufferAccess) {
            vertexBufferAccesses[pair.first].reserve(pair.second.size());
            for (auto &range: pair.second) {
                vertexBufferAccesses[pair.first].emplace_back(range);
            }
        }

        indexBufferAccesses.clear();
        indexBufferAccesses.reserve(indexBufferAccess.size());
        for (auto &range: indexBufferAccess) {
            indexBufferAccesses.emplace_back(range);
        }
    }

    void RenderPipelineIndirect::recordPrePass(RenderQueue &queue, const DrawList &drawList) const {
        if (drawList.drawCalls.empty()) {
            return;
        }

        rg::ComputePassBuilder builder("RenderPipelineIndirect/PrePass");

        builder.storageWrite(drawList.indirectBuffer);
        builder.storageWrite(drawList.indirectCountBuffer);
        builder.storageWrite(drawList.drawMeshBuffer);

        builder.storageRead(drawList.drawCallBuffer.getBuffer());

        builder.storageRead(cameraBuffer.getBuffer());

        queue.addFrame(builder.execute([this, &drawList](rg::ComputeContext &ctx) {
            ctx.bindPipeline(prePassPipeline);

            ctx.bindStorageBuffer("camera", cameraBuffer.getBuffer(), 0, 0);
            ctx.bindStorageBuffer("transforms", transformStreamer.getBuffer(), 0, 0);
            ctx.bindStorageBuffer("drawCallBuffer", drawList.drawCallBuffer.getBuffer(), 0, 0);

            ctx.bindStorageBuffer("drawMeshBuffer", drawList.drawMeshBuffer, 0, 0);

            ctx.bindStorageBuffer("commandBuffer", drawList.indirectBuffer, 0, 0);
            ctx.bindStorageBuffer("commandCountBuffer", drawList.indirectCountBuffer, 0, 0);

            ctx.setShaderParameter("batchSize",
                                   rg::ShaderPrimitive(static_cast<int>(drawList.getDrawCallCount(drawCalls))));

            ctx.dispatch(Vec3u((drawList.getDrawCallCount(drawCalls) + (prePassLocalSize - 1)) / prePassLocalSize,
                               1,
                               1));
        }));
    }
}
