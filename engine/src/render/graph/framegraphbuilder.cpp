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

#include "xng/render/graph/framegraphlayout.hpp"
#include "xng/render/graph/framegraphbuilder.hpp"

#include "xng/render/graph/framegraphpass.hpp"

namespace xng {
    FrameGraphBuilder::FrameGraphBuilder(RenderTarget &backBuffer,
                                         const Scene &scene,
                                         const GenericMapString &properties)
            : backBuffer(backBuffer),
              scene(scene),
              properties(properties) {}

    FrameGraphResource FrameGraphBuilder::createMeshBuffer(const ResourceHandle<Mesh> &h) {
        auto it = uriResources.find(h.getUri());
        if (it == uriResources.end()) {
            auto ret = FrameGraphResource(resourceCounter++);
            graph.allocations[ret] = FrameGraphAllocation{RenderObject::VERTEX_BUFFER,
                                                          true,
                                                          h.getUri()};
            currentPass.allocations.insert(ret);
            uriResources[h.getUri()] = ret;
            return ret;
        } else {
            auto ret = it->second;
            currentPass.allocations.insert(ret);
            return ret;
        }
    }

    FrameGraphResource FrameGraphBuilder::createTextureBuffer(const ResourceHandle<Texture> &h) {
        auto it = uriResources.find(h.getUri());
        if (it == uriResources.end()) {
            auto ret = FrameGraphResource(resourceCounter++);
            graph.allocations[ret] = FrameGraphAllocation{RenderObject::TEXTURE_BUFFER,
                                                          true,
                                                          h.getUri()};
            currentPass.allocations.insert(ret);
            uriResources[h.getUri()] = ret;
            return ret;
        } else {
            auto ret = it->second;
            currentPass.allocations.insert(ret);
            return ret;
        }
    }

    FrameGraphResource FrameGraphBuilder::createPipeline(const ResourceHandle<Shader> &shader,
                                                         const RenderPipelineDesc &desc) {
        auto ret = FrameGraphResource(resourceCounter++);
        graph.allocations[ret] = FrameGraphAllocation{RenderObject::RENDER_PIPELINE,
                                                      false,
                                                      std::make_pair<>(shader, desc)};
        currentPass.allocations.insert(ret);
        return ret;
    }

    FrameGraphResource FrameGraphBuilder::createRenderTarget(const RenderTargetDesc &desc) {
        auto ret = FrameGraphResource(resourceCounter++);
        graph.allocations[ret] = FrameGraphAllocation{RenderObject::RENDER_TARGET,
                                                      false,
                                                      desc};
        currentPass.allocations.insert(ret);
        return ret;
    }

    FrameGraphResource FrameGraphBuilder::createTextureBuffer(const TextureBufferDesc &attribs) {
        auto ret = FrameGraphResource(resourceCounter++);
        graph.allocations[ret] = FrameGraphAllocation{RenderObject::TEXTURE_BUFFER,
                                                      false,
                                                      attribs};
        currentPass.allocations.insert(ret);
        return ret;
    }

    FrameGraphResource FrameGraphBuilder::createShaderBuffer(const ShaderBufferDesc &desc) {
        auto ret = FrameGraphResource(resourceCounter++);
        graph.allocations[ret] = FrameGraphAllocation{RenderObject::SHADER_BUFFER,
                                                      false,
                                                      desc};
        currentPass.allocations.insert(ret);
        return ret;
    }

    void FrameGraphBuilder::write(FrameGraphResource target) {
        currentPass.writes.insert(target);
    }

    void FrameGraphBuilder::read(FrameGraphResource source) {
        currentPass.reads.insert(source);
    }

    FrameGraphResource FrameGraphBuilder::getBackBuffer() {
        return FrameGraphResource(0);
    }

    std::pair<Vec2i, int> FrameGraphBuilder::getBackBufferFormat() {
        return {backBuffer.getDescription().size, backBuffer.getDescription().samples};
    }

    const Scene &FrameGraphBuilder::getScene() {
        return scene;
    }

    const GenericMapString &FrameGraphBuilder::getProperties() {
        return properties;
    }

    GenericMapString &FrameGraphBuilder::getSharedData() {
        return sharedData;
    }

    FrameGraph FrameGraphBuilder::build(const FrameGraphLayout &layout) {
        sharedData.clear();
        graph = {};
        resourceCounter = 1;

        for (auto &pass: layout.getOrderedPasses()) {
            currentPass = {};
            pass.get().setup(*this);
            FrameGraph::Stage exec;
            exec.pass = pass.get().getTypeName();
            exec.resources = currentPass.allocations;
            exec.reads = currentPass.reads;
            exec.writes = currentPass.writes;
            graph.stages.emplace_back(exec);
        }

        return graph;
    }
}