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

#include "xng/rendergraph/shader/shaderfunction.hpp"

namespace xng::shaderlib::noise {
    /**
     * float simplex2D(vec2 v)
     *
     * @return
     */
    XENGINE_EXPORT ShaderFunction simplex2D();

    /**
     * float simplex3D(vec3 v)
     *
     * @return
     */
    XENGINE_EXPORT ShaderFunction simplex3D();

    /**
     * float simplex4D(vec4 v)
     *
     * @return
     */
    XENGINE_EXPORT ShaderFunction simplex4D();

    /**
     * float perlin(vec2 p, float dim)
     *
     * @return
     */
    XENGINE_EXPORT ShaderFunction perlin();

    /**
     * float perlinT(vec2 p, float dim, float time)
     *
     * @return
     */
    XENGINE_EXPORT ShaderFunction perlinT();

    /**
     * float perlinBW(vec2 p, float dim)
     *
     * @return
     */
    XENGINE_EXPORT ShaderFunction perlinBW();

    /**
     * float perlinC2D(vec2 p)
     *
     * @return
     */
    XENGINE_EXPORT ShaderFunction perlinC2D();

    /**
     * float perlinC3D(vec4 p)
     *
     * @return
     */
    XENGINE_EXPORT ShaderFunction perlinC3D();

    /**
     * float perlinPeriodic(vec4 p, vec4 rep)
     *
     * @return
     */
    XENGINE_EXPORT ShaderFunction perlinPeriodic();

    /**
     * float noise(float p)
     *
     * @return
     */
    XENGINE_EXPORT ShaderFunction noise();

    /**
     * float noise2D(vec2 p)
     *
     * @return
     */
    XENGINE_EXPORT ShaderFunction noise2D();

    /**
     * float noise3D(vec3 p)
     *
     * @return
     */
    XENGINE_EXPORT ShaderFunction noise3D();
}

#endif //XENGINE_NOISE_HPP
