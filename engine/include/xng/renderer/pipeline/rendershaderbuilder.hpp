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

#include "rendershadercompiler.hpp"

namespace xng {
    class RenderShaderBuilder {
    public:
        RenderShaderBuilder(RenderShaderCompiler &compiler,
                            std::vector<RenderShader::Attachment> attachments)
            : compiler(compiler),
              attachments(std::move(attachments)) {
        }

        rg::ShaderInstruction getVertexAttribute(const VertexAttribute attr) {
            vertexAttributes.insert(attr);
            return compiler.getVertexAttribute(attr);
        }

        rg::ShaderInstruction getAttribute(const RenderShader::Attribute attr) {
            attributes.insert(attr);
            return compiler.getAttribute(attr);
        }

        rg::ShaderInstruction getAttribute(const RenderShader::Attribute attr,
                                           const rg::ShaderInstruction &index) {
            attributes.insert(attr);
            return compiler.getAttribute(attr, index);
        }

        rg::ShaderInstruction writeAttachment(const unsigned int index, const rg::ShaderInstruction &color) const {
            return compiler.writeAttachment(index, color);
        }

        std::shared_ptr<RenderShader> compile(const std::vector<rg::Shader> &stages) const {
            return compiler.compile(stages, attachments, attributes, vertexAttributes);
        }

    private:
        RenderShaderCompiler &compiler;
        std::vector<RenderShader::Attachment> attachments;
        std::unordered_set<RenderShader::Attribute> attributes;
        std::unordered_set<VertexAttribute> vertexAttributes;
    };
}

#endif //XENGINE_RENDERSHADERBUILDER_HPP
