/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2024  Julian Zampiccoli
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

#ifndef XENGINE_DEFERREDLIGHTINGPASS_HPP
#define XENGINE_DEFERREDLIGHTINGPASS_HPP

#include "xng/render/scene/scene.hpp"
#include "xng/render/graph/framegraphpass.hpp"
#include "xng/render/graph/framegraphresource.hpp"

namespace xng {
    /**
     * The deferred shading model implementation for non transparent objects.
     *
     * Writes SLOT_DEFERRED_COLOR and SLOT_DEFERRED_DEPTH.
     *
     * Reads SLOT_SHADOW_MAP_* and SLOT_GBUFFER_*.
     */
    class XENGINE_EXPORT DeferredLightingPass : public FrameGraphPass {
    public:
        ~DeferredLightingPass() override = default;

        void setup(FrameGraphBuilder &builder) override;

        std::type_index getTypeIndex() const override;

    private:
        void generateEnvironmentMaps(FrameGraphBuilder &builder, const ResourceHandle <ImageRGBA>& hdrImage);

        Mesh quadMesh = Mesh::normalizedQuad();
        Mesh cubeMesh = Mesh::normalizedCube();

        FrameGraphResource quadVertexBuffer;
        FrameGraphResource cubeVertexBuffer;

        FrameGraphResource pipeline;
        FrameGraphResource convertMapPipeline;
        FrameGraphResource prefilterPipeline;
        FrameGraphResource irradiancePipeline;
        FrameGraphResource brdfPipeline;

        FrameGraphResource environmentMap;
        FrameGraphResource prefilterMap;
        FrameGraphResource irradianceMap;
        FrameGraphResource brdfMap;

        Uri currentEnvironmentTexture;
    };
}

#endif //XENGINE_DEFERREDLIGHTINGPASS_HPP
