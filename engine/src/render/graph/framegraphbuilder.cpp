/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2021  Julian Zampiccoli
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

#include "render/graph/framegraphbuilder.hpp"

#include "render/graph/framegraphpass.hpp"

namespace xng {
    FrameGraphBuilder::FrameGraphBuilder(RenderTarget &backBuffer,
                                         FrameGraphPool &pool,
                                         const Scene &scene,
                                         Vec2i renderResolution,
                                         int renderSamples)
            : pool(pool),
              backBuffer(backBuffer),
              scene(scene),
              renderRes(renderResolution),
              renderSamples(renderSamples) {
        std::function<std::reference_wrapper<RenderObject>()> f = [&backBuffer]() {
            return std::reference_wrapper<RenderObject>(dynamic_cast<RenderObject &>(backBuffer));
        };
        resources.emplace_back(std::move(f));
    }

    FrameGraphResource FrameGraphBuilder::createMeshBuffer(const ResourceHandle<Mesh> &h) {
        std::function<std::reference_wrapper<RenderObject>()> f = [this, h]() {
            return std::reference_wrapper<RenderObject>(
                    dynamic_cast<RenderObject &>(pool.getMesh(h)));
        };
        resources.emplace_back(std::move(f));
        auto ret = FrameGraphResource(resources.size() - 1);
        passResources.at(currentPass).insert(ret);
        return ret;
    }

    FrameGraphResource FrameGraphBuilder::createTextureBuffer(const ResourceHandle<Texture> &h) {
        std::function<std::reference_wrapper<RenderObject>()> f = [this, h]() {
            return std::reference_wrapper<RenderObject>(
                    dynamic_cast<RenderObject &>(pool.getTexture(h)));
        };
        resources.emplace_back(std::move(f));
        auto ret = FrameGraphResource(resources.size() - 1);
        passResources.at(currentPass).insert(ret);
        return ret;
    }

    FrameGraphResource FrameGraphBuilder::createShader(const ResourceHandle<Shader> &h) {
        std::function<std::reference_wrapper<RenderObject>()> f = [this, h]() {
            return std::reference_wrapper<RenderObject>(dynamic_cast<RenderObject &>(pool.getShader(h)));
        };
        resources.emplace_back(std::move(f));
        auto ret = FrameGraphResource(resources.size() - 1);
        passResources.at(currentPass).insert(ret);
        return ret;
    }

    FrameGraphResource FrameGraphBuilder::createTextureBuffer(const TextureBufferDesc &desc) {
        std::function<std::reference_wrapper<RenderObject>()> f = [this, desc]() {
            return std::reference_wrapper<RenderObject>(dynamic_cast<RenderObject &>(pool.createTextureBuffer(desc)));
        };
        resources.emplace_back(std::move(f));
        auto ret = FrameGraphResource(resources.size() - 1);
        passResources.at(currentPass).insert(ret);
        return ret;
    }

    FrameGraphResource FrameGraphBuilder::createRenderTarget(Vec2i size, int samples) {
        std::function<std::reference_wrapper<RenderObject>()> f = [this, size, samples]() {
            return std::reference_wrapper<RenderObject>(
                    dynamic_cast<RenderObject &>(pool.createRenderTarget({.size = size, .samples = samples})));
        };
        resources.emplace_back(std::move(f));
        auto ret = FrameGraphResource(resources.size() - 1);
        passResources.at(currentPass).insert(ret);
        return ret;
    }

    FrameGraphResource FrameGraphBuilder::createPipeline(const ResourceHandle<Shader> &shader,
                                                         const RenderPipelineDesc &desc) {
        std::function<std::reference_wrapper<RenderObject>()> f = [this, shader, desc]() {
            return std::reference_wrapper<RenderObject>(dynamic_cast<RenderObject &>(pool.getPipeline(shader, desc)));
        };
        resources.emplace_back(std::move(f));
        auto ret = FrameGraphResource(resources.size() - 1);
        passResources.at(currentPass).insert(ret);
        return ret;
    }

    void FrameGraphBuilder::write(FrameGraphResource target) {
        passResources.at(currentPass).insert(target);
    }

    void FrameGraphBuilder::read(FrameGraphResource source) {
        passResources.at(currentPass).insert(source);
    }

    FrameGraphResource FrameGraphBuilder::getBackBuffer() {
        auto resource = FrameGraphResource(0);
        passResources.at(currentPass).insert(resource);
        return resource;
    }

    std::pair<Vec2i, int> FrameGraphBuilder::getBackBufferFormat() {
        return {backBuffer.getDescription().size, backBuffer.getDescription().samples};
    }

    std::pair<Vec2i, int> FrameGraphBuilder::getRenderFormat() {
        return {renderRes, renderSamples};
    }

    const Scene &FrameGraphBuilder::getScene() {
        return scene;
    }

    FrameGraph FrameGraphBuilder::build(const std::vector<std::shared_ptr<FrameGraphPass>> &passes) {
        auto screen = resources.at(0);
        resources.clear();
        resources.emplace_back(screen);
        passResources.clear();
        passResources.resize(1);
        currentPass = 0;
        for (auto &pass: passes) {
            pass->setup(*this);
            currentPass++;
            passResources.resize(currentPass + 1);
        }
        return {passes, passResources, resources};
    }
}