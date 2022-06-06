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
#endif //XENGINE_RENDERPIPELINEDESC_HPP
