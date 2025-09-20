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

#ifndef XENGINE_CANVASTEXTURES_HPP
#define XENGINE_CANVASTEXTURES_HPP

#include "xng/rendergraph/rendergraphresource.hpp"

#include "xng/graphics/3d/sharedresourceregistry.hpp"
#include "xng/math/transform.hpp"

namespace xng {
    struct Canvas {
        Transform transform;
        bool worldSpace;
        RenderGraphResource texture;
    };

    /**
     * The RenderPass2D creates a canvas for each render batch that doesn't have a render target.
     */
    struct CanvasTextures final : SharedResource<RESOURCE_CANVAS_TEXTURES> {
        std::vector<Canvas> canvases;
    };
}

#endif //XENGINE_CANVASTEXTURES_HPP