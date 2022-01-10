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

#ifndef XENGINE_RENDERPASS_HPP
#define XENGINE_RENDERPASS_HPP

#include "renderpass.hpp"
#include "asset/scene.hpp"
#include "geometrybuffer.hpp"

#include "platform/graphics/renderer.hpp"
#include "platform/graphics/renderdevice.hpp"

namespace xengine {
    /**
     * Users may extend this class XENGINE_EXPORT and create custom render passes.
     */
    class XENGINE_EXPORT RenderPass {
    public:
        virtual ~RenderPass() = default;

        /**
         * Create the textures required for this render pass in the geometry buffer.
         */
        virtual void prepareBuffer(GeometryBuffer &gBuffer) {};

        /**
         * Run the pass and store the results in the geometry buffer.
         *
         * @param gBuffer
         * @param scene
         */
        virtual void render(GeometryBuffer &gBuffer, Scene &scene, AssetRenderManager &assetRenderManager) {};
    };
}

#endif //XENGINE_RENDERPASS_HPP
