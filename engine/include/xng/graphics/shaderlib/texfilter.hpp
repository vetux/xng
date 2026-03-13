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

#ifndef XENGINE_TEXFILTER_HPP
#define XENGINE_TEXFILTER_HPP

#include "xng/rendergraph/shaderscript/shaderscript.hpp"

namespace xng::shaderlib::texfilter {
    using namespace xng::ShaderScript;

    XENGINE_EXPORT vec4 textureBicubic(Param<Texture2D<RGBA>> texture, Param<vec2> uv);

    XENGINE_EXPORT vec4 textureBicubicMS(Param<Texture2DMS<RGBA>> texture, Param<vec2> uv, Param<Int> samples);

    XENGINE_EXPORT vec4 textureBicubicArray(Param<Texture2DArray<RGBA>> texture, Param<vec3> uv, Param<vec2> size);
}

#endif //XENGINE_TEXFILTER_HPP
