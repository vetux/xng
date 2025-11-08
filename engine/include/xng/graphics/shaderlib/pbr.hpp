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

#ifndef XENGINE_PBR_HPP
#define XENGINE_PBR_HPP

#include "xng/rendergraph/shaderscript/shaderscript.hpp"

namespace xng::ShaderScript {
    DEFINE_FUNCTION8(pbr_begin)
    DEFINE_FUNCTION5(pbr_point)
    DEFINE_FUNCTION5(pbr_directional)
    DEFINE_FUNCTION11(pbr_spot)
    DEFINE_FUNCTION2(pbr_finish)

    DefineStruct(PbrPass,
                 vec3, N,
                 vec3, V,
                 vec3, F0,
                 vec3, WorldPos,
                 vec3, Normal,
                 vec3, albedo,
                 Float, metallic,
                 Float, roughness,
                 Float, ao,
                 vec3, camPos,
                 Float, gamma);
}

namespace xng::shaderlib {
    /**
     * Defines the following functions:
     *
     * PbrPass pbr_begin(vec3 WorldPos, vec3 Normal, vec3 albedo, float metallic, float roughness, float ao, vec3 camPos);
     * vec3 pbr_point(PbrPass pass, vec3 Lo, vec3 position, vec3 color, float shadow);
     * vec3 pbr_directional(PbrPass pass, vec3 Lo, vec3 direction, vec3 color, float shadow);
     * vec3 pbr_spot(PbrPass pass, vec3 Lo, vec3 position, vec3 direction, float quadratic, vec3 color, float cutOff, float outerCutOff, float constant, float linear, float shadow);
     * vec3 pbr_finish(PbrPass pass, vec3 Lo);
     */
    XENGINE_EXPORT void pbr();
}

#endif //XENGINE_PBR_HPP
