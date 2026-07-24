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
#include <memory>

#include "xng/io/byte.hpp"

namespace xng {
    static rg::PipelineCache::Handle createPipeline(rg::PipelineCache &cache, const rg::Shader &shader) {
        rg::ComputePipeline pip;
        pip.shader = shader;
        return cache.create(pip);
    }

    Renderer::Renderer(rg::Runtime &runtime,
                       const size_t streamingBudget,
                       const rg::Shader &skinningShader)
        : runtime(runtime),
          chunkStreamer(runtime.getResourceHeap(), KB(256), streamingBudget / KB(256)),
          skinningPipeline(createPipeline(runtime.getPipelineCache(), skinningShader)),
          streamingBudget(streamingBudget) {
        runtime.setEnableTimers(true);
    }

    std::shared_ptr<RenderScene> Renderer::createScene(size_t tileSize, size_t tileBorder, float maxAnisotropy) {
        return std::make_shared<RenderScene>(runtime, chunkStreamer, tileSize, tileBorder, maxAnisotropy);
    }

    void Renderer::setPasses(std::vector<std::shared_ptr<RenderPass> > _passes) {
        passes = std::move(_passes);
    }

    void Renderer::draw(const std::shared_ptr<rg::Surface> &surface, RenderScene &scene) {
        stats = {};
        stats.frameStart = std::chrono::high_resolution_clock::now();
        stats.streamingBudgetUsed = chunkStreamer.getInFlightChunks() * KB(256);
        stats.streamingBudgetMax = streamingBudget;

        RenderQueue queue;

        // Commit Scene
        scene.commit(queue);

        // Record compute skinning
        queue.getFrameBuilder().addPass(recordSkinningPass(scene));

        // Prepare Scene
        scene.prepare(queue);

        // Record passes
        RenderPassRegistry registry;
        for (const auto &pass: passes) {
            pass->record(queue.getFrameBuilder(), surface, registry, scene);
        }

        // Execute Graph
        const auto sem = queue.submit(runtime);

        // Sync every Frame (Frames in flight will do more advanced syncing here)
        static constexpr size_t timeOut = 10'000'000'000ULL;
        if (!sem->wait(timeOut)) {
            throw std::runtime_error("Renderer timed out");
        }

        stats.tilesInFlight = scene.getVirtualTextureStreamer().getTilesInFlight();
        stats.frameSubmit = sem->getTimeline().submitTimeHost;

        std::unordered_map<std::string, size_t> passSliceIndices;
        for (auto &slice: sem->getTimeline().slices) {
            const auto sliceDuration = slice.end - slice.start;
            if (passSliceIndices.find(slice.passName) != passSliceIndices.end()) {
                stats.gpuTime.at(passSliceIndices.at(slice.passName)).second += sliceDuration;
            } else {
                stats.gpuTime.emplace_back(slice.passName, sliceDuration);
                passSliceIndices.emplace(slice.passName, stats.gpuTime.size() - 1);
            }
        }
        stats.frameEnd = std::chrono::high_resolution_clock::now();
    }

    RendererStatistics Renderer::getStatistics() const {
        return stats;
    }

    rg::ComputePass Renderer::recordSkinningPass(const RenderScene &scene) const {
        std::vector<std::reference_wrapper<const RenderMesh> > meshes;

        const auto &sceneMeshes = scene.getMeshes();
        for (auto &id: scene.getSkinnedMeshes()) {
            meshes.emplace_back(sceneMeshes.at(id));
        }

        rg::ComputePassBuilder builder("SkinningPass");

        if (meshes.empty()) {
            return builder.execute([](rg::ComputeContext &ctx) {
            });
        }

        // Declare Accesses
        std::unordered_set<RenderObject::ID> processedSkeletons;
        for (auto &mesh: meshes) {
            auto skeleton = mesh.get().getSkeleton();

            assert(skeleton.isAssigned());

            if (processedSkeletons.find(skeleton.getID()) == processedSkeletons.end()) {
                builder.storageRead(scene.getSkeletonStreamer().getBuffer(),
                                    skeleton.get().getBaseBone() * sizeof(Mat4f),
                                    skeleton.get().getOffsets().size() * sizeof(Mat4f));
                processedSkeletons.insert(skeleton.getID());
            }

            const auto bonePosSize = getVertexAttributeSize(POSITION);
            constexpr auto boneIndexSize = sizeof(int) * 4;
            constexpr auto boneWeightSize = sizeof(float) * 4;

            const auto &alloc = mesh.get().getAllocation();

            builder.storageRead(scene.getMeshStreamer().getSkinnedBoneIndicesBuffer(),
                                alloc.skinBaseVertex * boneIndexSize,
                                alloc.vertexCount * boneIndexSize);
            builder.storageRead(scene.getMeshStreamer().getSkinnedBoneWeightsBuffer(),
                                alloc.skinBaseVertex * boneWeightSize,
                                alloc.vertexCount * boneWeightSize);

            builder.storageRead(scene.getMeshStreamer().getSkinnedBindPosBuffer(),
                                alloc.skinBaseVertex * bonePosSize,
                                alloc.vertexCount * bonePosSize);
            builder.storageWrite(scene.getMeshStreamer().getVertexBuffers().at(POSITION),
                                 alloc.baseVertex * bonePosSize,
                                 alloc.vertexCount * bonePosSize);
        }

        // Execute
        return builder.execute([this, &scene, meshes = std::move(meshes)](rg::ComputeContext &ctx) {
            ctx.bindPipeline(skinningPipeline);
            ctx.bindStorageBuffer("bones", scene.getSkeletonStreamer().getBuffer(), 0, 0);
            ctx.bindStorageBuffer("positions", scene.getMeshStreamer().getSkinnedBindPosBuffer(), 0, 0);
            ctx.bindStorageBuffer("boneIds", scene.getMeshStreamer().getSkinnedBoneIndicesBuffer(), 0, 0);
            ctx.bindStorageBuffer("boneWeights", scene.getMeshStreamer().getSkinnedBoneWeightsBuffer(), 0, 0);
            ctx.bindStorageBuffer("skinnedPositions", scene.getMeshStreamer().getVertexBuffers().at(POSITION), 0, 0);
            for (auto &mesh: meshes) {
                const auto &alloc = mesh.get().getAllocation();
                const auto baseBone = mesh.get().getSkeleton().get().getBaseBone();
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
}
