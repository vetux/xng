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

#include "xng/renderer/pipeline/indirect/renderpipelinecompilerindirect.hpp"

#include "xng/shaderscript/shaderscript.hpp"
#include "xng/shaderscript/macro/helpermacros.hpp"

#include "xng/renderer/shaderlib/virtualtexture.hpp"

using namespace xng::ShaderScript;

namespace xng {
    void RenderPipelineCompilerIndirect::compileGetCameraPosition(ShaderScope &scope) {
        FunctionScope fScope("getCameraPosition");

        ShaderObject cameraBuffer(rg::ShaderOperand::storageBuffer(cameraBufferName));

        fScope.setReturnType<vec3>();
        Return(cameraBuffer["position"]);

        scope.addFunction(fScope.build());
    }

    void RenderPipelineCompilerIndirect::compileGetModel(ShaderScope &scope) {
        FunctionScope fScope("getModel");

        ShaderObject drawMeshBuffer(rg::ShaderOperand::storageBuffer(drawMeshBufferName));
        ShaderObject transformBuffer(rg::ShaderOperand::storageBuffer(transformBufferName));

        Int index = getBaseInstance() + getDrawID() + getInstanceID();

        auto drawMesh = drawMeshBuffer[index];

        fScope.setReturnType<mat4>();
        Return(transformBuffer[drawMesh["transformIndex"]]);

        scope.addFunction(fScope.build());
    }

    void RenderPipelineCompilerIndirect::compileGetView(ShaderScope &scope) {
        FunctionScope fScope("getView");

        ShaderObject cameraBuffer(rg::ShaderOperand::storageBuffer(cameraBufferName));

        fScope.setReturnType<mat4>();
        Return(cameraBuffer["view"]);

        scope.addFunction(fScope.build());
    }

    void RenderPipelineCompilerIndirect::compileGetProjection(ShaderScope &scope) {
        FunctionScope fScope("getProjection");

        ShaderObject cameraBuffer(rg::ShaderOperand::storageBuffer(cameraBufferName));

        fScope.setReturnType<mat4>();
        Return(cameraBuffer["projection"]);

        scope.addFunction(fScope.build());
    }

    void RenderPipelineCompilerIndirect::compileGetModelViewProjection(ShaderScope &scope) {
        FunctionScope fScope("getModelViewProjection");

        ShaderObject drawMeshBuffer(rg::ShaderOperand::storageBuffer(drawMeshBufferName));

        auto drawMesh = drawMeshBuffer[getBaseInstance() + getDrawID() + getInstanceID()];

        fScope.setReturnType<mat4>();
        Return(drawMesh["mvp"]);

        scope.addFunction(fScope.build());
    }

    void RenderPipelineCompilerIndirect::compileGetMaterialProperty(
        ShaderScope &scope,
        RenderPipelineMaterial::PropertyID prop,
        rg::ShaderPrimitiveType type
    ) {
        FunctionScope fScope("getMaterialAttribute");

        ShaderObject drawMeshBuffer(rg::ShaderOperand::storageBuffer(drawMeshBufferName));
        ShaderObject materialBuffer(rg::ShaderOperand::storageBuffer(materialBufferName));

        auto drawMesh = drawMeshBuffer[getBaseInstance() + getDrawID() + getInstanceID()];

        auto material = materialBuffer[drawMesh["materialIndex"]];

        const auto attributeName = std::string(materialPropertyPrefix) + std::to_string(prop);

        fScope.setReturnType(rg::ShaderDataType(type));
        Return(material[attributeName.c_str()]);

        scope.addFunction(fScope.build());
    }

    void RenderPipelineCompilerIndirect::compileSampleMaterialTexture(ShaderScope &scope,
                                                                    RenderPipelineMaterial::TextureID tex) {
        Param<vec2> uv(parameter("uv"));
        FunctionScope fScope("sampleMaterialTexture");

        ShaderObject drawMeshBuffer(rg::ShaderOperand::storageBuffer(drawMeshBufferName));
        ShaderObject materialBuffer(rg::ShaderOperand::storageBuffer(materialBufferName));

        ShaderObject atlasSize(rg::ShaderOperand::parameter(virtualAtlasSizeName));
        ShaderObject tileSize(rg::ShaderOperand::parameter(virtualTileSizeName));
        ShaderObject tileBorder(rg::ShaderOperand::parameter(virtualTileBorderName));
        ShaderObject maxAnisotropy(rg::ShaderOperand::parameter(virtualMaxAnisotropyName));

        StorageBufferWrapper<UInt> tileMapOffsets(rg::ShaderOperand::storageBuffer(virtualTileMapOffsetsName));
        StorageBufferWrapper<UInt> tileMap(rg::ShaderOperand::storageBuffer(virtualTileMapName));
        StorageBufferWrapper<UInt> residencyMapOffsets(rg::ShaderOperand::storageBuffer(virtualResidencyMapOffsetsName));
        StorageBufferWrapper<UInt> residencyMap(rg::ShaderOperand::storageBuffer(virtualResidencyMapName));
        StorageBufferWrapper<UInt> readbackBuffer(rg::ShaderOperand::storageBuffer(virtualReadbackBufferName));

        ShaderObject atlasTexture(rg::ShaderOperand::texture(virtualAtlasTextureName));

        auto drawMesh = drawMeshBuffer[getBaseInstance() + getDrawID() + getInstanceID()];

        auto material = materialBuffer[drawMesh["materialIndex"]];

        const auto texture = materialTexturePrefix + std::to_string(tex);

        fScope.setReturnType<vec4>();

        const auto textureID = ShaderTexture::getTextureID(texture, material);
        const auto wrap = ShaderTexture::getWrap(texture, material);
        const auto minFilter = ShaderTexture::getMinFilter(texture, material);
        const auto magFilter = ShaderTexture::getMagFilter(texture, material);
        const auto mipFilter = ShaderTexture::getMipFilter(texture, material);
        const auto imageSize = ShaderTexture::getTextureSize(texture, material);
        const auto maxMip = ShaderTexture::getMaxMip(texture, material);

        Return(shaderlib::virtualtexture::sample_virtual_readback(textureID,
                                                                  uv,
                                                                  wrap,
                                                                  minFilter,
                                                                  magFilter,
                                                                  mipFilter,
                                                                  imageSize,
                                                                  maxMip,
                                                                  atlasSize,
                                                                  tileSize,
                                                                  tileBorder,
                                                                  maxAnisotropy,
                                                                  tileMapOffsets,
                                                                  tileMap,
                                                                  residencyMapOffsets,
                                                                  residencyMap,
                                                                  readbackBuffer,
                                                                  atlasTexture));

        scope.addFunction(fScope.build());
    }

    void RenderPipelineCompilerIndirect::compileWriteAttachment(ShaderScope &scope,
                                                              const unsigned int index,
                                                              const RenderPipelineShader::Attachment &
                                                              attachment) {
        Param<vec4> color(parameter("color"));
        FunctionScope fScope("sampleMaterialTexture");

        if (attachment.type == RenderPipelineShader::Attachment::ATTACHMENT_TEXTURE) {
            throw std::runtime_error("writeAttachment for virtual texture not implemented yet.");
        } else {
            outputAttribute(attachmentPrefix + std::to_string(index)) = color;
        }

        scope.addFunction(fScope.build());
    }
}
