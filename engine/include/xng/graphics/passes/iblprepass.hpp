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

#ifndef XENGINE_IBLPREPASS_HPP
#define XENGINE_IBLPREPASS_HPP

#include "xng/graphics/renderconfiguration.hpp"
#include "xng/graphics/renderpass.hpp"
#include "xng/graphics/sharedresourceregistry.hpp"
#include "xng/graphics/sharedresources/ibl.hpp"
#include "xng/rendergraph/shaderscript/shaderstruct.hpp"

namespace xng
{
    class XENGINE_EXPORT IBLPrePass final : public RenderPass
    {
    public:
        IBLPrePass(std::shared_ptr<RenderConfiguration> config,
                   std::shared_ptr<SharedResourceRegistry> registry);

        ~IBLPrePass() override = default;

        void create(RenderGraphBuilder& builder) override;

        void recreate(RenderGraphBuilder& builder) override;

        bool shouldRebuild(const Vec2i& backBufferSize) override;

    private:
        void runPass(RenderGraphContext& ctx);

        RenderGraphPipeline makeBRDFPipeline();

        RenderGraphPipeline makeEquirectToCubePipeline();

        RenderGraphPipeline makeIrradiancePipeline();

        RenderGraphPipeline makePrefilterPipeline();

        ShaderStruct(CaptureFaceData, Mat4f, captureMatrix)

        ShaderStruct(PrefilterParams, Vec4f, prefilterParams)

        std::shared_ptr<RenderConfiguration> config;
        std::shared_ptr<SharedResourceRegistry> registry;

        RenderGraphResource irradiance;
        RenderGraphResource prefilter;
        RenderGraphResource brdfLut;
        RenderGraphResource env;
        RenderGraphResource environmentCube;

        RenderGraphResource pipelineBRDF;
        RenderGraphResource pipelineEquirectToCube;
        RenderGraphResource pipelineIrradiance;
        RenderGraphResource pipelinePrefilter;

        RenderGraphResource vertexBuffer;
        RenderGraphResource indexBuffer;
        RenderGraphResource cubeVertexBuffer;
        RenderGraphResource captureBuffer;
        RenderGraphResource brdfParamBuffer;
        RenderGraphResource prefilterParamBuffer;

        Vec2i cubemapSize{512, 512};
        Vec2i prefilterSize{128, 128};
        Vec2i irradianceSize{128, 128};
        Vec2i brdfSize{512, 512};

        ResourceHandle<ImageRGBF> currentHDRI;
        Uri builtHDRI;
    };
}

#endif //XENGINE_IBLPREPASS_HPP
