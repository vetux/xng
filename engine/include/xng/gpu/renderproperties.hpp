/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2023  Julian Zampiccoli
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

#ifndef XENGINE_RENDERPROPERTIES_HPP
#define XENGINE_RENDERPROPERTIES_HPP

namespace xng {
    enum DepthTestMode {
        DEPTH_TEST_ALWAYS,
        DEPTH_TEST_NEVER,
        DEPTH_TEST_LESS,
        DEPTH_TEST_EQUAL,
        DEPTH_TEST_LEQUAL,
        DEPTH_TEST_GREATER,
        DEPTH_TEST_NOTEQUAL,
        DEPTH_TEST_GEQUAL
    };

    enum StencilMode {
        STENCIL_NEVER,
        STENCIL_LESS,
        STENCIL_LEQUAL,
        STENCIL_GREATER,
        STENCIL_GEQUAL,
        STENCIL_EQUAL,
        STENCIL_NOTEQUAL,
        STENCIL_ALWAYS
    };

    enum StencilAction {
        STENCIL_KEEP,
        STENCIL_ZERO,
        STENCIL_REPLACE,
        STENCIL_INCR,
        STENCIL_INCR_WRAP,
        STENCIL_DECR,
        STENCIL_DECR_WRAP,
        STENCIL_INVERT
    };

    enum FaceCullingMode {
        CULL_NONE,
        CULL_FRONT,
        CULL_BACK
    };

    enum BlendMode {
        ZERO,
        ONE,
        SRC_COLOR,
        ONE_MINUS_SRC_COLOR,
        DST_COLOR,
        SRC_ALPHA,
        ONE_MINUS_SRC_ALPHA,
        DST_ALPHA,
        ONE_MINUS_DST_ALPHA,
        CONSTANT_COLOR,
        ONE_MINUS_CONSTANT_COLOR,
        CONSTANT_ALPHA,
        ONE_MINUS_CONSTANT_ALPHA
    };

    enum BlendEquation  {
        BLEND_ADD,
        BLEND_SUBTRACT,
        BLEND_REVERSE_SUBTRACT,
        BLEND_MIN,
        BLEND_MAX
    };
}

#endif //XENGINE_RENDERPROPERTIES_HPP
