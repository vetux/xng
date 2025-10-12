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

#include "xng/graphics/passes/shadowmappingpass.hpp"
#include "xng/rendergraph/shaderscript/shaderscript.hpp"

using namespace xng::ShaderScript;

namespace xng {
    DEFINE_FUNCTION1(getSkinnedVertexPosition)

    Shader ShadowMappingPass::createVertexShader() {
        BeginShader(Shader::VERTEX);

        Input(ShaderDataType::vec3(), vPosition);
        Input(ShaderDataType::vec3(), vNormal);
        Input(ShaderDataType::vec2(), vUv);
        Input(ShaderDataType::vec3(), vTangent);
        Input(ShaderDataType::vec3(), vBitangent);
        Input(ShaderDataType::ivec4(), boneIds);
        Input(ShaderDataType::vec4(), boneWeights);

        Output(ShaderDataType::vec4(), fPosition);

        Struct(PointLightData,
               {ShaderDataType::vec4(), "lightPosFarPlane"},
               {ShaderDataType::ivec4(), "layer"},
               {ShaderDataType::array(ShaderDataType::mat4(), 6), "shadowMatrices"});

        Struct(DrawData,
               {ShaderDataType::ivec4(), "boneOffset"},
               {ShaderDataType::mat4(), "model"});

        Struct(BoneData, {ShaderDataType::mat4(), "matrix"});

        Buffer(drawData, DrawData);
        DynamicBuffer(bones, BoneData);
        Buffer(lightData, PointLightData);

        Function("getSkinnedVertexPosition",
                 {ShaderFunction::Argument("offset", ShaderDataType::integer())},
                 ShaderDataType::vec4());
        {
            ARGUMENT(offset)

            If(offset < 0);
            {
                Return(vec4(vPosition, 1.0f));
            }
            EndIf();

            Int boneCount = bones.length();

            vec4 totalPosition;
            totalPosition = vec4(0, 0, 0, 0);

            If(boneIds.x() > -1);
            {
                If(boneIds.x() + offset >= boneCount);
                {
                    Return(vec4(vPosition, 1.0f));
                }
                Else();
                {
                    vec4 localPosition;
                    localPosition = bones[boneIds.x() + offset]["matrix"] * vec4(vPosition, 1.0f);
                    totalPosition += localPosition * boneWeights.x();
                }
                EndIf();
            }
            EndIf();

            If(boneIds.y() > -1);
            {
                If(boneIds.y() + offset >= boneCount);
                {
                    Return(vec4(vPosition, 1.0f));
                }
                Else();
                {
                    vec4 localPosition;
                    localPosition = bones[boneIds.y() + offset]["matrix"] * vec4(vPosition, 1.0f);
                    totalPosition += localPosition * boneWeights.y();
                }
                EndIf();
            }
            EndIf();

            If(boneIds.z() > -1);
            {
                If(boneIds.z() + offset >= boneCount);
                {
                    Return(vec4(vPosition, 1.0f));
                }
                Else();
                {
                    vec4 localPosition;
                    localPosition = bones[boneIds.z() + offset]["matrix"] * vec4(vPosition, 1.0f);
                    totalPosition += localPosition * boneWeights.z();
                }
                EndIf();
            }
            EndIf();

            If(boneIds.w() > -1);
            {
                If(boneIds.w() + offset >= boneCount);
                {
                    Return(vec4(vPosition, 1.0f));
                }
                Else();
                {
                    vec4 localPosition;
                    localPosition = bones[boneIds.w() + offset]["matrix"] * vec4(vPosition, 1.0f);
                    totalPosition += localPosition * boneWeights.w();
                }
                EndIf();
            }
            EndIf();

            Return(totalPosition);
        }
        EndFunction();

        fPosition = drawData["model"] * getSkinnedVertexPosition(drawData["boneOffset"].x());
        setVertexPosition(fPosition);

        return BuildShader();
    }

    Shader ShadowMappingPass::createGeometryShader() {
        BeginShader(Shader::GEOMETRY);

        InputPrimitive(TRIANGLES);
        OutputPrimitive(TRIANGLES, 18);

        Input(ShaderDataType::vec4(), fPosition);
        Output(ShaderDataType::vec4(), FragPos);

        Struct(PointLightData,
               {ShaderDataType::vec4(), "lightPosFarPlane"},
               {ShaderDataType::ivec4(), "layer"},
               {ShaderDataType::array(ShaderDataType::mat4(), 6), "shadowMatrices"});

        Struct(DrawData,
               {ShaderDataType::ivec4(), "boneOffset"},
               {ShaderDataType::mat4(), "model"});

        Struct(BoneData, {ShaderDataType::mat4(), "matrix"});

        Buffer(drawData, DrawData);
        DynamicBuffer(bones, BoneData);
        Buffer(lightData, PointLightData);

        Int face;
        face = Int(0);
        For(face, 0, 5, 1);
        {
            setLayer((lightData["layer"].x() * 6) + face);

            Int i;
            i = Int(0);
            For(i, 0, 2, 1);
            {
                FragPos = fPosition[i];
                setVertexPosition(lightData["shadowMatrices"][face] * FragPos);
                EmitVertex();
            }
            EndFor();

            EndPrimitive();
        }
        EndFor();

        return BuildShader();
    }

    Shader ShadowMappingPass::createFragmentShader() {
        BeginShader(Shader::FRAGMENT);

        Input(ShaderDataType::vec4(), FragPos);

        Struct(PointLightData,
               {ShaderDataType::vec4(), "lightPosFarPlane"},
               {ShaderDataType::ivec4(), "layer"},
               {ShaderDataType::array(ShaderDataType::mat4(), 6), "shadowMatrices"});

        Struct(DrawData,
               {ShaderDataType::ivec4(), "boneOffset"},
               {ShaderDataType::mat4(), "model"});

        Struct(BoneData, {ShaderDataType::mat4(), "matrix"});

        Buffer(drawData, DrawData);
        DynamicBuffer(bones, BoneData);
        Buffer(lightData, PointLightData);

        Float lightDistance = length(FragPos.xyz() - lightData["lightPosFarPlane"].xyz());

        // map to [0;1] range by dividing by far_plane
        lightDistance = lightDistance / lightData["lightPosFarPlane"].w();

        // write this as modified depth
        setFragmentDepth(lightDistance);

        return BuildShader();
    }

    Shader ShadowMappingPass::createDirVertexShader() {
        BeginShader(Shader::VERTEX);

        Input(ShaderDataType::vec3(), vPosition);
        Input(ShaderDataType::vec3(), vNormal);
        Input(ShaderDataType::vec2(), vUv);
        Input(ShaderDataType::vec3(), vTangent);
        Input(ShaderDataType::vec3(), vBitangent);
        Input(ShaderDataType::ivec4(), boneIds);
        Input(ShaderDataType::vec4(), boneWeights);

        Output(ShaderDataType::vec4(), fPosition);

        Struct(DirLightData,
               {ShaderDataType::ivec4(), "layer"},
               {ShaderDataType::mat4(), "shadowMatrix"});

        Struct(DrawData,
               {ShaderDataType::ivec4(), "boneOffset"},
               {ShaderDataType::mat4(), "model"});

        Struct(BoneData, {ShaderDataType::mat4(), "matrix"});

        Buffer(drawData, DrawData);
        DynamicBuffer(bones, BoneData);
        Buffer(lightData, DirLightData);

        Function("getSkinnedVertexPosition",
                 {ShaderFunction::Argument("offset", ShaderDataType::integer())},
                 ShaderDataType::vec4());
        {
            ARGUMENT(offset)

            If(offset < 0);
            {
                Return(vec4(vPosition, 1.0f));
            }
            EndIf();

            Int boneCount = bones.length();

            vec4 totalPosition;
            totalPosition = vec4(0, 0, 0, 0);

            If(boneIds.x() > -1);
            {
                If(boneIds.x() + offset >= boneCount);
                {
                    Return(vec4(vPosition, 1.0f));
                }
                Else();
                {
                    vec4 localPosition;
                    localPosition = bones[boneIds.x() + offset]["matrix"] * vec4(vPosition, 1.0f);
                    totalPosition += localPosition * boneWeights.x();
                }
                EndIf();
            }
            EndIf();

            If(boneIds.y() > -1);
            {
                If(boneIds.y() + offset >= boneCount);
                {
                    Return(vec4(vPosition, 1.0f));
                }
                Else();
                {
                    vec4 localPosition;
                    localPosition = bones[boneIds.y() + offset]["matrix"] * vec4(vPosition, 1.0f);
                    totalPosition += localPosition * boneWeights.y();
                }
                EndIf();
            }
            EndIf();

            If(boneIds.z() > -1);
            {
                If(boneIds.z() + offset >= boneCount);
                {
                    Return(vec4(vPosition, 1.0f));
                }
                Else();
                {
                    vec4 localPosition;
                    localPosition = bones[boneIds.z() + offset]["matrix"] * vec4(vPosition, 1.0f);
                    totalPosition += localPosition * boneWeights.z();
                }
                EndIf();
            }
            EndIf();

            If(boneIds.w() > -1);
            {
                If(boneIds.w() + offset >= boneCount);
                {
                    Return(vec4(vPosition, 1.0f));
                }
                Else();
                {
                    vec4 localPosition;
                    localPosition = bones[boneIds.w() + offset]["matrix"] * vec4(vPosition, 1.0f);
                    totalPosition += localPosition * boneWeights.w();
                }
                EndIf();
            }
            EndIf();

            Return(totalPosition);
        }
        EndFunction();

        fPosition = lightData["shadowMatrix"]
                    * drawData["model"]
                    * getSkinnedVertexPosition(drawData["boneOffset"].x());
        setVertexPosition(fPosition);

        return BuildShader();
    }

    Shader ShadowMappingPass::createDirGeometryShader() {
        BeginShader(Shader::GEOMETRY);

        InputPrimitive(TRIANGLES);
        OutputPrimitive(TRIANGLES, 3);

        Input(ShaderDataType::vec4(), fPosition);

        Output(ShaderDataType::vec4(), fragCoord);

        Struct(DirLightData,
               {ShaderDataType::ivec4(), "layer"},
               {ShaderDataType::mat4(), "shadowMatrix"});

        Struct(DrawData,
               {ShaderDataType::ivec4(), "boneOffset"},
               {ShaderDataType::mat4(), "model"});

        Struct(BoneData, {ShaderDataType::mat4(), "matrix"});

        Buffer(drawData, DrawData);
        DynamicBuffer(bones, BoneData);
        Buffer(lightData, DirLightData);

        setLayer(lightData["layer"].x());

        Int i;
        i = Int(0);
        For(i, 0, 2, 1);
        {
            fragCoord = fPosition[i];
            setVertexPosition(fragCoord);
            EmitVertex();
        }
        EndFor();

        EndPrimitive();

        return BuildShader();
    }

    Shader ShadowMappingPass::createDirFragmentShader() {
        BeginShader(Shader::FRAGMENT);

        Input(ShaderDataType::vec4(), fragCoord);

        Struct(DirLightData,
               {ShaderDataType::ivec4(), "layer"},
               {ShaderDataType::mat4(), "shadowMatrix"});

        Struct(DrawData,
               {ShaderDataType::ivec4(), "boneOffset"},
               {ShaderDataType::mat4(), "model"});

        Struct(BoneData, {ShaderDataType::mat4(), "matrix"});

        Buffer(drawData, DrawData);
        DynamicBuffer(bones, BoneData);
        Buffer(lightData, DirLightData);

        return BuildShader();
    }
}
