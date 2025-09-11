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

#include "xng/render/graph2/shader/fgshadernode.hpp"

#include "nodecompiler.hpp"
#include "functioncompiler.hpp"
#include "types.hpp"

using namespace xng;

std::string getSampler(const FGTexture &texture) {
    std::string prefix;
    if (texture.format >= R8I && texture.format <= RGBA32I) {
        prefix = "i";
    } else if (texture.format >= R8UI && texture.format <= RGBA32UI) {
        prefix = "u";
    }

    if (texture.arrayLayers > 1) {
        switch (texture.textureType) {
            case TEXTURE_2D:
                return prefix + "sampler2DArray";
            case TEXTURE_2D_MULTISAMPLE:
                return prefix + "sampler2DMSArray";
            case TEXTURE_CUBE_MAP:
                return prefix + "samplerCubeArray";
            default:
                throw std::runtime_error("Unrecognized texture type");
        }
    } else {
        switch (texture.textureType) {
            case TEXTURE_2D:
                return prefix + "sampler2D";
            case TEXTURE_2D_MULTISAMPLE:
                return prefix + "sampler2DMS";
            case TEXTURE_CUBE_MAP:
                return prefix + "samplerCube";
            default:
                throw std::runtime_error("Unrecognized texture type");
        }
    }
}

std::string generateElement(const std::string &name, const FGShaderValue &value, std::string prefix = "\t") {
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

std::string generateHeader(const FGShaderSource &source, CompiledPipeline &pipeline) {
    std::string ret;

    for (const auto &pair: source.buffers) {
        auto binding = pipeline.getBufferBinding(pair.first);

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

    for (const auto &pair: source.textures) {
        const auto location = pipeline.getTextureBinding(pair.first);
        ret += "layout(binding = "
                + std::to_string(location)
                + ") uniform "
                + getSampler(pair.second)
                + " "
                + texturePrefix
                + pair.first
                + ";\n";
    }

    if (source.textures.size() > 0) {
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
    ret += outputAttributes;
    ret += "\n";

    return ret;
}

std::string generateBody(const FGShaderSource &source) {
    std::string body;
    for (const auto &pair: source.functions) {
        body += compileFunction(pair.first, pair.second.arguments, pair.second.body, pair.second.returnType, source);
        body += "\n\n";
    }
    body += compileFunction("main", {}, source.mainFunction, {}, source);
    return body;
}

CompiledPipeline ShaderCompilerGLSL::compile(const std::vector<FGShaderSource> &sources) {
    CompiledPipeline ret;
    for (auto &shader: sources) {
        ret.sourceCode[shader.stage] = compileShader(shader, ret);
    }
    return ret;
}

std::string ShaderCompilerGLSL::compileShader(const FGShaderSource &source, CompiledPipeline &pipeline) {
    return "#version 460\n\n"
           + generateHeader(source, pipeline)
           + generateBody(source);
}
