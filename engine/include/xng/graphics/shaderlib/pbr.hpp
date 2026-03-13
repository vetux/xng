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
#include "xng/rendergraph/shaderscript/macro/shaderstruct.hpp"

namespace xng::shaderlib::pbr
{
    using namespace xng::ShaderScript;

    ShaderStruct(PbrPass,
                 Vec3f, N,
                 Vec3f, V,
                 Vec3f, F0,
                 Vec3f, WorldPos,
                 Vec3f, Normal,
                 Vec3f, albedo,
                 float, metallic,
                 float, roughness,
                 float, ao,
                 Vec3f, camPos,
                 float, gamma,
                 Vec3f, iblIrradiance,
                 Vec3f, iblPrefilter,
                 Vec2f, iblBRDF);

    XENGINE_EXPORT PbrPass pbr_begin(Param<vec3> WorldPos,
                                     Param<vec3> Normal,
                                     Param<vec3> albedo,
                                     Param<Float> metallic,
                                     Param<Float> roughness,
                                     Param<Float> ao,
                                     Param<vec3> camPos,
                                     Param<Float> gamma);

    XENGINE_EXPORT vec3 pbr_point(Param<PbrPass> pass, Param<vec3> Lo, Param<vec3> position, Param<vec3> color,
                                  Param<Float> shadow);

    XENGINE_EXPORT vec3 pbr_directional(Param<PbrPass> pass, Param<vec3> Lo, Param<vec3> direction, Param<vec3> color,
                                        Param<Float> shadow);

    XENGINE_EXPORT vec3 pbr_spot(Param<PbrPass> pass,
                                 Param<vec3> Lo,
                                 Param<vec3> position,
                                 Param<vec3> direction,
                                 Param<Float> quadratic,
                                 Param<vec3> color,
                                 Param<Float> cutOff,
                                 Param<Float> outerCutOff,
                                 Param<Float> constant,
                                 Param<Float> linear,
                                 Param<Float> shadow);

    XENGINE_EXPORT vec3 pbr_finish(Param<PbrPass> pass, Param<vec3> Lo);
}

#endif //XENGINE_PBR_HPP
