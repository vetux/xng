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

#ifndef XENGINE_RENDERSHADERBUILDER_HPP
#define XENGINE_RENDERSHADERBUILDER_HPP

#include "xng/renderer/pipeline/rendershadercompiler.hpp"

namespace xng {
    class RenderShaderBuilder {
    public:
        RenderShaderBuilder(RenderShaderCompiler &compiler,
                            std::vector<RenderShader::Attachment> attachments)
            : compiler(compiler),
              attachments(std::move(attachments)) {
        }

        rg::ShaderOperand getVertexAttribute(const VertexAttribute attr) {
            vertexAttributes.insert(attr);
            return compiler.getVertexAttribute(attr);
        }

        rg::ShaderOperand getMaterialAttribute(const RenderPipelineMaterial::AttributeID attr) {
            materialAttributes.insert(attr);
            return compiler.getMaterialAttribute(attr);
        }

        rg::ShaderInstruction writeAttachment(const unsigned int index, const rg::ShaderOperand &color) const {
            return compiler.writeAttachment(index, color);
        }

        std::shared_ptr<RenderShader> compile(const rg::RasterPipeline &pipeline) const {
            return compiler.compile(pipeline,
                                    attachments,
                                    vertexAttributes,
                                    materialAttributes);
        }

    private:
        RenderShaderCompiler &compiler;
        std::vector<RenderShader::Attachment> attachments;
        std::unordered_set<VertexAttribute> vertexAttributes;
        std::unordered_set<RenderPipelineMaterial::AttributeID> materialAttributes;
    };
}

#endif //XENGINE_RENDERSHADERBUILDER_HPP
