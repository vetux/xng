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

#ifndef XENGINE_RENDERSHADERCOMPILER_HPP
#define XENGINE_RENDERSHADERCOMPILER_HPP

#include <unordered_set>

#include "xng/rendergraph/shader/shader.hpp"

#include "xng/renderer/vertexattribute.hpp"
#include "xng/renderer/pipeline/renderpipelineshader.hpp"

namespace xng {
    class RenderPipelineCompiler {
    public:
        virtual ~RenderPipelineCompiler() = default;

        static constexpr auto getVertexAttributePrefix = "_getVertexAttribute";

        static constexpr auto getCameraPositionName = "_getCameraPosition";

        static constexpr auto getModelName = "_getModel";

        static constexpr auto getViewName = "_getView";

        static constexpr auto getProjectionName = "_getProjection";

        static constexpr auto getModelViewProjectionName = "_getModelViewProjection";

        static constexpr auto getMaterialPropertyPrefix = "_getMaterialProperty";

        static constexpr auto sampleMaterialTexturePrefix = "_sampleMaterialTexture";

        static constexpr auto writeAttachmentPrefix = "_writeAttachment";

        /**
         * Retrieve vertex attribute value. (Only valid in Vertex stage)
         *
         * @param attr
         * @return
         */
        static rg::ShaderOperand getVertexAttribute(const VertexAttribute attr) {
            return rg::ShaderOperand::instruction(rg::ShaderInstructionFactory::call(
                getVertexAttributePrefix + std::to_string(attr),
                {}));
        }

        static rg::ShaderOperand getCameraPosition() {
            return rg::ShaderOperand::instruction(rg::ShaderInstructionFactory::call(getCameraPositionName, {}));
        }

        static rg::ShaderOperand getModel() {
            return rg::ShaderOperand::instruction(rg::ShaderInstructionFactory::call(getModelName, {}));
        }

        static rg::ShaderOperand getView() {
            return rg::ShaderOperand::instruction(rg::ShaderInstructionFactory::call(getViewName, {}));
        }

        static rg::ShaderOperand getProjection() {
            return rg::ShaderOperand::instruction(rg::ShaderInstructionFactory::call(getProjectionName, {}));
        }

        static rg::ShaderOperand getModelViewProjection() {
            return rg::ShaderOperand::instruction(rg::ShaderInstructionFactory::call(getModelViewProjectionName, {}));
        }

        /**
         * Retrieve material property value.
         *
         * @param attr
         * @return
         */
        static rg::ShaderOperand getMaterialProperty(const RenderPipelineMaterial::PropertyID attr) {
            return rg::ShaderOperand::instruction(
                rg::ShaderInstructionFactory::call(getMaterialPropertyPrefix + std::to_string(attr), {}));
        }

        /**
         * Sample a material texture.
         *
         * @param tex
         * @param uv
         * @return
         */
        static rg::ShaderOperand sampleMaterialTexture(const RenderPipelineMaterial::TextureID tex,
                                                       const rg::ShaderOperand &uv) {
            return rg::ShaderOperand::instruction(rg::ShaderInstructionFactory::call(
                sampleMaterialTexturePrefix + std::to_string(tex),
                {uv}));
        }

        /**
         * Write the specified color value to the specified attachment.
         *
         * This wraps virtual texture write if the shader attachment is a virtual texture.
         *
         * @param index
         * @param color
         * @return
         */
        static rg::ShaderInstruction writeAttachment(const unsigned int index, const rg::ShaderOperand &color) {
            return rg::ShaderInstructionFactory::call(writeAttachmentPrefix + std::to_string(index),
                                                      {color});
        }

        /**
         * Inject the pipeline dependent required bindings, parameters, functions and vertex layout into the passed shaders.
         *
         * Users are free to do anything between vertex input and fragment output such as geometry stage etc. and may bind and access custom data.
         *
         * @param shaders The pipeline shaders.
         * @param pipelineConfig The pipeline configuration.
         * @param attachments The format of the attachments.
         * @param depthAttachmentFormat The format of the depth attachment
         * @param stencilAttachmentFormat The format of the stencil attachment
         */
        virtual std::shared_ptr<RenderPipelineShader> compile(const std::vector<rg::Shader> &shaders,
                                                              const rg::RasterPipeline::Configuration &pipelineConfig,
                                                              const std::vector<RenderPipelineShader::Attachment> &
                                                              attachments,
                                                              std::optional<rg::ColorFormat> depthAttachmentFormat,
                                                              std::optional<rg::ColorFormat> stencilAttachmentFormat) =
        0;
    };
}

#endif //XENGINE_RENDERSHADERCOMPILER_HPP
