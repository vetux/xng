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

std::string generateElement(const std::string &name, const ShaderDataType &type, std::string prefix = "\t") {
    std::string ret = prefix;

    if (std::holds_alternative<ShaderPrimitiveType>(type.value)) {
        ret += getTypeName(std::get<ShaderPrimitiveType>(type.value));
    } else {
        ret += std::get<ShaderStructType>(type.value);
    }
    ret += " " + name;

    if (type.count > 1) {
        ret += "[";
        ret += std::to_string(type.count);
        ret += "]";
    }

    return ret;
}

std::string generateHeader(const Shader &source, CompiledShader &pipeline) {
    std::string ret;

    for (const auto &v: source.typeDefinitions) {
        ret += "struct " + v.typeName + " {\n";
        for (const auto &element: v.elements) {
            ret += generateElement(element.name, element.type) + ";\n";
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
                + "\t" + pair.second.typeName + " " + bufferArrayName;
        if (pair.second.dynamic) {
            ret += "[]";
        }
        ret += ";\n} ";
        ret += bufferPrefix
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
                                      ShaderDataType(element),
                                      "")
                    + "[];\n";
        }
    } else {
        for (auto element: source.inputLayout.getElements()) {
            auto location = attributeCount++;
            inputAttributes += "layout(location = "
                    + std::to_string(location)
                    + ") in "
                    + generateElement(inputAttributePrefix + source.inputLayout.getElementName(location),
                                      ShaderDataType(element),
                                      "")
                    + ";\n";
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
                                  ShaderDataType(element),
                                  "")
                + +";\n";;
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
    body += compileFunction("main", {}, source.mainFunction, {}, source);
    return body;
}

CompiledShader ShaderCompilerGLSL::compile(const std::vector<Shader> &sources) {
    CompiledShader ret;
    for (auto &shader: sources) {
        ret.sourceCode[shader.stage] = compileShader(shader, ret);
    }
    return ret;
}

std::string ShaderCompilerGLSL::compileShader(const Shader &source, CompiledShader &pipeline) {
    return "#version 460\n\n"
           + generateHeader(source, pipeline)
           + generateBody(source);
}
