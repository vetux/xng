/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2023  Julian Zampiccoli
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

#include <utility>

#include "xng/render/graph/framegraphpass.hpp"

namespace xng {
    FrameGraphBuilder::FrameGraphBuilder(RenderTarget &backBuffer,
                                         const Scene &scene,
                                         const GenericMapString &properties,
                                         std::set<FrameGraphResource> persistentResources,
                                         ShaderCompiler &shaderCompiler,
                                         ShaderDecompiler &shaderDecompiler)
            : backBuffer(backBuffer),
              scene(scene),
              properties(properties),
              persistentResources(std::move(persistentResources)),
              shaderCompiler(shaderCompiler),
              shaderDecompiler(shaderDecompiler) {}

    FrameGraphResource FrameGraphBuilder::createPipeline(const RenderPipelineDesc &desc) {
        auto ret = createResourceId();
        graph.allocations[ret] = FrameGraphAllocation{RenderObject::RENDER_OBJECT_RENDER_PIPELINE, desc};
        currentPass.allocations.insert(ret);
        return ret;
    }

    FrameGraphResource FrameGraphBuilder::createRenderPass(const RenderPassDesc &desc) {
        auto ret = createResourceId();
        graph.allocations[ret] = FrameGraphAllocation{RenderObject::RENDER_OBJECT_RENDER_PASS, desc};
        currentPass.allocations.insert(ret);
        return ret;
    }

    FrameGraphResource FrameGraphBuilder::createRenderTarget(const RenderTargetDesc &desc) {
        auto ret = createResourceId();
        graph.allocations[ret] = FrameGraphAllocation{RenderObject::RENDER_OBJECT_RENDER_TARGET, desc};
        currentPass.allocations.insert(ret);
        return ret;
    }

    FrameGraphResource FrameGraphBuilder::createTextureBuffer(const TextureBufferDesc &desc) {
        auto ret = createResourceId();
        graph.allocations[ret] = FrameGraphAllocation{RenderObject::RENDER_OBJECT_TEXTURE_BUFFER, desc};
        currentPass.allocations.insert(ret);
        return ret;
    }

    FrameGraphResource FrameGraphBuilder::createTextureArrayBuffer(const TextureArrayBufferDesc &desc) {
        auto ret = createResourceId();
        graph.allocations[ret] = FrameGraphAllocation{RenderObject::RENDER_OBJECT_TEXTURE_ARRAY_BUFFER, desc};
        currentPass.allocations.insert(ret);
        return ret;
    }

    FrameGraphResource FrameGraphBuilder::createVertexBuffer(const VertexBufferDesc &desc) {
        auto ret = createResourceId();
        graph.allocations[ret] = FrameGraphAllocation{RenderObject::RENDER_OBJECT_VERTEX_BUFFER, desc};
        currentPass.allocations.insert(ret);
        return ret;
    }

    FrameGraphResource FrameGraphBuilder::createIndexBuffer(const IndexBufferDesc &desc) {
        auto ret = createResourceId();
        graph.allocations[ret] = FrameGraphAllocation{RenderObject::RENDER_OBJECT_INDEX_BUFFER, desc};
        currentPass.allocations.insert(ret);
        return ret;
    }

    FrameGraphResource FrameGraphBuilder::createVertexArrayObject(const VertexArrayObjectDesc &desc) {
        auto ret = createResourceId();
        graph.allocations[ret] = FrameGraphAllocation{RenderObject::RENDER_OBJECT_VERTEX_ARRAY_OBJECT, desc};
        currentPass.allocations.insert(ret);
        return ret;
    }

    FrameGraphResource FrameGraphBuilder::createShaderBuffer(const ShaderBufferDesc &desc) {
        auto ret = createResourceId();
        graph.allocations[ret] = FrameGraphAllocation{RenderObject::RENDER_OBJECT_SHADER_BUFFER, desc};
        currentPass.allocations.insert(ret);
        return ret;
    }

    void FrameGraphBuilder::write(FrameGraphResource target) {
        currentPass.writes.insert(target);
    }

    void FrameGraphBuilder::read(FrameGraphResource source) {
        currentPass.reads.insert(source);
    }

    void FrameGraphBuilder::persist(FrameGraphResource resource) {
        currentPass.persists.insert(resource);
    }

    FrameGraphResource FrameGraphBuilder::getBackBuffer() {
        return FrameGraphResource(0);
    }

    RenderTargetDesc FrameGraphBuilder::getBackBufferDescription() {
        return backBuffer.getDescription();
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

    ShaderCompiler &FrameGraphBuilder::getShaderCompiler() {
        return shaderCompiler;
    }

    ShaderDecompiler &FrameGraphBuilder::getShaderDecompiler() {
        return shaderDecompiler;
    }

    FrameGraphResource FrameGraphBuilder::createResourceId() {
        if (resourceCounter >= std::numeric_limits<size_t>::max()) {
            throw std::runtime_error("Resource id counter overflow");
        }
        auto ret = resourceCounter++;
        while (persistentResources.find(FrameGraphResource(ret)) != persistentResources.end()) {
            if (resourceCounter >= std::numeric_limits<size_t>::max()) {
                throw std::runtime_error("Resource id counter overflow");
            }
            ret = resourceCounter++;
        }
        return FrameGraphResource(ret);
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
            exec.allocations = currentPass.allocations;
            exec.reads = currentPass.reads;
            exec.writes = currentPass.writes;
            exec.persists = currentPass.persists;
            graph.stages.emplace_back(exec);
        }

        return graph;
    }
}