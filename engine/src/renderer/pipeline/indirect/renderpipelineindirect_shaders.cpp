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

#include "xng/renderer/pipeline/indirect/renderpipelineindirect.hpp"

#include "xng/shaderscript/shaderscript.hpp"
#include "xng/shaderscript/macro/helpermacros.hpp"
#include "xng/shaderscript/indirectbuffers.hpp"

using namespace xng::ShaderScript;

namespace xng {
    rg::Shader RenderPipelineIndirect::getPrePassShader() {
        BeginShader(rg::Shader::COMPUTE)

        ComputeLocalSize(prePassLocalSize, 1, 1);

        StorageBuffer(RenderPipelineCompilerIndirect::ShaderCamera, camera)
        StorageBufferDynamic(mat4, transforms)
        StorageBufferDynamic(ShaderDrawCall, drawCallBuffer)

        StorageBufferDynamicRW(RenderPipelineCompilerIndirect::ShaderDrawMesh, drawMeshBuffer)

        StorageBufferDynamicRW(ShaderDrawIndirectIndexed, commandBuffer)
        StorageBufferDynamicRW(Int, commandCountBuffer)

        Parameter(Int, batchSize)

        If(getGlobalInvocationID().x() >= batchSize)
            Return();
        Fi

        ShaderDrawCall drawCall = drawCallBuffer[getGlobalInvocationID().x()];

        // TODO: Culling

        RenderPipelineCompilerIndirect::ShaderDrawMesh drawMesh;
        drawMesh.mvp = camera.projection * camera.view * transforms[drawCall.transformIndex];
        drawMesh.transformIndex = drawCall.transformIndex;
        drawMesh.materialIndex = drawCall.materialIndex;
        drawMeshBuffer[getGlobalInvocationID().x()] = drawMesh;

        ShaderDrawIndirectIndexed command;
        command.indexCount = drawCall.indexCount;
        command.instanceCount = UInt(1);
        command.firstIndex = drawCall.indexOffset;
        command.baseVertex = drawCall.baseVertex;
        command.baseInstance = UInt(0);
        commandBuffer[getGlobalInvocationID().x()] = command;

        // Currently no Culling, so we set to batchSize.
        // When culling is implemented, the count buffer would only increment with the draws which survive culling.
        If(getGlobalInvocationID().x() == 0)
            commandCountBuffer[0] = Int(batchSize);
        Fi

        EndShader();

        return BuildShader();
    }
}