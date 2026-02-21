/*
 * IBL pre-pass shader builders
 */

#ifndef XNG_IBLPREPASS_SHADERS_HPP
#define XNG_IBLPREPASS_SHADERS_HPP

#include "xng/rendergraph/shaderscript/shaderscript.hpp"
#include "xng/rendergraph/rendergraphpipeline.hpp"

namespace xng {
    namespace iblprepass_shaders {
        RenderGraphPipeline makeBRDFPipeline();
        RenderGraphPipeline makeEquirectToCubePipeline();
        RenderGraphPipeline makeIrradiancePipeline();
        RenderGraphPipeline makePrefilterPipeline();
    }
}

#endif // XNG_IBLPREPASS_SHADERS_HPP
