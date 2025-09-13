/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2023  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 3 of the License, or (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.

 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <utility>

#include "xng/rendergraph/rendergraphbuilder.hpp"

namespace xng {
    RenderGraphBuilder::RenderGraphBuilder() {
        resourceCounter = 0;
        screenTexture = createResource();
    }

    RenderGraphResource RenderGraphBuilder::inheritResource(const RenderGraphResource resource) {
        const auto ret = createResource();
        inheritedResources[ret] = resource;
        return ret;
    }

    RenderGraphResource RenderGraphBuilder::createVertexBuffer(const size_t size) {
        const auto resource = createResource();
        vertexBufferAllocation[resource] = size;
        return resource;
    }

    RenderGraphResource RenderGraphBuilder::createIndexBuffer(const size_t size) {
        const auto resource = createResource();
        indexBufferAllocation[resource] = size;
        return resource;
    }

    RenderGraphResource RenderGraphBuilder::createShaderBuffer(const size_t size) {
        const auto resource = createResource();
        shaderBufferAllocation[resource] = size;
        return resource;
    }

    RenderGraphResource RenderGraphBuilder::createTexture(const RenderGraphTexture &texture) {
        const auto resource = createResource();
        textureAllocation[resource] = texture;
        return resource;
    }

    RenderGraphResource RenderGraphBuilder::createShader(const ShaderStage &shader) {
        const auto resource = createResource();
        shaderAllocation[resource] = shader;
        return resource;
    }

    RenderGraphResource RenderGraphBuilder::createPipeline(const std::unordered_set<RenderGraphResource> &shaders) {
        const auto resource = createResource();
        pipelineAllocation[resource] = shaders;
        return resource;
    }

    RenderGraphResource RenderGraphBuilder::getScreenTexture() const {
        return screenTexture;
    }

    RenderGraphBuilder::PassHandle RenderGraphBuilder::addPass(const std::string &name, std::function<void(RenderGraphContext &)> pass) {
        RenderGraphPass p;
        p.name = name;
        p.pass = std::move(pass);
        passes.emplace_back(p);
        return passes.size() - 1;
    }

    void RenderGraphBuilder::read(const PassHandle pass, const RenderGraphResource resource) {
        passes.at(pass).read.insert(resource);
    }

    void RenderGraphBuilder::write(const PassHandle pass, const RenderGraphResource resource) {
        passes.at(pass).write.insert(resource);
    }

    void RenderGraphBuilder::readWrite(const PassHandle pass, const RenderGraphResource resource) {
        passes.at(pass).read.insert(resource);
        passes.at(pass).write.insert(resource);
    }

    RenderGraph RenderGraphBuilder::build() {
        return {
            passes,
            vertexBufferAllocation,
            indexBufferAllocation,
            shaderBufferAllocation,
            textureAllocation,
            shaderAllocation,
            pipelineAllocation,
            inheritedResources,
            screenTexture
        };
    }

    RenderGraphResource RenderGraphBuilder::createResource() {
        return resourceCounter++;
    }
}
