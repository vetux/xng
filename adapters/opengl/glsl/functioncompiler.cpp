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

#include "functioncompiler.hpp"

#include "types.hpp"
#include "nodecompiler.hpp"

std::string compileFunction(const std::string &functionName,
                            const std::vector<ShaderFunction::Argument> &parameters,
                            const std::vector<std::unique_ptr<ShaderNode> > &body,
                            const std::optional<ShaderDataType> &returnType,
                            const Shader &source,
                            const std::string &appendix) {
    std::string ret;
    if (returnType.has_value()) {
        ret += getTypeName(returnType.value());
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
        if (param.type.index() == 0) {
            auto arg = std::get<ShaderDataType>(param.type);
            ret += getTypeName(arg) + " " + param.name;
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
    for (const auto &node: body) {
        ret += compileNode(*node, source, nodeFuncName, "\t");
        ret += ";\n";
    }
    if (appendix.size() > 0) {
        ret += "\t" + appendix + ";\n";
    }
    ret += "}";
    return ret;
}
