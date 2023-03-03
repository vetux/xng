/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2022  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "xng/render/graph/framegraphbuilder.hpp"

#include "xng/render/graph/framegraphpass.hpp"

namespace xng {
    FrameGraphBuilder::FrameGraphBuilder(RenderTarget &backBuffer,
                                         const Scene &scene,
                                         const GenericMapString &properties)
            : backBuffer(backBuffer),
              scene(scene),
              properties(properties) {}

    FrameGraphResource FrameGraphBuilder::createPipeline(const RenderPipelineDesc &desc) {
        auto ret = FrameGraphResource(resourceCounter++);
        graph.allocations[ret] = FrameGraphAllocation{RenderObject::RENDER_OBJECT_RENDER_PIPELINE,
                                                      desc};
        currentPass.allocations.insert(ret);
        return ret;
    }

    FrameGraphResource FrameGraphBuilder::createRenderTarget(const RenderTargetDesc &desc) {
        auto ret = FrameGraphResource(resourceCounter++);
        graph.allocations[ret] = FrameGraphAllocation{RenderObject::RENDER_OBJECT_RENDER_TARGET,
                                                      desc};
        currentPass.allocations.insert(ret);
        return ret;
    }

    FrameGraphResource FrameGraphBuilder::createTextureBuffer(const TextureBufferDesc &attribs) {
        auto ret = FrameGraphResource(resourceCounter++);
        graph.allocations[ret] = FrameGraphAllocation{RenderObject::RENDER_OBJECT_TEXTURE_BUFFER,
                                                      attribs};
        currentPass.allocations.insert(ret);
        return ret;
    }

    FrameGraphResource FrameGraphBuilder::createShaderBuffer(const ShaderBufferDesc &desc) {
        auto ret = FrameGraphResource(resourceCounter++);
        graph.allocations[ret] = FrameGraphAllocation{RenderObject::RENDER_OBJECT_SHADER_BUFFER,
                                                      desc};
        currentPass.allocations.insert(ret);
        return ret;
    }

    FrameGraphResource FrameGraphBuilder::createMeshBuffer(const VertexBufferDesc &desc) {
        return FrameGraphResource();
    }

    void FrameGraphBuilder::write(FrameGraphResource target) {
        currentPass.writes.insert(target);
    }

    void FrameGraphBuilder::read(FrameGraphResource source) {
        currentPass.reads.insert(source);
    }

    void FrameGraphBuilder::persist(FrameGraphResource resource) {

    }

    FrameGraphResource FrameGraphBuilder::getBackBuffer() {
        return FrameGraphResource(0);
    }

    std::pair<Vec2i, int> FrameGraphBuilder::getBackBufferFormat() {
        return {backBuffer.getDescription().size, backBuffer.getDescription().samples};
    }

    const Scene &FrameGraphBuilder::getScene() const {
        return scene;
    }

    const GenericMapString &FrameGraphBuilder::getProperties() const {
        return properties;
    }

    GenericMapString &FrameGraphBuilder::getSharedData() {
        return sharedData;
    }

    FrameGraph FrameGraphBuilder::build(const std::vector<std::shared_ptr<FrameGraphPass>> &passes) {
        sharedData.clear();
        graph = {};
        resourceCounter = 1;

        for (auto &pass: passes) {
            currentPass = {};
            pass->setup(*this);
            FrameGraph::Stage exec;
            exec.pass = pass->getTypeIndex();
            exec.resources = currentPass.allocations;
            exec.reads = currentPass.reads;
            exec.writes = currentPass.writes;
            graph.stages.emplace_back(exec);
        }

        return graph;
    }
}