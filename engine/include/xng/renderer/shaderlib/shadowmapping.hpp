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

#ifndef XENGINE_SHADOWMAPPING_HPP
#define XENGINE_SHADOWMAPPING_HPP

#include "xng/shaderscript/shaderscript.hpp"

namespace xng::shaderlib::shadowmapping
{
    using namespace xng::ShaderScript;

    XENGINE_EXPORT Float sampleShadowPoint(Param<vec3> fragPos,
                                           Param<vec3> lightPos,
                                           Param<vec3> viewPos,
                                           Param<TextureCubeArray<rg::DEPTH>> depthMap,
                                           Param<Int> depthMapIndex,
                                           Param<Float> far_plane);

    XENGINE_EXPORT Float sampleShadowDirectional(Param<vec4> fragPosLightSpace,
                                                 Param<Texture2DArray<rg::DEPTH>> shadowMap,
                                                 Param<Int> shadowMapIndex,
                                                 Param<vec3> Normal,
                                                 Param<vec3> lightPos,
                                                 Param<vec3> fragPos);
}

#endif //XENGINE_SHADOWMAPPING_HPP
