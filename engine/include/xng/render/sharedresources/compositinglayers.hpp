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

#ifndef XENGINE_COMPOSITINGLAYERS_HPP
#define XENGINE_COMPOSITINGLAYERS_HPP

#include "xng/rendergraph/rendergraphresource.hpp"

#include "xng/render/sharedresourceregistry.hpp"

namespace xng {
    struct CompositeLayer {
        RenderGraphResource color;
        RenderGraphResource depth;
        bool containsTransparency = false;
    };

    /**
     * The layers are presented by overlapping each layer from layers[0] to layers[end]
     * ontop of the previous performing depth testing if the depth texture is assigned.
     */
    struct CompositingLayers final : SharedResource<RESOURCE_COMPOSITE_TEXTURES> {
        std::vector<CompositeLayer> layers;
    };
}

#endif //XENGINE_COMPOSITINGLAYERS_HPP
