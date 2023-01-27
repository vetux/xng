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

#ifndef XENGINE_PHONGDEFERREDPASS_HPP
#define XENGINE_PHONGDEFERREDPASS_HPP

#include "xng/asset/scene.hpp"
#include "xng/render/graph/framegraphpass.hpp"

namespace xng {
    /**
     * The deferred SHADE_PHONG* shading model implementation for non transparent objects.
     *
     * Depends on GBufferPass
     */
    class XENGINE_EXPORT PhongDeferredPass : public FrameGraphPass {
    public:
        // FrameGraphResource to a Texture RGBA : Contains the ambient color values
        SHARED_PROPERTY(PhongDeferredPass, AMBIENT)

        // FrameGraphResource to a Texture RGBA : Contains the diffuse color values
        SHARED_PROPERTY(PhongDeferredPass, DIFFUSE)

        // FrameGraphResource to a Texture RGBA : Contains the specular color values
        SHARED_PROPERTY(PhongDeferredPass, SPECULAR)

        PhongDeferredPass();

        ~PhongDeferredPass() override = default;

        void setup(FrameGraphBuilder &builder) override;

        void execute(FrameGraphPassResources &resources) override;

        std::type_index getTypeName() override;

    private:
        FrameGraphResource shader;
        FrameGraphResource quadMesh;

        FrameGraphResource renderTarget;
        FrameGraphResource multiSampleRenderTarget;

        FrameGraphResource colorMultisample;
        FrameGraphResource depthMultisample;

        FrameGraphResource outColor;
        FrameGraphResource outDepth;
    };
}

#endif //XENGINE_PHONGDEFERREDPASS_HPP
