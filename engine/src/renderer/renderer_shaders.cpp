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

        ComputeLocalSize(skinningLocalSize, 1, 1);

        DynamicBuffer(ShaderTransform, bones)
        DynamicBuffer(Float, positions)
        DynamicBuffer(ivec4, boneIds)
        DynamicBuffer(vec4, boneWeights)

        DynamicBufferRW(Float, skinnedPositions)

        Parameter(UInt, vertexCount)
        Parameter(UInt, baseVertex)
        Parameter(UInt, skinBaseVertex)
        Parameter(UInt, baseBone)

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

        //TODO: Implement Normals / Tangents and bitangents skinning
        vec4 totalPosition;
        totalPosition = vec4(0, 0, 0, 0);

        If(vertexBoneIds.x() > -1)
            vec4 localPosition;
            localPosition = bones[baseBone + vertexBoneIds.x()].transform * position;
            totalPosition += localPosition * vertexBoneWeights.x();
        Fi

        If(vertexBoneIds.y() > -1)
            vec4 localPosition;
            localPosition = bones[baseBone + vertexBoneIds.y()].transform * position;
            totalPosition += localPosition * vertexBoneWeights.y();
        Fi

        If(vertexBoneIds.z() > -1)
            vec4 localPosition;
            localPosition = bones[baseBone + vertexBoneIds.z()].transform * position;
            totalPosition += localPosition * vertexBoneWeights.z();
        Fi

        If(vertexBoneIds.w() > -1)
            vec4 localPosition;
            localPosition = bones[baseBone + vertexBoneIds.w()].transform * position;
            totalPosition += localPosition * vertexBoneWeights.w();
        Fi

        skinnedPositions[floatOutputIndex] = totalPosition.x();
        skinnedPositions[floatOutputIndex + 1] = totalPosition.y();
        skinnedPositions[floatOutputIndex + 2] = totalPosition.z();

        return BuildShader();
    }

    rg::Shader Renderer::compileScenePrepassShader() {
        BeginShader(rg::Shader::COMPUTE)

        ComputeLocalSize(prePassLocalSize, 1, 1);

        Buffer(ShaderCamera, camera)
        DynamicBuffer(ShaderTransform, transforms)
        DynamicBuffer(ShaderMesh, meshBuffer)
        DynamicBuffer(UInt, meshIndices)
        DynamicBufferRW(ShaderDrawMesh, drawBuffer)

        DynamicBufferRW(ShaderDrawIndirectIndexed, commandBuffer)
        DynamicBufferRW(Int, commandCountBuffer)

        Parameter(UInt, meshIndexOffset)
        Parameter(UInt, batchSize)

        If(getGlobalInvocationID().x() >= batchSize)
            Return();
        Fi

        UInt meshIndex = meshIndices[getGlobalInvocationID().x() + meshIndexOffset];
        ShaderMesh mesh = meshBuffer[meshIndex];

        // TODO: Culling
        // TODO: Multi Draw / Instancing

        ShaderDrawMesh drawMesh;
        drawMesh.mvp = camera.projection * camera.view * transforms[mesh.transformIndex].transform;
        drawMesh.modelID = mesh.modelID;
        drawMesh.meshID = mesh.meshID;
        drawMesh.transformIndex = mesh.transformIndex;
        drawMesh.materialIndex = mesh.materialIndex;
        drawMesh.receiveShadows = mesh.receiveShadows;
        drawBuffer[getGlobalInvocationID().x() + meshIndexOffset] = drawMesh;

        ShaderDrawIndirectIndexed command;
        command.indexCount = mesh.indexCount;
        command.instanceCount = UInt(1);
        command.firstIndex = mesh.indexOffset;
        command.baseVertex = mesh.baseVertex;
        command.baseInstance = UInt(getGlobalInvocationID().x() + meshIndexOffset);
        commandBuffer[getGlobalInvocationID().x()] = command;

        // Currently no Culling, so we set to batchSize.
        // When culling is implemented, the count buffer would only increment with the draws which survive culling.
        If(getGlobalInvocationID().x() == 0)
            commandCountBuffer[0] = Int(batchSize);
        Fi

        return BuildShader();
    }
}
