/**
 *  xEngine - C++ game engine library
 *  Copyright (C) 2021  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "render/graph/framegraphbuilder.hpp"

#include "render/graph/renderpass.hpp"

namespace xengine {
    FrameGraphBuilder::FrameGraphBuilder(RenderTarget &backBuffer,
                                         ObjectPool &pool,
                                         Vec2i renderResolution,
                                         int renderSamples)
            : pool(pool), backBuffer(backBuffer), renderRes(renderResolution), renderSamples(renderSamples) {
        std::function<std::reference_wrapper<RenderObject>()> f = [&backBuffer]() {
            return std::reference_wrapper<RenderObject>(dynamic_cast<RenderObject &>(backBuffer));
        };
        resources.emplace_back(std::move(f));
    }

    FrameGraphResource FrameGraphBuilder::createMeshBuffer(const Mesh &mesh) {
        std::function<std::reference_wrapper<RenderObject>()> f = [this, &mesh]() {
            return std::reference_wrapper<RenderObject>(dynamic_cast<RenderObject &>(pool.getMeshBuffer(mesh)));
        };
        resources.emplace_back(std::move(f));
        auto ret = FrameGraphResource(resources.size() - 1);
        passResources.at(currentPass).insert(ret);
        return ret;
    }

    FrameGraphResource FrameGraphBuilder::createInstancedMeshBuffer(const Mesh &mesh,
                                                                    const std::vector<Transform> &offsets) {
        std::function<std::reference_wrapper<RenderObject>()> f = [this, &mesh, &offsets]() {
            return std::reference_wrapper<RenderObject>(
                    dynamic_cast<RenderObject &>(pool.getInstancedBuffer(mesh, offsets)));
        };
        resources.emplace_back(std::move(f));
        auto ret = FrameGraphResource(resources.size() - 1);
        passResources.at(currentPass).insert(ret);
        return ret;
    }

    FrameGraphResource FrameGraphBuilder::createTextureBuffer(const Texture &texture) {
        std::function<std::reference_wrapper<RenderObject>()> f = [this, &texture]() {
            return std::reference_wrapper<RenderObject>(dynamic_cast<RenderObject &>(pool.getTextureBuffer(texture)));
        };
        resources.emplace_back(std::move(f));
        auto ret = FrameGraphResource(resources.size() - 1);
        passResources.at(currentPass).insert(ret);
        return ret;
    }

    FrameGraphResource FrameGraphBuilder::createShader(const Shader &shader) {
        std::function<std::reference_wrapper<RenderObject>()> f = [this, &shader]() {
            return std::reference_wrapper<RenderObject>(dynamic_cast<RenderObject &>(pool.getShaderProgram(shader)));
        };
        resources.emplace_back(std::move(f));
        auto ret = FrameGraphResource(resources.size() - 1);
        passResources.at(currentPass).insert(ret);
        return ret;
    }

    FrameGraphResource FrameGraphBuilder::createRenderTarget(Vec2i size, int samples) {
        std::function<std::reference_wrapper<RenderObject>()> f = [this, size, samples]() {
            return std::reference_wrapper<RenderObject>(
                    dynamic_cast<RenderObject &>(pool.getRenderTarget(size, samples)));
        };
        resources.emplace_back(std::move(f));
        auto ret = FrameGraphResource(resources.size() - 1);
        passResources.at(currentPass).insert(ret);
        return ret;
    }

    void FrameGraphBuilder::write(FrameGraphResource target) {}

    void FrameGraphBuilder::read(FrameGraphResource source) {}

    FrameGraphResource FrameGraphBuilder::getBackBuffer() {
        return FrameGraphResource(0);
    }

    std::pair<Vec2i, int> FrameGraphBuilder::getBackBufferFormat() {
        return {backBuffer.getSize(), backBuffer.getSamples()};
    }

    std::pair<Vec2i, int> FrameGraphBuilder::getRenderFormat() {
        return {renderRes, renderSamples};
    }

    FrameGraph FrameGraphBuilder::build(const std::vector<std::shared_ptr<RenderPass>> &passes) {
        auto backBuffer = resources.at(0);
        layers.clear();
        resources.clear();
        resources.emplace_back(backBuffer);
        passResources.clear();
        currentPass = 0;
        for (auto &pass: passes) {
            pass->setup(*this);
            currentPass++;
        }
        return {passes, passResources, resources};
    }
}