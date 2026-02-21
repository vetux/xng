#ifndef XENGINE_ENVIRONMENTMAP_HPP
#define XENGINE_ENVIRONMENTMAP_HPP

#include "xng/rendergraph/rendergraphresource.hpp"
#include "xng/graphics/sharedresourceregistry.hpp"

namespace xng {
    struct EnvironmentMap final : SharedResource<RESOURCE_ENVIRONMENT_MAP> {
        RenderGraphResource hdrTexture; // equirectangular HDR or cubemap provided by user
    };
}

#endif //XENGINE_ENVIRONMENTMAP_HPP
