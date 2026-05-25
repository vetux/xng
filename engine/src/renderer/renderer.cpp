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
          allocator(runtime.getResourceHeap(), streamingBudget),
          skinningPipeline(createPipeline(runtime.getPipelineCache(), skinningShader)),
          scenePrepassPipeline(createPipeline(runtime.getPipelineCache(), scenePrepassShader)) {
    }

    RenderAllocator &Renderer::getAllocator() {
        return allocator;
    }

    void Renderer::setPasses(std::vector<std::shared_ptr<RenderPass> > _passes) {
        passes = std::move(_passes);
    }

    void Renderer::draw(rg::Surface &surface, const RenderDrawList &drawList) {
        // Flush all referenced objects
        for (auto &model: drawList.models) {
            model->flush();
        }
        for (auto &light: drawList.directionalLights) {
            light->flush();
        }
        for (auto &light: drawList.pointLights) {
            light->flush();
        }
        for (auto &light: drawList.spotLights) {
            light->flush();
        }
        for (auto &canvas: drawList.canvases) {
            canvas->flush();
        }

        rg::GraphBuilder graph;

        // Commit streams
        const auto buffers = allocator.commit(graph);

        // Record compute skinning
        graph.addPass(recordSkinningPass(drawList, buffers));

        // Record prepass
        RenderScene scene;
        graph.addPass(recordScenePrePass(drawList, buffers, scene));

        // Record passes
        RenderPassRegistry registry;
        for (const auto &pass: passes) {
            pass->record(graph, surface, registry, scene);
        }

        runtime.execute(graph.build());
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
                ctx.dispatch(Vec3u((alloc.vertexCount + 63) / 64, 1, 1));
            }
        });
    }

    rg::ComputePass Renderer::recordScenePrePass(const RenderDrawList &drawList,
                                                 const RenderAllocator::Buffers &buffers,
                                                 RenderScene &scene) {
    }
}
