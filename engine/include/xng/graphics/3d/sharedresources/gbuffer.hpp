/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
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

#ifndef XENGINE_GBUFFER_HPP
#define XENGINE_GBUFFER_HPP

#include "xng/rendergraph/rendergraphresource.hpp"

#include "xng/graphics/3d/sharedresourceregistry.hpp"

namespace xng {
    struct GBuffer final : SharedResource<RESOURCE_GEOMETRY_BUFFER> {
        RenderGraphResource gBufferPosition;
        RenderGraphResource gBufferNormal;
        RenderGraphResource gBufferTangent;
        RenderGraphResource gBufferRoughnessMetallicAmbientOcclusion;
        RenderGraphResource gBufferAlbedo;
        RenderGraphResource gBufferObjectShadows;
        RenderGraphResource gBufferDepth;
    };
}

#endif //XENGINE_GBUFFER_HPP
