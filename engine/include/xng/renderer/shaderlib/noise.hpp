/**
 *   xEngine - C++ Game Engine Library
 *   Copyright (C) 2026 Julia Zampiccoli
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the Lesser General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef XENGINE_NOISE_HPP
#define XENGINE_NOISE_HPP

#include "xng/shaderscript/shaderscript.hpp"

namespace xng::shaderlib::noise
{
    using namespace xng::ShaderScript;

    XENGINE_EXPORT Float simplex(Param<vec2> v);
    XENGINE_EXPORT Float simplex3(Param<vec3> v);
    XENGINE_EXPORT Float simplex4(Param<vec4> v);

    XENGINE_EXPORT Float perlin(Param<vec2> p, Param<Float> dim);
    XENGINE_EXPORT Float perlinT(Param<vec2> p, Param<Float> dim, Param<Float> time);
    XENGINE_EXPORT Float perlinbw(Param<vec2> p, Param<Float> dim);
    XENGINE_EXPORT Float perlinc(Param<vec2> p);
    XENGINE_EXPORT Float perlinc4(Param<vec4> p);
    XENGINE_EXPORT Float perlincr(Param<vec4> p, Param<vec4> rep);

    XENGINE_EXPORT Float noise(Param<Float> p);
    XENGINE_EXPORT Float noise2(Param<vec2> p);
    XENGINE_EXPORT Float noise3(Param<vec3> p);
}

#endif //XENGINE_NOISE_HPP
