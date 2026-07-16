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
        ret += std::get<ShaderStructTypeName>(type.value);
    }
    ret += " " + name;

    if (type.count > 1) {
        ret += "[";
        ret += std::to_string(type.count);
        ret += "]";
    }

    return ret;
}

std::string getInterpolationKeyword(const rg::ShaderAttributeLayout::InterpolationMode mode) {
    switch (mode) {
        case ShaderAttributeLayout::INTERPOLATE_SMOOTH:
            return "smooth";
        case ShaderAttributeLayout::INTERPOLATE_NO_PERSPECTIVE:
            return "noperspective";
        case ShaderAttributeLayout::INTERPOLATE_FLAT:
            return "flat";
        default:
            throw std::runtime_error("Invalid interpolation mode");
    }
}

std::string generateHeader(const Shader &source, CompiledShader &compiledShader) {
    std::string ret;

    for (const auto &v: source.typeDefinitions) {
        ret += "struct " + v.typeName + " {\n";
        for (const auto &element: v.elements) {
            ret += generateElement(element.name, element.type) + ";\n";
        }
        ret += "};\n\n";
    }

    for (const auto &pair: source.buffers) {
        auto binding = compiledShader.createShaderBufferBinding(pair.first);
        ret += "layout(binding = "
                + std::to_string(binding);
        if (std::holds_alternative<ShaderStructTypeName>(pair.second.type.value)) {
            ret += std::string(", std140");
        } else {
            ret += std::string(", std430");
        }
        ret += std::string(") buffer ShaderBuffer")
                + std::to_string(binding)
                + " {\n"
                + "\t";
        if (std::holds_alternative<ShaderStructTypeName>(pair.second.type.value)) {
            ret += std::get<ShaderStructTypeName>(pair.second.type.value) + " " + bufferArrayName;
        } else {
            ret += getTypeName(std::get<ShaderPrimitiveType>(pair.second.type.value)) + " " + bufferArrayName;
        }
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

        auto textureBinding = compiledShader.createTextureArrayBinding(pair.first, texArray.arraySize);

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
    size_t layoutIndex = 0;
    size_t elementIndex = 0;
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

        for (auto i = 0; i < source.inputLayout.getElements().size(); i++) {
            auto element = source.inputLayout.getElements().at(i);
            auto location = elementIndex++;
            inputAttributes += "layout(location = "
                    + std::to_string(layoutIndex)
                    + ") "
                    + getInterpolationKeyword(source.inputLayout.getInterpolationModes().at(i))
                    + " in "
                    + generateElement(inputAttributePrefix + source.inputLayout.getElementName(location),
                                      ShaderDataType(element),
                                      "")
                    + "[];\n";
            switch (element.type) {
                case ShaderPrimitiveType::SCALAR:
                case ShaderPrimitiveType::VECTOR2:
                case ShaderPrimitiveType::VECTOR3:
                case ShaderPrimitiveType::VECTOR4:
                    layoutIndex += 1;
                    break;
                case ShaderPrimitiveType::MAT2:
                    layoutIndex += 2;
                    break;
                case ShaderPrimitiveType::MAT3:
                    layoutIndex += 3;
                    break;
                case ShaderPrimitiveType::MAT4:
                    layoutIndex += 4;
                    break;
            }
        }
    } else if (source.stage == Shader::VERTEX || source.stage == Shader::FRAGMENT) {
        for (auto i = 0; i < source.inputLayout.getElements().size(); i++) {
            auto element = source.inputLayout.getElements().at(i);
            auto location = elementIndex++;
            inputAttributes += "layout(location = "
                    + std::to_string(layoutIndex)
                    + ")";
            if (source.stage == Shader::FRAGMENT) {
                inputAttributes += " "
                        + getInterpolationKeyword(source.inputLayout.getInterpolationModes().at(i))
                        + " ";
            }
            inputAttributes += " in "
                    + generateElement(inputAttributePrefix + source.inputLayout.getElementName(location),
                                      ShaderDataType(element),
                                      "")
                    + ";\n";
            switch (element.type) {
                case ShaderPrimitiveType::SCALAR:
                case ShaderPrimitiveType::VECTOR2:
                case ShaderPrimitiveType::VECTOR3:
                case ShaderPrimitiveType::VECTOR4:
                    layoutIndex += 1;
                    break;
                case ShaderPrimitiveType::MAT2:
                    layoutIndex += 2;
                    break;
                case ShaderPrimitiveType::MAT3:
                    layoutIndex += 3;
                    break;
                case ShaderPrimitiveType::MAT4:
                    layoutIndex += 4;
                    break;
            }
        }
    } else {
        inputAttributes += "layout(local_size_x = " + std::to_string(source.computeLocalSize.x)
                + ", local_size_y = " + std::to_string(source.computeLocalSize.y)
                + ", local_size_z = " + std::to_string(source.computeLocalSize.z)
                + ") in;\n";
    }

    ret += inputAttributes;
    ret += "\n";

    if (source.stage != Shader::COMPUTE) {
        std::string outputAttributes;
        layoutIndex = 0;
        elementIndex = 0;
        for (auto element: source.outputLayout.getElements()) {
            auto location = elementIndex++;
            outputAttributes += "layout(location = "
                    + std::to_string(layoutIndex)
                    + ")";
            if (source.stage == Shader::VERTEX || source.stage == Shader::GEOMETRY) {
                outputAttributes += " "
                        + getInterpolationKeyword(source.outputLayout.getInterpolationModes().at(location))
                        + " ";
            }
            outputAttributes += "out "
                    + generateElement(outputAttributePrefix + source.outputLayout.getElementName(location),
                                      ShaderDataType(element),
                                      "")
                    + ";\n";
            switch (element.type) {
                case ShaderPrimitiveType::SCALAR:
                case ShaderPrimitiveType::VECTOR2:
                case ShaderPrimitiveType::VECTOR3:
                case ShaderPrimitiveType::VECTOR4:
                    layoutIndex += 1;
                    break;
                case ShaderPrimitiveType::MAT2:
                    layoutIndex += 2;
                    break;
                case ShaderPrimitiveType::MAT3:
                    layoutIndex += 3;
                    break;
                case ShaderPrimitiveType::MAT4:
                    layoutIndex += 4;
                    break;
            }
        }
        ret += outputAttributes;
        ret += "\n";
    }

    for (const auto &param: source.parameters) {
        ret += "layout(location = "
                + std::to_string(compiledShader.createParameterBinding(param.first, param.second))
                + ") uniform "
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
