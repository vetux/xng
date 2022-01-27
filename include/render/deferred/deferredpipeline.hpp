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

#include "platform/graphics/renderer.hpp"
#include "platform/graphics/renderdevice.hpp"
#include "platform/graphics/shadercompiler.hpp"

#include "render/deferred/renderpass.hpp"
#include "render/deferred/compositor.hpp"
#include "render/deferred/gconstructor.hpp"
#include "render/pipeline.hpp"

#include "asset/manager/assetrendermanager.hpp"

namespace xengine {
    class XENGINE_EXPORT DeferredPipeline : public Pipeline {
    public:
        DeferredPipeline(RenderDevice &device,
                         AssetRenderManager &assetRenderManager,
                         GConstructor &gconstructor,
                         PassChain &chain,
                         Compositor &compositor);

        ~DeferredPipeline();

        void render(RenderTarget &target, Scene &scene) override;

        GBuffer &getGeometryBuffer();

    private:
        GBuffer geometryBuffer;

        AssetRenderManager &assetRenderManager;

        GConstructor &gconstructor;
        PassChain &chain;
        Compositor &compositor;
    };
}

#endif //XENGINE_DEFERREDPIPELINE_HPP
