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

#include "xng/renderer/renderer.hpp"

#include "xng/shaderscript/shaderscript.hpp"
#include "xng/shaderscript/macro/helpermacros.hpp"

using namespace xng::ShaderScript;

namespace xng {
    rg::Shader Renderer::compileSkinningShader() {
        BeginShader(rg::Shader::COMPUTE)

        ComputeLocalSize(64, 1, 1);

        DynamicBuffer(ShaderTransform, bones)
        DynamicBuffer(Float, positions)
        DynamicBuffer(ivec4, boneIds)
        DynamicBuffer(vec4, boneWeights)

        DynamicBufferRW(Float, skinnedPositions)

        Parameter(UInt, vertexCount)
        Parameter(UInt, baseVertex)
        Parameter(UInt, skinBaseVertex)

        If(getGlobalInvocationID().x() >= vertexCount)
            Return();
        Fi

        UInt outputIndex = baseVertex + getGlobalInvocationID().x();
        UInt index = skinBaseVertex + getGlobalInvocationID().x();

        UInt floatOutputIndex = outputIndex * 3;
        UInt floatIndex = index * 3;

        vec4 position = vec4(positions[floatIndex], positions[floatIndex + 1], positions[floatIndex + 2], 1.0f);
        ivec4 vertexBoneIds = boneIds[index];
        vec4 vertexBoneWeights = boneWeights[index];

        vec4 totalPosition;
        totalPosition = vec4(0, 0, 0, 0);

        If(vertexBoneIds.x() > -1)
            vec4 localPosition;
            localPosition = bones[vertexBoneIds.x()].transform * position;
            totalPosition += localPosition * vertexBoneWeights.x();
        Fi

        If(vertexBoneIds.y() > -1)
            vec4 localPosition;
            localPosition = bones[vertexBoneIds.y()].transform * position;
            totalPosition += localPosition * vertexBoneWeights.y();
        Fi

        If(vertexBoneIds.z() > -1)
            vec4 localPosition;
            localPosition = bones[vertexBoneIds.z()].transform * position;
            totalPosition += localPosition * vertexBoneWeights.z();
        Fi

        If(vertexBoneIds.w() > -1)
            vec4 localPosition;
            localPosition = bones[vertexBoneIds.w()].transform * position;
            totalPosition += localPosition * vertexBoneWeights.w();
        Fi

        skinnedPositions[floatOutputIndex] = totalPosition.x();
        skinnedPositions[floatOutputIndex + 1] = totalPosition.y();
        skinnedPositions[floatOutputIndex + 2] = totalPosition.z();

        return BuildShader();
    }

    rg::Shader Renderer::compileScenePrepassShader() {
        BeginShader(rg::Shader::COMPUTE)
        return BuildShader();
    }
}
