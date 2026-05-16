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

    Renderer::Renderer(rg::Runtime &runtime, const rg::Shader &skinningShader, const rg::Shader &scenePrepassShader)
        : runtime(runtime),
          allocator(runtime.getResourceHeap()),
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
        auto buffers = allocator.commit(graph);

        // Record compute skinning
        graph.addPass(recordSkinningPass(drawList, buffers));

        // Record prepass
        RenderScene scene;
        graph.addPass(recordScenePrePass(drawList, buffers, scene));

        // Record passes
        RenderPassRegistry registry;
        for (const auto &pass : passes) {
            pass->record(graph, surface, registry, scene);
        }

        runtime.execute(graph.build());
    }

    rg::ComputePass Renderer::recordSkinningPass(const RenderDrawList &drawList, const RenderAllocator::Buffers &buffers) {
    }

    rg::ComputePass Renderer::recordScenePrePass(const RenderDrawList &drawList, const RenderAllocator::Buffers &buffers, RenderScene &scene) {
    }
}
