/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2023  Julian Zampiccoli
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

#include "xng/render/scene.hpp"
#include "xng/render/graph/framegraphpass.hpp"

namespace xng {
    /**
     * The deferred SHADE_PHONG* shading model implementation for non transparent objects.
     *
     * Creates the deferred phong shade compositor layer.
     *
     * Depends on GBufferPass and ShadowMappingPass
     */
    class XENGINE_EXPORT PhongDeferredPass : public FrameGraphPass {
    public:
        // FrameGraphResource to a Texture RGBA : Contains the combined color values with shadowing applied
        SHARED_PROPERTY(PhongDeferredPass, COLOR)

        // FrameGraphResource to a Texture DEPTH_STENCIL : Contains the depth values for the compositor
        SHARED_PROPERTY(PhongDeferredPass, DEPTH)

        PhongDeferredPass();

        ~PhongDeferredPass() override = default;

        void setup(FrameGraphBuilder &builder) override;

        void execute(FrameGraphPassResources &resources) override;

        std::type_index getTypeIndex() const override;

    private:
        Mesh mesh = Mesh::normalizedQuad();

        FrameGraphResource pipelineRes;
        FrameGraphResource passRes;

        FrameGraphResource shaderBufferRes;

        FrameGraphResource vertexBufferRes;
        FrameGraphResource vertexArrayObjectRes;

        SPIRVBundle vsb;
        SPIRVBundle fsb;

        bool quadAllocated = false;

        FrameGraphResource gBufferPosition;
        FrameGraphResource gBufferNormal;
        FrameGraphResource gBufferTangent;
        FrameGraphResource gBufferRoughnessMetallicAO;
        FrameGraphResource gBufferAlbedo;
        FrameGraphResource gBufferAmbient;
        FrameGraphResource gBufferSpecular;
        FrameGraphResource gBufferModelObject;
        FrameGraphResource gBufferDepth;

        Camera camera;
    };
}

#endif //XENGINE_PHONGDEFERREDPASS_HPP
