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

#ifndef XENGINE_NOISE_HPP
#define XENGINE_NOISE_HPP

#include "xng/rendergraph/shaderscript/shaderscript.hpp"

using namespace xng::ShaderScript;

DEFINE_FUNCTION1(simplex)

DEFINE_FUNCTION1(perlin)
DEFINE_FUNCTION2(perlin)
DEFINE_FUNCTION3(perlin)
DEFINE_FUNCTION1(perlinc)
DEFINE_FUNCTION2(perlinc)

DEFINE_FUNCTION1(noise)

namespace xng::shaderlib::noise {
    /**
     * float simplex(vec2 v)
     * float simplex(vec3 v)
     * float simplex(vec4 v)
     */
    XENGINE_EXPORT void simplex();

    /**
     * float perlin(vec2 p, float dim)
     * float perlin(vec2 p, float dim, float time)
     * float perlinbw(vec2 p, float dim)
     * float perlinc(vec2 p)
     * float perlinc(vec4 p)
     * float perlinc(vec4 p, vec4 rep)
     */
    XENGINE_EXPORT void perlin();

    /**
     * float noise(float p)
     * float noise(vec2 p)
     * float noise(vec3 p)
     */
    XENGINE_EXPORT void noise();
}

#endif //XENGINE_NOISE_HPP
