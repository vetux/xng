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

#include "shadercompilerglsl.hpp"

#include <utility>

#include "instructioncompiler.hpp"
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

    return ret;
}

std::string generateHeader(const Shader &source, CompiledPipeline &pipeline) {
    std::string ret;

    if (source.stage == Shader::Stage::VERTEX) {
        ret += "#define " + std::string(drawID) + " gl_DrawID\n\n";
    } else if (source.stage == Shader::Stage::GEOMETRY) {
        ret += "#define " + std::string(drawID) + " in_drawID[0]\n\n";
    } else {
        ret += "#define " + std::string(drawID) + " drawID\n\n";
    }

    for (const auto &v: source.typeDefinitions) {
        ret += "struct " + v.name + " {\n";
        for (const auto &element: v.elements) {
            if (std::holds_alternative<ShaderStructTypeName>(element.type)) {
                ret += "\t" + std::get<ShaderStructTypeName>(element.type) + " " + element.name + ";\n";
            } else {
                ret += generateElement(element.name, std::get<ShaderDataType>(element.type)) + ";\n";
            }
        }
        ret += "};\n\n";
    }

    for (const auto &pair: source.buffers) {
        auto binding = pipeline.createShaderBufferBinding(pair.first);
        ret += "layout(binding = "
                + std::to_string(binding)
                + ", std140) buffer ShaderBuffer"
                + std::to_string(binding)
                + " {\n"
                + "\t" + pair.second.typeName + " " + bufferArrayName
                + "[];\n} "
                + bufferPrefix
                + pair.first
                + ";\n";
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
    if (source.stage == Shader::GEOMETRY) {
        switch (source.geometryInput) {
            case POINTS:
                inputAttributes += "layout(points) in;\n";
                break;
            case LINES:
                inputAttributes += "layout(lines) in;\n";
                break;
            case TRIANGLES:
                inputAttributes += "layout(triangles) in;\n";
                break;
            case QUAD:
                throw std::runtime_error("Geometry shaders with quad input not supported");
        }
        switch (source.geometryOutput) {
            case POINTS:
                inputAttributes += "layout(points, max_vertices = ";
                break;
            case LINES:
                inputAttributes += "layout(line_strip, max_vertices = ";
                break;
            case TRIANGLES:
                inputAttributes += "layout(triangle_strip, max_vertices = ";
                break;
            case QUAD:
                throw std::runtime_error("Geometry shaders with quad output not supported");
        }
        inputAttributes += std::to_string(source.geometryMaxVertices) + ") out;\n";
        inputAttributes += "\n";

        for (auto element: source.inputLayout.getElements()) {
            auto location = attributeCount++;
            inputAttributes += "layout(location = "
                    + std::to_string(location)
                    + ") in "
                    + generateElement(inputAttributePrefix + source.inputLayout.getElementName(location),
                                      element,
                                      "")
                    + "[];\n";
        }
        inputAttributes += "layout(location = " + std::to_string(attributeCount) + ") flat in uint in_drawID[];\n";
    } else {
        for (auto element: source.inputLayout.getElements()) {
            auto location = attributeCount++;
            inputAttributes += "layout(location = "
                    + std::to_string(location)
                    + ") in "
                    + generateElement(inputAttributePrefix + source.inputLayout.getElementName(location),
                                      element,
                                      "")
                    + ";\n";
        }
        if (source.stage == Shader::Stage::FRAGMENT) {
            inputAttributes += "layout(location = " + std::to_string(attributeCount) + ") flat in uint drawID;\n";
        }
    }
    ret += inputAttributes;
    ret += "\n";

    std::string outputAttributes;
    attributeCount = 0;
    for (auto element: source.outputLayout.getElements()) {
        auto location = attributeCount++;
        outputAttributes += "layout(location = "
                + std::to_string(location)
                + ") out "
                + generateElement(outputAttributePrefix + source.outputLayout.getElementName(location),
                                  element,
                                  "")
                + +";\n";;
    }
    if (source.stage == Shader::Stage::VERTEX || source.stage == Shader::Stage::GEOMETRY) {
        outputAttributes += "layout(location = " + std::to_string(attributeCount) + ") flat out uint out_drawID;\n";
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
        appendix = "out_drawID = gl_DrawID";
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
