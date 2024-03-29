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

#ifndef XENGINE_RENDERPIPELINEDESC_HPP
#define XENGINE_RENDERPIPELINEDESC_HPP

#include <functional>
#include <variant>

#include "xng/render/scene/color.hpp"

#include "xng/math/vector2.hpp"

#include "xng/gpu/texturebuffer.hpp"
#include "xng/gpu/shaderuniformbuffer.hpp"
#include "xng/gpu/renderproperties.hpp"
#include "xng/gpu/vertexlayout.hpp"

#include "xng/shader/shaderstage.hpp"
#include "xng/shader/spirvshader.hpp"

#include "xng/util/crc.hpp"

#include "xng/render/geometry/primitive.hpp"

namespace xng {
    struct RenderPipelineDesc {
        std::map<ShaderStage, SPIRVShader> shaders; // The shaders to use for this pipeline
        std::vector<RenderPipelineBindingType> bindings; // The set of binding types defining how shader resources are bound

        Primitive primitive = TRIANGLES;
        VertexLayout vertexLayout{}; // The layout of one vertex
        VertexLayout instanceArrayLayout{}; // Layout of the instanced array (Optional)

        bool multiSample = false;
        bool multiSampleEnableFrequency = false;
        float multiSampleFrequency = 1;

        bool enableDepthTest = false;
        bool depthTestWrite = false;
        DepthTestMode depthTestMode = DepthTestMode::DEPTH_TEST_LESS;

        bool enableStencilTest = false;
        unsigned int stencilTestMask = 0xFF;
        StencilMode stencilMode = StencilMode::STENCIL_ALWAYS;
        int stencilReference = 1;
        unsigned int stencilFunctionMask = 0xFF;
        StencilAction stencilFail = StencilAction::STENCIL_KEEP;
        StencilAction stencilDepthFail = StencilAction::STENCIL_KEEP;
        StencilAction stencilPass = StencilAction::STENCIL_KEEP;

        bool enableFaceCulling = false;
        FaceCullingMode faceCullMode = FaceCullingMode::CULL_BACK;
        bool faceCullClockwiseWinding = false;

        bool enableBlending = false;
        BlendMode colorBlendSourceMode = BlendMode::SRC_ALPHA;
        BlendMode colorBlendDestinationMode = BlendMode::ONE_MINUS_SRC_ALPHA;
        BlendMode alphaBlendSourceMode = BlendMode::SRC_ALPHA;
        BlendMode alphaBlendDestinationMode = BlendMode::ONE_MINUS_SRC_ALPHA;
        BlendEquation colorBlendEquation = BlendEquation::BLEND_ADD;
        BlendEquation alphaBlendEquation = BlendEquation::BLEND_ADD;

        bool operator==(const RenderPipelineDesc &other) const {
            return bindings == other.bindings
                   && shaders == other.shaders
                   && multiSample == other.multiSample
                   && multiSampleEnableFrequency == other.multiSampleEnableFrequency
                   && enableDepthTest == other.enableDepthTest
                   && depthTestWrite == other.depthTestWrite
                   && depthTestMode == other.depthTestMode
                   && enableStencilTest == other.enableStencilTest
                   && stencilTestMask == other.stencilTestMask
                   && stencilMode == other.stencilMode
                   && stencilReference == other.stencilReference
                   && stencilFunctionMask == other.stencilFunctionMask
                   && stencilFail == other.stencilFail
                   && stencilDepthFail == other.stencilDepthFail
                   && stencilPass == other.stencilPass
                   && enableFaceCulling == other.enableFaceCulling
                   && faceCullMode == other.faceCullMode
                   && faceCullClockwiseWinding == other.faceCullClockwiseWinding
                   && enableBlending == other.enableBlending
                   && colorBlendSourceMode == other.colorBlendSourceMode
                   && colorBlendDestinationMode == other.colorBlendDestinationMode
                   && alphaBlendSourceMode == other.alphaBlendSourceMode
                   && alphaBlendDestinationMode == other.alphaBlendDestinationMode
                   && colorBlendEquation == other.colorBlendEquation
                   && alphaBlendEquation == other.alphaBlendEquation;
        }
    };
}

namespace std {
    template<>
    struct hash<xng::RenderPipelineDesc> {
        std::size_t operator()(const xng::RenderPipelineDesc &k) const {
            size_t ret = 0;
            for (auto &b: k.bindings) {
                xng::hash_combine(ret, b);
            }
            for (auto &pair: k.shaders) {
                xng::hash_combine(ret, pair.first);
                xng::hash_combine(ret, pair.second.getStage());
                xng::hash_combine(ret, pair.second.getEntryPoint());
                xng::hash_combine(ret, xng::crc(pair.second.getBlob().begin(), pair.second.getBlob().end()));
            }

            xng::hash_combine(ret, k.multiSample);
            xng::hash_combine(ret, k.multiSampleEnableFrequency);
            xng::hash_combine(ret, k.multiSampleFrequency);
            xng::hash_combine(ret, k.enableDepthTest);
            xng::hash_combine(ret, k.depthTestWrite);
            xng::hash_combine(ret, k.depthTestMode);
            xng::hash_combine(ret, k.enableStencilTest);
            xng::hash_combine(ret, k.stencilTestMask);
            xng::hash_combine(ret, k.stencilMode);
            xng::hash_combine(ret, k.stencilReference);
            xng::hash_combine(ret, k.stencilFunctionMask);
            xng::hash_combine(ret, k.stencilFail);
            xng::hash_combine(ret, k.stencilDepthFail);
            xng::hash_combine(ret, k.stencilPass);
            xng::hash_combine(ret, k.enableFaceCulling);
            xng::hash_combine(ret, k.faceCullMode);
            xng::hash_combine(ret, k.faceCullClockwiseWinding);
            xng::hash_combine(ret, k.enableBlending);
            xng::hash_combine(ret, k.colorBlendSourceMode);
            xng::hash_combine(ret, k.colorBlendDestinationMode);
            xng::hash_combine(ret, k.colorBlendEquation);
            xng::hash_combine(ret, k.alphaBlendEquation);
            return ret;
        }
    };
}

#endif //XENGINE_RENDERPIPELINEDESC_HPP
