/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2023  Julian Zampiccoli
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

#include "shadercompilerglsl.hpp"

#include <utility>

#include "xng/rendergraph/shader/shadernode.hpp"

#include "nodecompiler.hpp"
#include "functioncompiler.hpp"
#include "types.hpp"

using namespace xng;

std::string generateElement(const std::string &name, const ShaderDataType &value, std::string prefix = "\t") {
    auto ret = prefix
               + getTypeName(value)
               + " "
               + name;

    if (value.count > 1) {
        ret += "[";
        ret += std::to_string(value.count);
        ret += "]";
    }

    return ret + ";\n";
}

std::string generateHeader(const Shader &source, CompiledPipeline &pipeline) {
    std::string ret;

    if (source.stage == Shader::Stage::VERTEX) {
        ret += "#define " + std::string(drawID) + " gl_DrawID\n\n";
    } else {
        ret += "#define " + std::string(drawID) + " drawID\n\n";
    }

    for (const auto &pair: source.buffers) {
        auto binding = pipeline.createShaderBufferBinding(pair.first);

        std::string bufferLayout = "struct ShaderBufferData" + std::to_string(binding) + " {\n";
        for (const auto &element: pair.second.elements) {
            bufferLayout += generateElement(element.name, element.value);
        }
        bufferLayout += "};\n";

        std::string bufferCode = "layout(binding = "
                                 + std::to_string(binding)
                                 + ", std140) buffer ShaderBuffer"
                                 + std::to_string(binding)
                                 + " {\n"
                                 + "\tShaderBufferData"
                                 + std::to_string(binding)
                                 + " "
                                 + bufferArrayName
                                 + "[];\n} "
                                 + bufferPrefix
                                 + pair.first
                                 + ";\n";

        ret += bufferLayout;
        ret += "\n";
        ret += bufferCode;
        ret += "\n";
    }

    for (auto &pair: source.textureArrays) {
        auto &texArray = pair.second;

        auto textureBinding = pipeline.createTextureArrayBinding(pair.first, texArray.arraySize);

        ret += "layout(binding = "
                + std::to_string(textureBinding)
                + ") uniform "
                + getSampler(texArray.texture)
                + " "
                + texturePrefix
                + pair.first
                + "["
                + std::to_string(texArray.arraySize)
                + "]"
                + ";\n";
    }

    if (source.textureArrays.size() > 0) {
        ret += "\n";
    }

    std::string inputAttributes;
    size_t attributeCount = 0;
    for (auto element: source.inputLayout.elements) {
        auto location = attributeCount++;
        inputAttributes += "layout(location = "
                + std::to_string(location)
                + ") in "
                + generateElement(inputAttributePrefix + std::to_string(location), element, "");
    }
    if (source.stage == Shader::Stage::FRAGMENT) {
        inputAttributes += "layout(location = " + std::to_string(attributeCount) + ") flat in uint drawID;\n";
    }
    ret += inputAttributes;
    ret += "\n";

    std::string outputAttributes;
    attributeCount = 0;
    for (auto element: source.outputLayout.elements) {
        auto location = attributeCount++;
        outputAttributes += "layout(location = "
                + std::to_string(location)
                + ") out "
                + generateElement(outputAttributePrefix + std::to_string(location), element, "");
    }
    if (source.stage == Shader::Stage::VERTEX) {
        outputAttributes += "layout(location = " + std::to_string(attributeCount) + ") flat out uint drawID;\n";
    }
    ret += outputAttributes;
    ret += "\n";

    for (const auto &param: source.parameters) {
        ret += "uniform "
                + getTypeName(param.second)
                + " "
                + parameterPrefix
                + param.first
                + ";\n";
    }

    if (source.parameters.size() > 0) {
        ret += "\n";
    }

    return ret;
}

std::string generateBody(const Shader &source) {
    std::string body;
    for (const auto &func: source.functions) {
        body += compileFunction(func.name, func.arguments, func.body, func.returnType, source);
        body += "\n\n";
    }
    std::string appendix = "";
    if (source.stage == Shader::Stage::VERTEX) {
        appendix = "drawID = gl_DrawID";
    }
    body += compileFunction("main", {}, source.mainFunction, {}, source, appendix);
    return body;
}

CompiledPipeline ShaderCompilerGLSL::compile(const std::vector<Shader> &sources) {
    CompiledPipeline ret;
    for (auto &shader: sources) {
        ret.sourceCode[shader.stage] = compileShader(shader, ret);
    }
    return ret;
}

std::string ShaderCompilerGLSL::compileShader(const Shader &source, CompiledPipeline &pipeline) {
    return "#version 460\n\n"
           + generateHeader(source, pipeline)
           + generateBody(source);
}
