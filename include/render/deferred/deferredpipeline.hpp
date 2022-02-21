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

#ifndef XENGINE_DEFERREDPIPELINE_HPP
#define XENGINE_DEFERREDPIPELINE_HPP

#include <utility>
#include <map>
#include <typeindex>

#include "graphics/renderer.hpp"
#include "graphics/renderdevice.hpp"

#include "shader/shadercompiler.hpp"

#include "render/deferred/prepass.hpp"
#include "render/deferred/renderpass.hpp"
#include "render/deferred/compositor.hpp"
#include "render/pipeline.hpp"

#include "asset/manager/assetrendermanager.hpp"

namespace xengine {
    class XENGINE_EXPORT DeferredPipeline : public Pipeline {
    public:
        explicit DeferredPipeline(RenderDevice &device,
                                  AssetManager &assetManager,
                                  std::vector<std::unique_ptr<RenderPass>> passes);

        DeferredPipeline(RenderDevice &device,
                         std::unique_ptr<PrePass> prePass,
                         std::unique_ptr<GBuffer> gBuffer,
                         std::unique_ptr<Compositor> compositor,
                         std::unique_ptr<AssetRenderManager> assetRenderManager,
                         std::vector<std::unique_ptr<RenderPass>> passes);

        ~DeferredPipeline();

        void render(RenderTarget &target, Scene &scene) override;

        AssetRenderManager &getAssetRenderManager() { return *assetRenderManager; }

        PrePass &getPrePass() { return *prePass; }

        GBuffer &getGeometryBuffer() { return *gBuffer; }

        Compositor &getCompositor() { return *compositor; }

        std::vector<std::unique_ptr<RenderPass>> &getPasses() { return passes; }

        void setPasses(std::vector<std::unique_ptr<RenderPass>> value) { passes = std::move(value); }

    private:
        std::unique_ptr<PrePass> prePass;
        std::unique_ptr<GBuffer> gBuffer;
        std::unique_ptr<Compositor> compositor;
        std::unique_ptr<AssetRenderManager> assetRenderManager;

        std::vector<std::unique_ptr<RenderPass>> passes;
    };
}

#endif //XENGINE_DEFERREDPIPELINE_HPP
