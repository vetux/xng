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

#include "xng/render/graph2/fgbuilder.hpp"

namespace xng {
    FGBuilder::FGBuilder() {
        resourceCounter = 0;
        screenTexture = createResource();
    }

    FGResource FGBuilder::inheritResource(const FGResource resource) {
        const auto ret = createResource();
        inheritedResources[ret] = resource;
        return ret;
    }

    FGResource FGBuilder::createVertexBuffer(const size_t size) {
        const auto resource = createResource();
        vertexBufferAllocation[resource] = size;
        return resource;
    }

    FGResource FGBuilder::createIndexBuffer(const size_t size) {
        const auto resource = createResource();
        indexBufferAllocation[resource] = size;
        return resource;
    }

    FGResource FGBuilder::createShaderBuffer(const size_t size) {
        const auto resource = createResource();
        shaderBufferAllocation[resource] = size;
        return resource;
    }

    FGResource FGBuilder::createTexture(const FGTexture &texture) {
        const auto resource = createResource();
        textureAllocation[resource] = texture;
        return resource;
    }

    FGResource FGBuilder::createShader(const FGShaderSource &shader) {
        const auto resource = createResource();
        shaderAllocation[resource] = shader;
        return resource;
    }

    FGResource FGBuilder::getScreenTexture() const {
        return screenTexture;
    }

    FGBuilder::PassHandle FGBuilder::addPass(const std::string &name, std::function<void(FGContext &)> pass) {
        FGPass p;
        p.name = name;
        p.pass = std::move(pass);
        passes.emplace_back(p);
        return passes.size() - 1;
    }

    void FGBuilder::read(const PassHandle pass, const FGResource resource) {
        passes.at(pass).read.insert(resource);
    }

    void FGBuilder::write(const PassHandle pass, const FGResource resource) {
        passes.at(pass).write.insert(resource);
    }

    void FGBuilder::readWrite(const PassHandle pass, const FGResource resource) {
        passes.at(pass).read.insert(resource);
        passes.at(pass).write.insert(resource);
    }

    FGGraph FGBuilder::build() {
        return {
            passes,
            vertexBufferAllocation,
            indexBufferAllocation,
            shaderBufferAllocation,
            textureAllocation,
            shaderAllocation,
            inheritedResources,
            screenTexture
        };
    }

    FGResource FGBuilder::createResource() {
        return resourceCounter++;
    }
}
