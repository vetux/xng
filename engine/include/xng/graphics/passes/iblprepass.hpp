#ifndef XENGINE_IBLPREPASS_HPP
#define XENGINE_IBLPREPASS_HPP

#include "xng/graphics/renderconfiguration.hpp"
#include "xng/graphics/renderpass.hpp"
#include "xng/graphics/sharedresourceregistry.hpp"
#include "xng/graphics/sharedresources/ibl.hpp"
#include "xng/graphics/sharedresources/environmentmap.hpp"

namespace xng {
    class XENGINE_EXPORT IBLPrePass final : public RenderPass {
    public:
        IBLPrePass(std::shared_ptr<RenderConfiguration> config,
                   std::shared_ptr<SharedResourceRegistry> registry);

        ~IBLPrePass() override = default;

        void create(RenderGraphBuilder &builder) override;

        void recreate(RenderGraphBuilder &builder) override;

        bool shouldRebuild(const Vec2i &backBufferSize) override;

    private:
        void runPass(RenderGraphContext &ctx);

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
