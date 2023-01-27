/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2022  Julian Zampiccoli
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

#ifndef XENGINE_PHONGRESOLVEPASS_HPP
#define XENGINE_PHONGRESOLVEPASS_HPP

#include "xng/render/graph/framegraphpass.hpp"

namespace xng {
    /**
     * The resolve pass creates a single color and depth texture from the deferred and forward phong shade textures,
     *  applies the shadow map and creates the phong shading compositor layer.
     *
     * Depends on PhongForwardPass, PhongDeferredPass and ShadowMappingPass
     */
    class PhongResolvePass : public FrameGraphPass {
    public:
        // FrameGraphResource to a Texture RGBA : The combined lighting textures with shadowing applied
        SHARED_PROPERTY(PhongResolvePass, COLOR)

        // FrameGraphResource to a Texture DEPTH_STENCIL : The combined depth textures
        SHARED_PROPERTY(PhongResolvePass, DEPTH)

        void setup(FrameGraphBuilder &builder) override;

        void execute(FrameGraphPassResources &resources) override;

        std::type_index getTypeName() override;
    };
}

#endif //XENGINE_PHONGRESOLVEPASS_HPP
