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

#ifndef XENGINE_RENDERPIPELINEDESC_HPP
#define XENGINE_RENDERPIPELINEDESC_HPP

#include <functional>

#include "shaderprogram.hpp"
#include "renderproperties.hpp"
#include "xng/asset/color.hpp"

#include "xng/math/vector2.hpp"

namespace xng {
    struct RenderPipelineDesc {
        Vec2i viewportOffset = Vec2i(0);
        Vec2i viewportSize = Vec2i(1);

        bool multiSample = false;
        bool multiSampleEnableFrequency = false;
        float multiSampleFrequency = 1;

        ColorRGBA clearColorValue = ColorRGBA::black();
        float clearDepthValue = 1;

        bool clearColor = false;
        bool clearDepth = false;
        bool clearStencil = false;

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
        BlendMode blendSourceMode = BlendMode::SRC_ALPHA;
        BlendMode blendDestinationMode = BlendMode::ONE_MINUS_SRC_ALPHA;

        bool operator==(const RenderPipelineDesc &other) const {
            return viewportOffset == other.viewportOffset
                   && viewportSize == other.viewportSize
                   && multiSample == other.multiSample
                   && multiSampleEnableFrequency == other.multiSampleEnableFrequency
                   && clearColorValue == other.clearColorValue
                   && clearDepthValue == other.clearDepthValue
                   && clearColor == other.clearColor
                   && clearDepth == other.clearDepth
                   && clearStencil == other.clearStencil
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
                   && blendSourceMode == other.blendSourceMode
                   && blendDestinationMode == other.blendDestinationMode;
        }
    };
}

namespace std {
    template<>
    struct hash<xng::RenderPipelineDesc> {
        std::size_t operator()(const xng::RenderPipelineDesc &k) const {
            size_t ret = 0;
            xng::hash_combine(ret, k.viewportOffset.x);
            xng::hash_combine(ret, k.viewportOffset.y);
            xng::hash_combine(ret, k.viewportSize.x);
            xng::hash_combine(ret, k.viewportSize.y);
            xng::hash_combine(ret, k.multiSample);
            xng::hash_combine(ret, k.multiSampleEnableFrequency);
            xng::hash_combine(ret, k.multiSampleFrequency);
            xng::hash_combine(ret, k.clearColorValue.r());
            xng::hash_combine(ret, k.clearColorValue.g());
            xng::hash_combine(ret, k.clearColorValue.b());
            xng::hash_combine(ret, k.clearColorValue.a());
            xng::hash_combine(ret, k.clearDepthValue);
            xng::hash_combine(ret, k.clearColor);
            xng::hash_combine(ret, k.clearDepth);
            xng::hash_combine(ret, k.clearStencil);
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
            xng::hash_combine(ret, k.blendSourceMode);
            xng::hash_combine(ret, k.blendDestinationMode);
            return ret;
        }
    };
}

#endif //XENGINE_RENDERPIPELINEDESC_HPP
