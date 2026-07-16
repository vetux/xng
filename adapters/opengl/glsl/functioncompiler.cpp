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

#include "functioncompiler.hpp"

#include "types.hpp"
#include "instructioncompiler.hpp"

std::string compileFunction(const std::string &functionName,
                            const std::vector<ShaderFunction::Argument> &parameters,
                            const std::vector<ShaderInstruction> &body,
                            const std::optional<ShaderDataType> &returnType,
                            const Shader &source,
                            const std::string &appendix) {
    std::string ret;
    if (returnType.has_value()) {
        if (std::holds_alternative<ShaderPrimitiveType>(returnType.value().value)) {
            ret += getTypeName(std::get<ShaderPrimitiveType>(returnType.value().value));
        } else {
            ret += std::get<ShaderStructTypeName>(returnType.value().value);
        }
    } else {
        ret += "void";
    }
    ret += " " + functionName + "(";

    size_t paramCount = 0;
    for (const auto &param: parameters) {
        if (paramCount > 0) {
            ret += ", ";
        }
        paramCount++;
        if (std::holds_alternative<ShaderDataType>(param.type)) {
            if (param.isOut) {
                ret += "out ";
            }
            auto arg = std::get<ShaderDataType>(param.type);
            if (std::holds_alternative<ShaderPrimitiveType>(arg.value)) {
                ret += getTypeName(std::get<ShaderPrimitiveType>(arg.value)) + " " + param.name;
            } else {
                ret += std::get<ShaderStructTypeName>(arg.value) + " " + param.name;
            }
        } else {
            auto arg = std::get<ShaderTexture>(param.type);
            ret += getSampler(arg) + " " + param.name;
        }
    }

    ret += ") {\n";

    auto nodeFuncName = functionName;
    if (functionName == "main") {
        nodeFuncName = "";
    }
    for (const auto &instruction: body) {
        ret += InstructionCompiler::compile(instruction, source, nodeFuncName, "\t") + ";\n";
    }
    if (appendix.size() > 0) {
        ret += "\t" + appendix + ";\n";
    }
    ret += "}";
    return ret;
}
