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

    DefineStruct(PointLightData,
                 vec4, lightPosFarPlane,
                 ivec4, layer,
                 ArrayMat4<6>, shadowMatrices)

    DefineStruct(DirLightData,
                 ivec4, layer,
                 mat4, shadowMatrix)

    DefineStruct(DrawData,
                 ivec4, boneOffset,
                 mat4, model)

    DefineStruct(BoneData, mat4, matrix)

    Shader ShadowMappingPass::createVertexShader() {
        BeginShader(Shader::VERTEX)

        Input(vec3, vPosition)
        Input(vec3, vNormal)
        Input(vec2, vUv)
        Input(vec3, vTangent)
        Input(vec3, vBitangent)
        Input(ivec4, boneIds)
        Input(vec4, boneWeights)

        Output(vec4, fPosition)

        DeclareStruct(PointLightData)
        DeclareStruct(DirLightData)
        DeclareStruct(DrawData)
        DeclareStruct(BoneData)

        Buffer(DrawData, drawData)
        DynamicBuffer(BoneData, bones)
        Buffer(PointLightData, lightData)

        Function("getSkinnedVertexPosition",
                 {ShaderFunction::Argument(ShaderDataType::Int(), "offset")},
                 ShaderDataType::vec4());
        {
            ARGUMENT(Int, offset)

            If(offset < 0)
                Return(vec4(vPosition, 1.0f));
            Fi

            Int boneCount = bones.length();

            vec4 totalPosition;
            totalPosition = vec4(0, 0, 0, 0);

            If(boneIds.x() > -1)
                If(boneIds.x() + offset >= boneCount)
                    Return(vec4(vPosition, 1.0f));
                Else
                    vec4 localPosition;
                    localPosition = bones[boneIds.x() + offset].matrix * vec4(vPosition, 1.0f);
                    totalPosition += localPosition * boneWeights.x();
                Fi
            Fi

            If(boneIds.y() > -1)
                If(boneIds.y() + offset >= boneCount)
                    Return(vec4(vPosition, 1.0f));
                Else
                    vec4 localPosition;
                    localPosition = bones[boneIds.y() + offset].matrix * vec4(vPosition, 1.0f);
                    totalPosition += localPosition * boneWeights.y();
                Fi
            Fi

            If(boneIds.z() > -1)
                If(boneIds.z() + offset >= boneCount)
                    Return(vec4(vPosition, 1.0f));
                Else
                    vec4 localPosition;
                    localPosition = bones[boneIds.z() + offset].matrix * vec4(vPosition, 1.0f);
                    totalPosition += localPosition * boneWeights.z();
                Fi
            Fi

            If(boneIds.w() > -1)
                If(boneIds.w() + offset >= boneCount)
                    Return(vec4(vPosition, 1.0f));
                Else
                    vec4 localPosition;
                    localPosition = bones[boneIds.w() + offset].matrix * vec4(vPosition, 1.0f);
                    totalPosition += localPosition * boneWeights.w();
                Fi
            Fi

            Return(totalPosition);
        }
        EndFunction();

        fPosition = drawData.model * getSkinnedVertexPosition(drawData.boneOffset.x());
        setVertexPosition(fPosition);

        return BuildShader();
    }

    Shader ShadowMappingPass::createGeometryShader() {
        BeginShader(Shader::GEOMETRY)

        InputPrimitive(TRIANGLES)
        OutputPrimitive(TRIANGLES, 18)

        Input(vec4, fPosition)
        Output(vec4, FragPos)

        DeclareStruct(PointLightData)
        DeclareStruct(DirLightData)
        DeclareStruct(DrawData)
        DeclareStruct(BoneData)

        Buffer(DrawData, drawData)
        DynamicBuffer(BoneData, bones)
        Buffer(PointLightData, lightData)

        For(Int, face, 0, face <= 5, face + 1)
            setLayer((lightData.layer.x() * 6) + face);
            For(Int, i, 0, i <= 2, i + 1)
                FragPos = fPosition[i];
                setVertexPosition(lightData.shadowMatrices[face] * FragPos);
                EmitVertex();
            Done
            EndPrimitive();
        Done

        return BuildShader();
    }

    Shader ShadowMappingPass::createFragmentShader() {
        BeginShader(Shader::FRAGMENT)

        Input(vec4, FragPos)

        DeclareStruct(PointLightData)
        DeclareStruct(DirLightData)
        DeclareStruct(DrawData)
        DeclareStruct(BoneData)

        Buffer(DrawData, drawData)
        DynamicBuffer(BoneData, bones)
        Buffer(PointLightData, lightData)

        Float lightDistance = length(FragPos.xyz() - lightData.lightPosFarPlane.xyz());

        // map to [0;1] range by dividing by far_plane
        lightDistance = lightDistance / lightData.lightPosFarPlane.w();

        // write this as modified linear depth
        setFragmentDepth(lightDistance);

        return BuildShader();
    }

    Shader ShadowMappingPass::createDirVertexShader() {
        BeginShader(Shader::VERTEX)

        Input(vec3, vPosition)
        Input(vec3, vNormal)
        Input(vec2, vUv)
        Input(vec3, vTangent)
        Input(vec3, vBitangent)
        Input(ivec4, boneIds)
        Input(vec4, boneWeights)

        Output(vec4, fPosition)

        DeclareStruct(PointLightData)
        DeclareStruct(DirLightData)
        DeclareStruct(DrawData)
        DeclareStruct(BoneData)

        Buffer(DrawData, drawData)
        DynamicBuffer(BoneData, bones)
        Buffer(DirLightData, lightData)

        Function("getSkinnedVertexPosition",
                 {ShaderFunction::Argument(ShaderDataType::Int(), "offset")},
                 ShaderDataType::vec4());
        {
            ARGUMENT(Int, offset)

            If(offset < 0)
                Return(vec4(vPosition, 1.0f));
            Fi

            Int boneCount = bones.length();

            vec4 totalPosition;
            totalPosition = vec4(0, 0, 0, 0);

            If(boneIds.x() > -1)
                If(boneIds.x() + offset >= boneCount)
                    Return(vec4(vPosition, 1.0f));
                Else
                    vec4 localPosition;
                    localPosition = bones[boneIds.x() + offset].matrix * vec4(vPosition, 1.0f);
                    totalPosition += localPosition * boneWeights.x();
                Fi
            Fi

            If(boneIds.y() > -1)
                If(boneIds.y() + offset >= boneCount)
                    Return(vec4(vPosition, 1.0f));
                Else
                    vec4 localPosition;
                    localPosition = bones[boneIds.y() + offset].matrix * vec4(vPosition, 1.0f);
                    totalPosition += localPosition * boneWeights.y();
                Fi
            Fi

            If(boneIds.z() > -1)
                If(boneIds.z() + offset >= boneCount)
                    Return(vec4(vPosition, 1.0f));
                Else
                    vec4 localPosition;
                    localPosition = bones[boneIds.z() + offset].matrix * vec4(vPosition, 1.0f);
                    totalPosition += localPosition * boneWeights.z();
                Fi
            Fi

            If(boneIds.w() > -1)
                If(boneIds.w() + offset >= boneCount)
                    Return(vec4(vPosition, 1.0f));
                Else
                    vec4 localPosition;
                    localPosition = bones[boneIds.w() + offset].matrix * vec4(vPosition, 1.0f);
                    totalPosition += localPosition * boneWeights.w();
                Fi
            Fi

            Return(totalPosition);
        }
        EndFunction();

        fPosition = lightData.shadowMatrix
                    * drawData.model
                    * getSkinnedVertexPosition(drawData.boneOffset.x());

        setVertexPosition(fPosition);

        return BuildShader();
    }

    Shader ShadowMappingPass::createDirGeometryShader() {
        BeginShader(Shader::GEOMETRY)

        InputPrimitive(TRIANGLES)
        OutputPrimitive(TRIANGLES, 3)

        Input(vec4, fPosition)

        Output(vec4, fragCoord)

        DeclareStruct(PointLightData)
        DeclareStruct(DirLightData)
        DeclareStruct(DrawData)
        DeclareStruct(BoneData)

        Buffer(DrawData, drawData)
        DynamicBuffer(BoneData, bones)
        Buffer(DirLightData, lightData)

        setLayer(lightData.layer.x());

        For(Int, i, 0, i <= 2, i + 1)
            fragCoord = fPosition[i];
            setVertexPosition(fragCoord);
            EmitVertex();
        Done

        EndPrimitive();

        return BuildShader();
    }

    Shader ShadowMappingPass::createDirFragmentShader() {
        BeginShader(Shader::FRAGMENT)

        Input(vec4, fragCoord)

        DeclareStruct(PointLightData)
        DeclareStruct(DirLightData)
        DeclareStruct(DrawData)
        DeclareStruct(BoneData)

        Buffer(DrawData, drawData)
        DynamicBuffer(BoneData, bones)
        Buffer(DirLightData, lightData)

        return BuildShader();
    }
}
