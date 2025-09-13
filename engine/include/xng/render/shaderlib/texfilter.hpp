/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2023  Julian Zampiccoli
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

using namespace xng::ShaderScript;
DEFINE_FUNCTION2(textureBicubic)
DEFINE_FUNCTION3(textureBicubic)

namespace xng::shaderlib {
    /**
     * vec4 textureBicubic(sampler2D sampler, vec2 texCoords)
     * vec4 textureBicubic(sampler2DMS sampler, vec2 texCoords, int samples)
     * vec4 textureBicubic(sampler2DArray sampler, vec3 texCoords3, vec2 size)
     *
     * @return
     */
    XENGINE_EXPORT void textureBicubic();
}

#endif //XENGINE_TEXFILTER_HPP
