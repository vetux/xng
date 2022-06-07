/**
 *  xEngine - C++ game engine library
 *  Copyright (C) 2021  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef XENGINE_RENDERPIPELINEDESC_HPP
#define XENGINE_RENDERPIPELINEDESC_HPP

#include <functional>

#include "graphics/shaderprogram.hpp"

#include "graphics/renderproperties.hpp"

namespace xengine {
    struct RenderPipelineDesc {
        ShaderProgram *shader = nullptr;

        Vec2i viewportOffset;
        Vec2i viewportSize;

        bool multiSample = true;
        bool multiSampleEnableFrequency = false;
        float multiSampleFrequency = 1;

        ColorRGBA clearColorValue = ColorRGBA::black();
        float clearDepthValue = 1;

        bool clearColor = true;
        bool clearDepth = true;
        bool clearStencil = true;

        bool enableDepthTest = true;
        bool depthTestWrite = true;
        DepthTestMode depthTestMode = DepthTestMode::DEPTH_TEST_LESS;

        bool enableStencilTest = false;
        uint stencilTestMask = 0xFF;
        StencilMode stencilMode = StencilMode::STENCIL_ALWAYS;
        int stencilReference = 1;
        uint stencilFunctionMask = 0xFF;
        StencilAction stencilFail = StencilAction::STENCIL_KEEP;
        StencilAction stencilDepthFail = StencilAction::STENCIL_KEEP;
        StencilAction stencilPass = StencilAction::STENCIL_KEEP;

        bool enableFaceCulling = false;
        FaceCullingMode faceCullMode = FaceCullingMode::CULL_BACK;
        bool faceCullClockwiseWinding = false;

        bool enableBlending = false;
        BlendMode blendSourceMode = BlendMode::SRC_ALPHA;
        BlendMode blendDestinationMode = BlendMode::ONE_MINUS_SRC_ALPHA;
    };
}

using namespace xengine;
namespace std {
    template<>
    struct hash<RenderPipelineDesc> {
        std::size_t operator()(const RenderPipelineDesc &k) const {
            size_t ret = 0;
            hash_combine(ret, reinterpret_cast<uintptr_t> (k.shader));
            hash_combine(ret, k.viewportOffset.x);
            hash_combine(ret, k.viewportOffset.y);
            hash_combine(ret, k.viewportSize.x);
            hash_combine(ret, k.viewportSize.y);
            hash_combine(ret, k.multiSample);
            hash_combine(ret, k.multiSampleEnableFrequency);
            hash_combine(ret, k.multiSampleFrequency);
            hash_combine(ret, k.clearColorValue.r());
            hash_combine(ret, k.clearColorValue.g());
            hash_combine(ret, k.clearColorValue.b());
            hash_combine(ret, k.clearColorValue.a());
            hash_combine(ret, k.clearDepthValue);
            hash_combine(ret, k.clearColor);
            hash_combine(ret, k.clearDepth);
            hash_combine(ret, k.clearStencil);
            hash_combine(ret, k.enableDepthTest);
            hash_combine(ret, k.depthTestWrite);
            hash_combine(ret, k.depthTestMode);
            hash_combine(ret, k.enableStencilTest);
            hash_combine(ret, k.stencilTestMask);
            hash_combine(ret, k.stencilMode);
            hash_combine(ret, k.stencilReference);
            hash_combine(ret, k.stencilFunctionMask);
            hash_combine(ret, k.stencilFail);
            hash_combine(ret, k.stencilDepthFail);
            hash_combine(ret, k.stencilPass);
            hash_combine(ret, k.enableFaceCulling);
            hash_combine(ret, k.faceCullMode);
            hash_combine(ret, k.faceCullClockwiseWinding);
            hash_combine(ret, k.enableBlending);
            hash_combine(ret, k.blendSourceMode);
            hash_combine(ret, k.blendDestinationMode);
            return ret;
        }
    };
}

#endif //XENGINE_RENDERPIPELINEDESC_HPP
