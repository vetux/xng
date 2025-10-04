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

#include "xng/graphics/shaderlib/shadowmapping.hpp"

#include "xng/rendergraph/shaderscript/shaderscript.hpp"

using namespace xng::ShaderScript;

namespace xng::shaderlib::shadowmapping {
    void sampleShadowPoint() {
        Function("sampleShadowPoint", {
                     {"fragPos", ShaderDataType::vec3()},
                     {"lightPos", ShaderDataType::vec3()},
                     {"viewPos", ShaderDataType::vec3()},
                     {"depthMap", ShaderTexture(TEXTURE_CUBE_MAP_ARRAY, DEPTH)},
                     {"depthMapIndex", ShaderDataType::integer()},
                     {"far_plane", ShaderDataType::float32()}
                 },
                 ShaderDataType::float32());
        {
            ARGUMENT(fragPos)
            ARGUMENT(lightPos)
            ARGUMENT(viewPos)
            ARGUMENT(depthMap)
            ARGUMENT(depthMapIndex)
            ARGUMENT(far_plane)

            ArrayVec3<20> gridSamplingDisk;
            gridSamplingDisk = std::vector{
                vec3(1, 1, 1), vec3(1, -1, 1), vec3(-1, -1, 1), vec3(-1, 1, 1),
                vec3(1, 1, -1), vec3(1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
                vec3(1, 1, 0), vec3(1, -1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
                vec3(1, 0, 1), vec3(-1, 0, 1), vec3(1, 0, -1), vec3(-1, 0, -1),
                vec3(0, 1, 1), vec3(0, -1, 1), vec3(0, -1, -1), vec3(0, 1, -1)
            };

            vec3 fragToLight = fragPos - lightPos;

            Float currentDepth = length(fragToLight);

            Float shadow;
            shadow = Float(0.0f);

            //TODO: Configurable shadow bias and sample count
            Float bias = Float(0.15f);
            Int samples = 20;

            Float viewDistance = length(viewPos - fragPos);
            Float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;

            Int i;
            i = Int(0);
            For(i, 0, samples - 1, 1);
            {
                Float closestDepth = texture(depthMap,
                                             vec4(fragToLight + gridSamplingDisk[i] * diskRadius,
                                                  depthMapIndex)).x();
                closestDepth *= far_plane; // undo mapping [0;1]
                If(currentDepth - bias > closestDepth);
                {
                    shadow += 1.0;
                }
                EndIf();
            }
            EndFor();

            Float fSamples = 20.0f;
            shadow = shadow / fSamples;

            Return(1 - shadow);
        }
        EndFunction();
    }

    // TODO: Fix directional shadows
    void sampleShadowDirectional() {
        Function("sampleShadowDirectional", {
                     {"fragPosLightSpace", ShaderDataType::vec4()},
                     {"shadowMap", ShaderTexture(TEXTURE_2D_ARRAY, DEPTH)},
                     {"shadowMapIndex", ShaderDataType::integer()},
                     {"Normal", ShaderDataType::vec3()},
                     {"lightPos", ShaderDataType::vec3()},
                     {"fragPos", ShaderDataType::vec3()},
                 },
                 ShaderDataType::float32());
        {
            ARGUMENT(fragPosLightSpace)
            ARGUMENT(shadowMap)
            ARGUMENT(shadowMapIndex)
            ARGUMENT(Normal)
            ARGUMENT(lightPos)
            ARGUMENT(fragPos)

            // perform perspective divide
            vec3 projCoords = fragPosLightSpace.xyz() / fragPosLightSpace.w();
            // transform to [0,1] range
            projCoords = projCoords * 0.5f + 0.5f;

            // Check if position is outside projection
            If(projCoords.x() < 0
               || projCoords.x() > 1
               || projCoords.y() < 0
               || projCoords.y() > 1
               || projCoords.z() > 1);
            {
                Return(1.0f);
            }
            EndIf();

            // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
            Float closestDepth = texture(shadowMap, vec3(projCoords.xy(), shadowMapIndex)).x();
            // get depth of current fragment from light's perspective
            Float currentDepth = projCoords.z();

            // calculate bias (based on depth map resolution and slope)
            vec3 normal = normalize(Normal);
            vec3 lightDir = normalize(lightPos - fragPos);
            Float bias = max(0.05f * (1.0f - dot(normal, lightDir)), 0.005f);
            // check whether current frag pos is in shadow
            // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;

            // PCF
            Float shadow;
            shadow = Float(0.0f);
            vec2 texelSize = 1.0f / textureSize(shadowMap, 0).xy();
            Int x;
            x = Int(0);
            For(x, -1, 1, 1);
            {
                Int y;
                y = Int(0);
                For(y, -1, 1, 1);
                {
                    Float pcfDepth = texture(shadowMap, vec3(projCoords.xy() + vec2(x, y) * texelSize, shadowMapIndex)).
                            x();
                    If(currentDepth - bias > pcfDepth);
                    {
                        shadow += 1.0f;
                    }
                    EndIf();
                }
            }
            EndFor();

            shadow /= 9.0f;

            Return(1.0f - shadow);
        }
        EndFunction();
    }
}
