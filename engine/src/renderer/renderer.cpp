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

#include "xng/renderer/renderer.hpp"

#include <cstring>

namespace xng {
    static rg::PipelineCache::Handle createPipeline(rg::PipelineCache &cache, const rg::Shader &shader) {
        rg::ComputePipeline pip;
        pip.shader = shader;
        return cache.create(pip);
    }

    Renderer::Renderer(rg::Runtime &runtime,
                       const size_t streamingBudget,
                       const rg::Shader &skinningShader,
                       const rg::Shader &scenePrepassShader)
        : runtime(runtime),
          allocator(runtime, streamingBudget),
          skinningPipeline(createPipeline(runtime.getPipelineCache(), skinningShader)),
          scenePrepassPipeline(createPipeline(runtime.getPipelineCache(), scenePrepassShader)) {
        cameraBuffer = runtime.getResourceHeap().allocateBuffer(rg::Buffer(sizeof(ShaderCamera::CPU),
                                                                           rg::Buffer::CAPABILITY_STORAGE
                                                                           | rg::Buffer::CAPABILITY_TRANSFER_DST,
                                                                           rg::Buffer::MEMORY_CPU_TO_GPU));
        configBuffer = runtime.getResourceHeap().allocateBuffer(rg::Buffer(sizeof(ShaderConfiguration::CPU),
                                                                           rg::Buffer::CAPABILITY_STORAGE
                                                                           | rg::Buffer::CAPABILITY_TRANSFER_DST,
                                                                           rg::Buffer::MEMORY_CPU_TO_GPU));
    }

    RenderAllocator &Renderer::getAllocator() {
        return allocator;
    }

    void Renderer::setPasses(std::vector<std::shared_ptr<RenderPass> > _passes) {
        passes = std::move(_passes);
    }

    void Renderer::draw(const std::shared_ptr<rg::Surface> &surface, const RenderDrawList &drawList) {
        rg::GraphBuilder graph;

        // Commit streams
        const auto buffers = allocator.commit(graph);

        // Record compute skinning
        graph.addPass(recordSkinningPass(drawList, buffers));

        // Record prepass
        RenderScene scene;
        graph.addPass(recordScenePrePass(graph, runtime.getResourceHeap(), drawList, buffers, scene));

        // Record passes
        RenderPassRegistry registry;
        for (const auto &pass: passes) {
            pass->record(graph, surface, registry, scene);
        }

        static constexpr size_t timeOut = 10'000'000'000ULL;
        auto sem = runtime.execute(graph.build());
        if (!sem->wait(timeOut)) {
            throw std::runtime_error("Renderer timed out");
        }
    }

    RendererStatistics Renderer::getStatistics() const {
        RendererStatistics stats;
        stats.streamingTiles = allocator.getTextureStreamer().getStreamingTiles();
        return stats;
    }

    rg::ComputePass Renderer::recordSkinningPass(const RenderDrawList &drawList,
                                                 const RenderAllocator::Buffers &buffers) const {
        std::unordered_set<RenderObjectHandle<RenderMesh> > meshes;
        for (auto &model: drawList.models) {
            for (auto &mesh: model->getMeshes()) {
                if (mesh->getSkeleton()) {
                    meshes.insert(mesh);
                }
            }
        }

        rg::ComputePassBuilder builder("SkinningPass");

        if (meshes.empty()) {
            return builder.execute([](rg::ComputeContext &ctx) {
            });
        }

        // Declare Accesses
        std::unordered_set<RenderObjectHandle<RenderSkeleton> > processedSkeletons;
        for (auto &mesh: meshes) {
            auto skeleton = mesh->getSkeleton();

            assert(skeleton);

            if (processedSkeletons.find(skeleton) == processedSkeletons.end()) {
                builder.storageRead(buffers.boneBuffer,
                                    skeleton->getBaseBone() * sizeof(ShaderTransform::CPU),
                                    skeleton->getOffsets().size() * sizeof(ShaderTransform::CPU));
                processedSkeletons.insert(skeleton);
            }

            const auto bonePosSize = getVertexAttributeSize(POSITION);
            constexpr auto boneIndexSize = sizeof(int) * 4;
            constexpr auto boneWeightSize = sizeof(float) * 4;

            const auto &alloc = mesh->getAllocation();

            builder.storageRead(buffers.skinnedBoneIndicesBuffer,
                                alloc.skinBaseVertex * boneIndexSize,
                                alloc.vertexCount * boneIndexSize);
            builder.storageRead(buffers.skinnedBoneWeightsBuffer,
                                alloc.skinBaseVertex * boneWeightSize,
                                alloc.vertexCount * boneWeightSize);

            builder.storageRead(buffers.skinnedBindPosBuffer,
                                alloc.skinBaseVertex * bonePosSize,
                                alloc.vertexCount * bonePosSize);
            builder.storageWrite(buffers.vertexBuffers.at(POSITION),
                                 alloc.baseVertex * bonePosSize,
                                 alloc.vertexCount * bonePosSize);
        }

        // Execute
        return builder.execute([this, buffers, meshes = std::move(meshes)](rg::ComputeContext &ctx) {
            ctx.bindPipeline(skinningPipeline);
            ctx.bindStorageBuffer("bones", buffers.boneBuffer, 0, 0);
            ctx.bindStorageBuffer("positions", buffers.skinnedBindPosBuffer, 0, 0);
            ctx.bindStorageBuffer("boneIds", buffers.skinnedBoneIndicesBuffer, 0, 0);
            ctx.bindStorageBuffer("boneWeights", buffers.skinnedBoneWeightsBuffer, 0, 0);
            ctx.bindStorageBuffer("skinnedPositions", buffers.vertexBuffers.at(POSITION), 0, 0);
            for (auto &mesh: meshes) {
                const auto &alloc = mesh->getAllocation();
                const auto baseBone = mesh->getSkeleton()->getBaseBone();
                ctx.setShaderParameter("vertexCount",
                                       rg::ShaderPrimitive(static_cast<unsigned int>(alloc.vertexCount)));
                ctx.setShaderParameter("baseVertex",
                                       rg::ShaderPrimitive(static_cast<unsigned int>(alloc.baseVertex)));
                ctx.setShaderParameter("skinBaseVertex",
                                       rg::ShaderPrimitive(static_cast<unsigned int>(alloc.skinBaseVertex)));
                ctx.setShaderParameter("baseBone",
                                       rg::ShaderPrimitive(static_cast<unsigned int>(baseBone)));
                ctx.dispatch(Vec3u((alloc.vertexCount + (skinningLocalSize - 1)) / skinningLocalSize, 1, 1));
            }
        });
    }

    struct DrawBatch {
        ShadingModel shadingModel;
        std::vector<int> meshIndices;

        std::vector<RenderScene::BufferAccessRange> drawBufferAccesses;
        std::vector<RenderScene::BufferAccessRange> transformBufferAccesses;
        std::vector<RenderScene::BufferAccessRange> materialBufferAccesses;

        std::unordered_map<VertexAttribute, std::vector<RenderScene::BufferAccessRange> > vertexBufferAccesses;
        std::vector<RenderScene::BufferAccessRange> indexBufferAccesses;

        std::vector<size_t> textureAccesses;
    };

    rg::ComputePass Renderer::recordScenePrePass(rg::GraphBuilder &graphBuilder,
                                                 rg::Heap &heap,
                                                 const RenderDrawList &drawList,
                                                 const RenderAllocator::Buffers &buffers,
                                                 RenderScene &scene) {
        // Gather / Sort forward and deferred batches
        std::map<ShadingModel, DrawBatch> deferredBatches;
        std::vector<DrawBatch> forwardBatches;
        DrawBatch currentForwardBatch;

        size_t totalDrawCount = 0;
        for (auto &model: drawList.models) {
            if (!model->isUploadComplete()) {
                continue;
            }
            DrawBatch *currentBatch = &currentForwardBatch;
            if (model->getRenderPath() == RENDER_PATH_DEFERRED) {
                currentBatch = &deferredBatches[model->getShadingModel()];
            } else {
                const auto shadingModel = model->getShadingModel();
                if (shadingModel != currentForwardBatch.shadingModel) {
                    if (!currentForwardBatch.meshIndices.empty()) {
                        forwardBatches.emplace_back(currentForwardBatch);
                        currentForwardBatch = {};
                    }
                    currentForwardBatch.shadingModel = shadingModel;
                }
            }

            auto &batch = *currentBatch;
            for (auto &meshSlot: model->getShaderMeshSlots()) {
                batch.meshIndices.push_back(static_cast<int>(meshSlot));
                batch.drawBufferAccesses.emplace_back(RenderScene::BufferAccessRange{
                    meshSlot * sizeof(ShaderDrawMesh::CPU), sizeof(ShaderDrawMesh::CPU)
                });
            }
            for (auto &mesh: model->getMeshes()) {
                for (auto attr = ATTRIBUTE_BEGIN;
                     attr <= ATTRIBUTE_END;
                     attr = static_cast<VertexAttribute>(attr + 1)) {
                    batch.vertexBufferAccesses[attr].emplace_back(RenderScene::BufferAccessRange{
                        mesh->getAllocation().baseVertex * getVertexAttributeSize(attr),
                        mesh->getAllocation().vertexCount * getVertexAttributeSize(attr)
                    });
                }
                batch.indexBufferAccesses.emplace_back(RenderScene::BufferAccessRange{
                    mesh->getAllocation().drawCall.offset,
                    mesh->getAllocation().drawCall.count * sizeof(unsigned int)
                });
            }
            batch.transformBufferAccesses.emplace_back(RenderScene::BufferAccessRange{
                model->getTransformSlot() * sizeof(ShaderTransform::CPU),
                sizeof(ShaderTransform::CPU)
            });
            batch.materialBufferAccesses.emplace_back(RenderScene::BufferAccessRange{
                model->getMaterial()->getSlot() * sizeof(ShaderMaterial::CPU),
                sizeof(ShaderMaterial::CPU)
            });
            for (auto i = 0; i < buffers.atlasTexture.getDescription().arrayLayers; i++) {
                // TODO: Granular texture access declaration
                batch.textureAccesses.emplace_back(i);
            }
            totalDrawCount += model->getShaderMeshSlots().size();
        }

        if (!currentForwardBatch.meshIndices.empty()) {
            forwardBatches.emplace_back(currentForwardBatch);
        }

        scene.transformBuffer = rg::Resource(buffers.transformBuffer);
        scene.materialBuffer = rg::Resource(buffers.materialBuffer);

        for (auto &pair: buffers.vertexBuffers) {
            scene.vertexBuffers.emplace(pair.first, rg::Resource(pair.second));
        }

        scene.indexBuffer = rg::Resource(buffers.indexBuffer);

        scene.textureAtlas = rg::Resource(buffers.atlasTexture);
        scene.tileMapBuffer = rg::Resource(buffers.tileMapBuffer);
        scene.tileMapOffsetsBuffer = rg::Resource(buffers.tileMapOffsetsBuffer);
        scene.residencyMapBuffer = rg::Resource(buffers.residencyMapBuffer);
        scene.residencyMapOffsetsBuffer = rg::Resource(buffers.residencyMapOffsetsBuffer);
        scene.readbackBuffer = rg::Resource(buffers.readbackBuffer);

        scene.atlasSize = buffers.atlasSize;
        scene.tileSize = buffers.tileSize;
        scene.tileBorder = buffers.tileBorder;
        scene.maxAnisotropy = buffers.maxAnisotropy;

        //TODO: Implement selective light buffers
        scene.pointLightBuffer = rg::Resource(buffers.pointLightBuffer);
        scene.spotLightBuffer = rg::Resource(buffers.spotLightBuffer);
        scene.directionalLightBuffer = rg::Resource(buffers.directionalLightBuffer);

        //TODO: Staging buffers for camera / meshIndices / config
        {
            ShaderCamera::CPU camera;
            camera.viewPosition = drawList.camera.getPosition();
            camera.view = drawList.camera.getView();
            camera.projection = drawList.camera.getProjection();
            const auto cameraMapping = heap.map(cameraBuffer);
            std::memcpy(cameraMapping->data(), &camera, sizeof(ShaderCamera::CPU));
        }
        scene.cameraBuffer = rg::Resource(cameraBuffer);

        {
            ShaderConfiguration::CPU config;
            config.gamma = drawList.config.gamma;
            config.ibl = false;
            const auto configMapping = heap.map(configBuffer);
            std::memcpy(configMapping->data(), &config, sizeof(ShaderConfiguration::CPU));
        }
        scene.configBuffer = rg::Resource(configBuffer);

        scene.normalizedQuad = allocator.getMeshStreamer().getAllocation(allocator.getMeshStreamer().getNormalizedQuad());
        scene.normalizedCube = allocator.getMeshStreamer().getAllocation(allocator.getMeshStreamer().getNormalizedCube());

        rg::ComputePassBuilder builder("ScenePrePass");

        if (totalDrawCount <= 0) {
            return builder.execute([](rg::ComputeContext &ctx) {
            });
        }

        scene.drawBuffer = graphBuilder.allocateBuffer(rg::Buffer(totalDrawCount * sizeof(ShaderDrawMesh::CPU),
                                                                  rg::Buffer::CAPABILITY_STORAGE,
                                                                  rg::Buffer::MEMORY_GPU_ONLY));

        meshIndicesBuffer = heap.allocateBuffer(rg::Buffer(totalDrawCount * sizeof(int),
                                                           rg::Buffer::CAPABILITY_STORAGE
                                                           | rg::Buffer::CAPABILITY_TRANSFER_DST,
                                                           rg::Buffer::MEMORY_CPU_TO_GPU));

        // Copy mesh indices in execution order
        {
            const auto meshIndicesMapping = heap.map(meshIndicesBuffer);
            size_t currentMeshIndex = 0;
            for (auto &batch: forwardBatches) {
                std::memcpy(meshIndicesMapping->data() + currentMeshIndex * sizeof(int),
                            batch.meshIndices.data(),
                            batch.meshIndices.size() * sizeof(int));
                currentMeshIndex += batch.meshIndices.size();
            }
            for (auto &batch: deferredBatches) {
                std::memcpy(meshIndicesMapping->data() + currentMeshIndex * sizeof(int),
                            batch.second.meshIndices.data(),
                            batch.second.meshIndices.size() * sizeof(int));
                currentMeshIndex += batch.second.meshIndices.size();
            }
        }

        scene.canvases = drawList.canvases;

        // TODO: Split draw batches across (oversized) batch counts to allow command buffer reuse.
        // TODO: Share single indirect / count buffer across batches.

        for (const auto &batch: forwardBatches) {
            const auto indirectBuffer = graphBuilder.allocateBuffer(rg::Buffer(
                sizeof(ShaderScript::ShaderDrawIndirectIndexed::CPU) * batch.meshIndices.size(),
                rg::Buffer::CAPABILITY_STORAGE | rg::Buffer::CAPABILITY_INDIRECT,
                rg::Buffer::MEMORY_GPU_ONLY));
            const auto indirectCountBuffer = graphBuilder.allocateBuffer(rg::Buffer(
                sizeof(int),
                rg::Buffer::CAPABILITY_STORAGE | rg::Buffer::CAPABILITY_INDIRECT,
                rg::Buffer::MEMORY_GPU_ONLY));
            scene.drawList.emplace_back(batch.meshIndices.size(),
                                        sizeof(ShaderScript::ShaderDrawIndirectIndexed::CPU),
                                        indirectBuffer,
                                        0,
                                        indirectCountBuffer,
                                        0,
                                        RENDER_PATH_FORWARD,
                                        batch.shadingModel,
                                        batch.drawBufferAccesses,
                                        batch.transformBufferAccesses,
                                        batch.materialBufferAccesses,
                                        batch.vertexBufferAccesses,
                                        batch.indexBufferAccesses,
                                        batch.textureAccesses);
            builder.storageWrite(indirectBuffer,
                                 0,
                                 batch.meshIndices.size() * sizeof(ShaderScript::ShaderDrawIndirectIndexed::CPU));
            builder.storageWrite(indirectCountBuffer,
                                 0,
                                 sizeof(int));
            for (auto &access: batch.drawBufferAccesses) {
                builder.storageWrite(scene.drawBuffer, access.offset, access.size);
            }
            for (auto &access: batch.transformBufferAccesses) {
                builder.storageRead(scene.transformBuffer, access.offset, access.size);
            }
            for (auto &index: batch.meshIndices) {
                builder.storageRead(buffers.meshBuffer, index * sizeof(ShaderMesh::CPU), sizeof(ShaderMesh::CPU));
            }
        }

        for (const auto &pair: deferredBatches) {
            const auto &batch = pair.second;
            const auto indirectBuffer = graphBuilder.allocateBuffer(rg::Buffer(
                sizeof(ShaderScript::ShaderDrawIndirectIndexed::CPU) * batch.meshIndices.size(),
                rg::Buffer::CAPABILITY_STORAGE | rg::Buffer::CAPABILITY_INDIRECT,
                rg::Buffer::MEMORY_GPU_ONLY));
            const auto indirectCountBuffer = graphBuilder.allocateBuffer(rg::Buffer(
                sizeof(int),
                rg::Buffer::CAPABILITY_STORAGE | rg::Buffer::CAPABILITY_INDIRECT,
                rg::Buffer::MEMORY_GPU_ONLY));
            scene.drawList.emplace_back(batch.meshIndices.size(),
                                        sizeof(ShaderScript::ShaderDrawIndirectIndexed::CPU),
                                        indirectBuffer,
                                        0,
                                        indirectCountBuffer,
                                        0,
                                        RENDER_PATH_DEFERRED,
                                        pair.first,
                                        batch.drawBufferAccesses,
                                        batch.transformBufferAccesses,
                                        batch.materialBufferAccesses,
                                        batch.vertexBufferAccesses,
                                        batch.indexBufferAccesses,
                                        batch.textureAccesses);
            builder.storageWrite(indirectBuffer,
                                 0,
                                 batch.meshIndices.size() * sizeof(ShaderScript::ShaderDrawIndirectIndexed::CPU));
            builder.storageWrite(indirectCountBuffer,
                                 0,
                                 sizeof(int));
            for (auto &access: batch.drawBufferAccesses) {
                builder.storageWrite(scene.drawBuffer, access.offset, access.size);
            }
            for (auto &access: batch.transformBufferAccesses) {
                builder.storageRead(scene.transformBuffer, access.offset, access.size);
            }
            for (auto &index: batch.meshIndices) {
                builder.storageRead(buffers.meshBuffer, index * sizeof(ShaderMesh::CPU), sizeof(ShaderMesh::CPU));
            }
        }

        builder.storageRead(scene.cameraBuffer, 0, 0);
        builder.storageRead(meshIndicesBuffer, 0, 0);

        return builder.execute([this, &scene, &buffers](rg::ComputeContext &ctx) {
            ctx.bindPipeline(scenePrepassPipeline);

            ctx.bindStorageBuffer("camera", scene.cameraBuffer, 0, 0);
            ctx.bindStorageBuffer("transforms", scene.transformBuffer, 0, 0);
            ctx.bindStorageBuffer("meshBuffer", buffers.meshBuffer, 0, 0);
            ctx.bindStorageBuffer("meshIndices", meshIndicesBuffer, 0, 0);
            ctx.bindStorageBuffer("drawBuffer", scene.drawBuffer, 0, 0);

            size_t currentMeshIndex = 0;
            for (auto &batch: scene.drawList) {
                ctx.bindStorageBuffer("commandBuffer", batch.indirectBuffer, 0, 0);
                ctx.bindStorageBuffer("commandCountBuffer", batch.indirectCountBuffer, 0, 0);
                ctx.setShaderParameter("meshIndexOffset",
                                       rg::ShaderPrimitive(static_cast<int>(currentMeshIndex)));
                ctx.setShaderParameter("batchSize", rg::ShaderPrimitive(static_cast<int>(batch.batchSize)));
                ctx.dispatch(Vec3u((batch.batchSize + (prePassLocalSize - 1)) / prePassLocalSize,
                                   1,
                                   1));
                currentMeshIndex += batch.batchSize;
            }
        });
    }
}
