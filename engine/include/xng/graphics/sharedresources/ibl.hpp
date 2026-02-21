#ifndef XENGINE_IBL_HPP
#define XENGINE_IBL_HPP

#include "xng/rendergraph/rendergraphresource.hpp"
#include "xng/graphics/sharedresourceregistry.hpp"

namespace xng {
    struct IBLMaps final : SharedResource<RESOURCE_IBL> {
        RenderGraphResource environment; // original HDR equirectangular or cubemap if provided
        RenderGraphResource irradiance;  // cubemap
        RenderGraphResource prefilter;   // cubemap with mips
        RenderGraphResource brdfLUT;     // 2D texture
    };
}

#endif //XENGINE_IBL_HPP
