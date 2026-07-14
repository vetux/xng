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
#include "xng/renderer/pipeline/rendershader.hpp"

namespace xng {
    class RenderShaderCompiler {
    public:
        virtual ~RenderShaderCompiler() = default;

        /**
         * Retrieve vertex attribute value. (Only valid in Vertex stage)
         *
         * @param attr
         * @return
         */
        virtual rg::ShaderInstruction getVertexAttribute(VertexAttribute attr) = 0;

        /**
         * Retrieve instance attribute value.
         *
         * @param attr
         * @return
         */
        virtual rg::ShaderInstruction getInstanceAttribute(RenderShader::InstanceAttribute attr) = 0;

        /**
         * Retrieve global attribute value.
         *
         * @param attr
         * @return
         */
        virtual rg::ShaderInstruction getGlobalAttribute(RenderShader::GlobalAttribute attr) = 0;

        /**
         * Retrieve indexed attribute value.
         *
         * @param attr
         * @param index
         * @return
         */
        virtual rg::ShaderInstruction getIndexedAttribute(RenderShader::IndexedAttribute attr,
                                                          const rg::ShaderInstruction &index) = 0;

        /**
         * Write the specified color value to the specified attachment.
         *
         * This wraps virtual texture write if the shader attachment is a virtual texture.
         *
         * @param index
         * @param color
         * @return
         */
        virtual rg::ShaderInstruction writeAttachment(unsigned int index, const rg::ShaderInstruction &color) = 0;

        /**
         * Inject the pipeline dependent required bindings, parameters, functions and vertex layout into the passed shaders.
         *
         * Users are free to do anything between vertex input and fragment output such as geometry stage etc. and may bind and access custom data.
         *
         * @param pipeline The pipeline containing the shader stages and pipeline configuration. Attachments and vertex layout are overridden.
         * @param attachments The format of the attachments.
         * @param vertexAttributes The set of accessed vertex attributes.
         * @param instanceAttributes The set of accessed attributes.
         * @param globalAttributes
         * @param indexedAttributes
         */
        virtual std::shared_ptr<RenderShader> compile(const rg::RasterPipeline &pipeline,
                                                      const std::vector<RenderShader::Attachment> &attachments,
                                                      const std::unordered_set<VertexAttribute> &vertexAttributes,
                                                      const std::unordered_set<RenderShader::InstanceAttribute> &
                                                      instanceAttributes,
                                                      const std::unordered_set<RenderShader::GlobalAttribute> &
                                                      globalAttributes,
                                                      const std::unordered_set<RenderShader::IndexedAttribute> &
                                                      indexedAttributes) = 0;
    };
}

#endif //XENGINE_RENDERSHADERCOMPILER_HPP
