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

#include "xng/renderer/passes/deferredpbrpass.hpp"

#include "xng/shaderscript/shaderscript.hpp"
#include "xng/shaderscript/macro/helpermacros.hpp"
#include "xng/shaderscript/macro/shaderstruct.hpp"

#include "xng/renderer/shaderlib/pbr.hpp"

using namespace xng::ShaderScript;
using namespace xng::shaderlib::pbr;

namespace xng {
    rg::Shader DeferredPBRPass::compileVertexShader() {
        BeginShader(rg::Shader::VERTEX);

        Input(vec3, position);
        Input(vec2, uv);

        Output(vec3, fPos);
        Output(vec2, fUv);

        Texture(rg::TEXTURE_2D, rg::RGBA32F, gPosition)
        Texture(rg::TEXTURE_2D, rg::RGBA32F, gNormal)
        Texture(rg::TEXTURE_2D, rg::RGBA32F, gTangent)
        Texture(rg::TEXTURE_2D, rg::RGBA8, gAlbedo)
        Texture(rg::TEXTURE_2D, rg::RGBA8, gRoughnessMetallicAO)
        Texture(rg::TEXTURE_2D, rg::RGBA8, gObjectIdReceiveShadows)

        fPos = position;
        fUv = uv;

        setVertexPosition(vec4(fPos.xyz(), 1));

        EndShader();

        return BuildShader();
    }

    rg::Shader DeferredPBRPass::compileFragmentShader() {
        BeginShader(rg::Shader::FRAGMENT);

        Input(vec3, fPos);
        Input(vec2, fUv);

        Output(vec4, oColor);

        Texture(rg::TEXTURE_2D, rg::RGBA32F, gPosition)
        Texture(rg::TEXTURE_2D, rg::RGBA32F, gNormal)
        Texture(rg::TEXTURE_2D, rg::RGBA32F, gTangent)
        Texture(rg::TEXTURE_2D, rg::RGBA8, gAlbedo)
        Texture(rg::TEXTURE_2D, rg::RGBA8, gRoughnessMetallicAO)
        Texture(rg::TEXTURE_2D, rg::RGBA8, gObjectIdReceiveShadows)

        StorageBufferDynamic(ShaderPointLight, pointLights)
        StorageBufferDynamic(ShaderDirectionalLight, directionalLights)
        StorageBufferDynamic(ShaderSpotLight, spotLights)

        Parameter(vec4, viewPosition)
        Parameter(Float, gamma)

        vec3 position = textureSample(gPosition, fUv).xyz();
        vec3 normal = textureSample(gNormal, fUv).xyz();
        vec3 albedo = textureSample(gAlbedo, fUv).xyz();
        vec3 roughnessMetallicAO = textureSample(gRoughnessMetallicAO, fUv).xyz();

        // sRGB conversion
        albedo = pow(albedo, vec3(2.2f));

        PbrPass pass;
        pass = pbr_begin(position,
                         normal,
                         albedo,
                         roughnessMetallicAO.y(),
                         roughnessMetallicAO.x(),
                         roughnessMetallicAO.z(),
                         viewPosition.xyz(),
                         gamma);

        vec3 reflectance;
        reflectance = vec3(0, 0, 0);

        For(Int, i, Int(0), i < pointLights.length(), i + 1)
            auto light = pointLights[i];
            reflectance = pbr_point(pass, reflectance, light.position.xyz(), light.color.xyz(), Float(1.0f));
        Done

        For(Int, i, Int(0), i < directionalLights.length(), i + 1)
            auto light = directionalLights[i];
            reflectance = pbr_directional(pass, reflectance, light.direction.xyz(), light.color.xyz(), Float(1.0f));
        Done

        For(Int, i, Int(0), i < spotLights.length(), i + 1)
            auto light = spotLights[i];
            reflectance = pbr_spot(pass,
                                   reflectance,
                                   light.position.xyz(),
                                   light.direction_quadratic.xyz(),
                                   light.direction_quadratic.w(),
                                   light.color.xyz(),
                                   light.cutOff_outerCutOff_constant_linear.x(),
                                   light.cutOff_outerCutOff_constant_linear.y(),
                                   light.cutOff_outerCutOff_constant_linear.z(),
                                   light.cutOff_outerCutOff_constant_linear.w(),
                                   Float(1.0f));
        Done

        If(Bool(false))
            pass.iblIrradiance = vec3(0);
            pass.iblPrefilter = vec3(0);
            pass.iblBRDF = vec2(0);
        Else
            // Sample IBL textures and store results in the PbrPass for pbr_finish
            /*vec3 irradiance = textureSampleCube(iblIrradiance, pass.N).xyz();
            vec3 R = reflect(pass.V * -1, pass.N);
            Float maxMip = Float(shaderData.iblPresent_prefilterMipCount.y() - 1);
            vec3 prefilteredColor = textureSampleCube(iblPrefilter, R, pass.roughness * maxMip).xyz();
            vec2 brdf = textureSample(iblBRDF, vec2(max(dot(pass.N, pass.V), 0.0f), pass.roughness)).xy();
            pass.iblIrradiance = irradiance;
            pass.iblPrefilter = prefilteredColor;
            pass.iblBRDF = brdf;*/
        Fi

        oColor = vec4(pbr_finish(pass, reflectance), 1);

        EndShader();

        return BuildShader();
    }
}
