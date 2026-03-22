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

#include "xng/graphics/passes/iblprepass.hpp"
#include "xng/graphics/shaderlib/pi.hpp"

#include "xng/rendergraph/shaderscript/shaderscript.hpp"
#include "../../../include/xng/rendergraph/resource/rendergraphpipeline.hpp"
#include "xng/rendergraph/shaderscript/macro/helpermacros.hpp"

using namespace xng::ShaderScript;

namespace xng
{
    // Van der Corput radical inverse for Hammersley sequence
    Float radicalInverseVdC(Param<Int> bits)
    {
        IRFunction

        Float inv;
        inv = Float(0.0f);
        Float denom;
        denom = Float(1.0f);
        Float nnf;
        nnf = Float(bits);
        For(Int, r, Int(0), r < Int(32), r + 1)
            denom = denom * Float(2.0f);
            Float bitf = mod(nnf, Float(2.0f));
            inv = inv + bitf / denom;
            nnf = floor(nnf / Float(2.0f));
            If(nnf <= Float(0.0f))
                IRReturn(inv);
            Fi
        Done
        IRReturn(inv);

        IRFunctionEnd
    }

    vec2 integrateBRDF(Param<Float> NdotV, Param<Float> roughness)
    {
        IRFunction

        Int SAMPLE_COUNT;
        SAMPLE_COUNT = Int(1024);

        Float A;
        A = Float(0.0f);
        Float B;
        B = Float(0.0f);
        vec3 V;
        V = vec3(sqrt(Float(1.0f) - NdotV * NdotV), Float(0.0f), NdotV);

        For(Int, i, Int(0), i < SAMPLE_COUNT, i + 1)
            // Hammersley sequence for better distribution
            Float Xi1 = (Float(i) + Float(0.5f)) / Float(SAMPLE_COUNT);
            Float Xi2 = radicalInverseVdC(i);

            Float a = roughness * roughness;
            Float phi = Float(2.0f) * Float(pi()) * Xi1;
            Float cosTheta = sqrt((Float(1.0f) - Xi2) / (Float(1.0f) + (a * a - Float(1.0f)) * Xi2));
            Float sinTheta = sqrt(Float(1.0f) - cosTheta * cosTheta);

            vec3 H = vec3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);
            vec3 L = normalize(Float(2.0f) * dot(V, H) * H - V);

            Float NdotL = max(L.z(), Float(0.0f));
            Float NdotH = max(H.z(), Float(0.0f));
            Float VdotH = max(dot(V, H), Float(0.0f));

            If(NdotL > Float(0.0f))
                // IBL-specific geometry (k = roughness^2 / 2)
                Float k = (roughness * roughness) / Float(2.0f);
                Float gV = NdotV / (NdotV * (Float(1.0f) - k) + k);
                Float gL = NdotL / (NdotL * (Float(1.0f) - k) + k);
                Float G = gV * gL;
                Float G_Vis = (G * VdotH) / (NdotH * NdotV + Float(0.0001f));
                Float Fc = pow(Float(1.0f) - VdotH, Float(5.0f));
                A = A + (Float(1.0f) - Fc) * G_Vis;
                B = B + Fc * G_Vis;
            Fi
        Done

        A = A / Float(SAMPLE_COUNT);
        B = B / Float(SAMPLE_COUNT);
        IRReturn(vec2(A, B));

        IRFunctionEnd
    }

    RenderGraphPipeline IBLPrePass::makeBRDFPipeline()
    {
        RenderGraphPipeline pip;

        // Vertex shader for fullscreen quad
        Shader vs;
        {
            BeginShader(Shader::VERTEX)
            Input(vec3, position)
            Input(vec2, uv)
            Output(vec2, vUv)
            setVertexPosition(vec4(position, 1.0f));
            vUv = uv;
            EndShader();
            vs = BuildShader();
        }

        // Fragment shader: integrate BRDF
        Shader fs;
        {
            BeginShader(Shader::FRAGMENT)
            Input(vec2, vUv)
            Output(vec2, outColor)

            Float NdotV = max(vUv.x(), Float(0.001f));
            Float roughness = max(vUv.y(), Float(0.001f));
            vec2 res = integrateBRDF(NdotV, roughness);
            outColor = res;

            EndShader();

            fs = BuildShader();
        }

        pip.shaders = {vs, fs};
        pip.enableDepthTest = false;
        pip.enableFaceCulling = false;

        return pip;
    }

    RenderGraphPipeline IBLPrePass::makeEquirectToCubePipeline()
    {
        RenderGraphPipeline eqPip;

        Shader eqVs;
        {
            BeginShader(Shader::VERTEX)
            Input(vec3, position)
            Input(vec3, normal)
            Input(vec2, uv)
            Output(vec3, vPosition)
            Buffer(CaptureFaceData, captureData)
            vPosition = position;
            setVertexPosition(captureData.captureMatrix * vec4(position, 1.0f));
            EndShader();
            eqVs = BuildShader();
        }

        Shader eqFs;
        {
            BeginShader(Shader::FRAGMENT)
            Input(vec3, vPosition)
            Output(vec4, outColor)
            Texture(TEXTURE_2D, RGB32F, envEquirect)

            vec3 dir = normalize(vPosition);
            Float u = atan2(dir.z(), dir.x()) / (Float(2.0f) * Float(pi())) + Float(0.5f);
            Float v = Float(0.5f) - asin(clamp(dir.y(), Float(-1.0f), Float(1.0f))) / Float(pi());
            vec4 samp = textureSample(envEquirect, vec2(u, v));
            outColor = vec4(samp.xyz(), Float(1.0f));
            EndShader();
            eqFs = BuildShader();
        }

        eqPip.shaders = {eqVs, eqFs};
        eqPip.enableDepthTest = false;
        eqPip.enableFaceCulling = false;

        return eqPip;
    }

    RenderGraphPipeline IBLPrePass::makeIrradiancePipeline()
    {
        RenderGraphPipeline irrPip;

        Shader irrVs;
        {
            BeginShader(Shader::VERTEX)
            Input(vec3, position)
            Input(vec3, normal)
            Input(vec2, uv)
            Output(vec3, vPosition)
            Buffer(CaptureFaceData, captureData)
            vPosition = position;
            setVertexPosition(captureData.captureMatrix * vec4(position, 1.0f));
            EndShader();
            irrVs = BuildShader();
        }

        Shader irrFs;
        {
            BeginShader(Shader::FRAGMENT)
            Input(vec3, vPosition)
            Output(vec4, outColor)
            Texture(TEXTURE_CUBE_MAP, RGBA16F, envCube)

            Buffer(PrefilterParams, prefilterParams)

            vec3 N = normalize(vPosition);

            // Build tangent frame
            vec3 up;
            up = vec3(0.0f, 1.0f, 0.0f);
            If(abs(N.y()) > Float(0.999f))
                up = vec3(1.0f, 0.0f, 0.0f);
            Fi
            vec3 right = normalize(cross(up, N));
            up = cross(N, right);

            // Use Monte Carlo integration with better sampling
            Int SAMPLE_COUNT;
            SAMPLE_COUNT = Int(2048);
            vec3 irradiance;
            irradiance = vec3(0.0f);

            // Environment cubemap resolution for LOD computation
            Float resolution = prefilterParams.prefilterParams.y();
            Float saTexel = Float(4.0f) * Float(pi()) / (Float(6.0f) * resolution * resolution);

            For(Int, i, Int(0), i < SAMPLE_COUNT, i + 1)
                // Hammersley sequence for uniform hemisphere sampling
                Float Xi1 = (Float(i) + Float(0.5f)) / Float(SAMPLE_COUNT);
                Float Xi2 = radicalInverseVdC(i);

                // Cosine-weighted hemisphere sampling
                Float phi = Float(2.0f) * Float(pi()) * Xi1;
                Float cosTheta = sqrt(Xi2);
                Float sinTheta = sqrt(Float(1.0f) - Xi2);

                vec3 tangentSample = vec3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);
                vec3 sampleVec = tangentSample.x() * right + tangentSample.y() * up + tangentSample.z() * N;

                // Compute per-sample LOD based on cosine-weighted PDF to avoid fireflies
                Float pdf = cosTheta / Float(pi());
                Float saSample = Float(1.0f) / (Float(SAMPLE_COUNT) * pdf + Float(1e-6f));
                Float lod = max(Float(0.5f) * log2(saSample / saTexel), Float(0.0f));

                irradiance = irradiance + textureSampleCube(envCube, sampleVec, lod).xyz();
            Done

            // Average samples; cosine-weighted sampling yields E/pi directly
            irradiance = irradiance / Float(SAMPLE_COUNT);
            outColor = vec4(irradiance, Float(1.0f));
            EndShader();
            irrFs = BuildShader();
        }

        irrPip.shaders = {irrVs, irrFs};
        irrPip.enableDepthTest = false;
        irrPip.enableFaceCulling = false;

        return irrPip;
    }

    RenderGraphPipeline IBLPrePass::makePrefilterPipeline()
    {
        RenderGraphPipeline prePip;

        Shader preVs;
        {
            BeginShader(Shader::VERTEX)
            Input(vec3, position)
            Input(vec3, normal)
            Input(vec2, uv)
            Output(vec3, vPosition)
            Buffer(CaptureFaceData, captureData)
            vPosition = position;
            setVertexPosition(captureData.captureMatrix * vec4(position, 1.0f));
            EndShader();
            preVs = BuildShader();
        }

        Shader preFs;
        {
            BeginShader(Shader::FRAGMENT)
            Input(vec3, vPosition)
            Output(vec4, outColor)
            Texture(TEXTURE_CUBE_MAP, RGBA16F, envCube)

            Buffer(PrefilterParams, prefilterParams)

            vec3 N = normalize(vPosition);
            vec3 R = N;
            vec3 V = R;

            Float roughness = prefilterParams.prefilterParams.x();
            Int SAMPLE_COUNT;
            SAMPLE_COUNT = Int(1024);

            // Build tangent frame for N
            vec3 up;
            up = vec3(0.0f, 1.0f, 0.0f);
            If(abs(N.y()) > Float(0.999f))
                up = vec3(1.0f, 0.0f, 0.0f);
            Fi
            vec3 tangent = normalize(cross(up, N));
            vec3 bitangent = cross(N, tangent);

            vec3 prefilteredColor;
            prefilteredColor = vec3(0.0f);
            Float totalWeight;
            totalWeight = Float(0.0f);

            For(Int, i, Int(0), i < SAMPLE_COUNT, i + 1)
                // Hammersley sequence for importance sampling
                Float Xi1 = (Float(i) + Float(0.5f)) / Float(SAMPLE_COUNT);
                Float Xi2 = radicalInverseVdC(i);

                // GGX importance sampling
                Float a = roughness * roughness;
                Float phi = Float(2.0f) * Float(pi()) * Xi1;
                Float cosTheta = sqrt((Float(1.0f) - Xi2) / (Float(1.0f) + (a * a - Float(1.0f)) * Xi2));
                Float sinTheta = sqrt(Float(1.0f) - cosTheta * cosTheta);

                // Tangent space half vector
                vec3 H_tangent = vec3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);

                // Transform to world space
                vec3 H = normalize(tangent * H_tangent.x() + bitangent * H_tangent.y() + N * H_tangent.z());
                vec3 L = normalize(Float(2.0f) * dot(V, H) * H - V);

                Float NdotL = max(dot(N, L), Float(0.0f));
                If(NdotL > Float(0.0f))
                    // compute PDF and mip level similarly to learnopengl implementation
                    Float NdotH = max(dot(N, H), Float(0.0f));
                    Float HdotV = max(dot(H, V), Float(0.0f));
                    Float a2 = a * a;
                    Float NdotH2 = NdotH * NdotH;
                    Float nom = a2;
                    Float denom;
                    denom = (NdotH2 * (a2 - Float(1.0f)) + Float(1.0f));
                    denom = Float(pi()) * denom * denom;
                    Float D = nom / denom;
                    Float pdf = D * NdotH / (Float(4.0f) * (HdotV + Float(1e-6f))) + Float(1e-6f);

                    // read the base prefilter resolution passed from CPU side
                    Float resolution = prefilterParams.prefilterParams.y();
                    Float saTexel = Float(4.0f) * Float(pi()) / (Float(6.0f) * resolution * resolution);
                    Float saSample = Float(1.0f) / (Float(SAMPLE_COUNT) * pdf + Float(1e-6f));

                    Float mipLevel;
                    If(roughness == Float(0.0f))
                        mipLevel = Float(0.0f);
                    Else
                        mipLevel = Float(0.5f) * log2(saSample / saTexel);
                    Fi

                    prefilteredColor = prefilteredColor + textureSampleCube(envCube, L, mipLevel).xyz() * NdotL;
                    totalWeight = totalWeight + NdotL;
                Fi
            Done

            If(totalWeight > Float(0.0f))
                prefilteredColor = prefilteredColor / totalWeight;
            Else
                prefilteredColor = textureSampleCube(envCube, N, Float(0.0f)).xyz();
            Fi

            outColor = vec4(prefilteredColor, Float(1.0f));
            EndShader();
            preFs = BuildShader();
        }

        prePip.shaders = {preVs, preFs};
        prePip.enableDepthTest = false;
        prePip.enableFaceCulling = false;

        return prePip;
    }
}
