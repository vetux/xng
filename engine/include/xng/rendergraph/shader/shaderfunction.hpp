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

#ifndef XENGINE_SHADERFUNCTION_HPP
#define XENGINE_SHADERFUNCTION_HPP

#include <string>
#include <utility>
#include <vector>
#include <memory>
#include <unordered_map>

#include "xng/rendergraph/shader/shadernode.hpp"
#include "xng/rendergraph/shader/shaderdatatype.hpp"

namespace xng {
    struct ShaderFunction {
        std::string name;
        std::unordered_map<std::string, ShaderDataType> arguments;
        std::vector<std::unique_ptr<ShaderNode> > body;
        ShaderDataType returnType;

        ShaderFunction() = default;

        ShaderFunction(std::string name,
                         const std::unordered_map<std::string, ShaderDataType> &arguments,
                         std::vector<std::unique_ptr<ShaderNode> > body,
                         const ShaderDataType returnType)
            : name(std::move(name)),
              arguments(arguments),
              body(std::move(body)),
              returnType(returnType) {
        }

        ShaderFunction(const ShaderFunction &other)
            : name(other.name),
              arguments(other.arguments),
              returnType(other.returnType) {
            for (auto &node: other.body) {
                body.emplace_back(node->copy());
            }
        }

        ShaderFunction(ShaderFunction &&other) noexcept
            : name(std::move(other.name)),
              arguments(std::move(other.arguments)),
              returnType(other.returnType),
              body(std::move(other.body)) {
        }

        ShaderFunction &operator=(const ShaderFunction &other) {
            name = other.name;
            arguments = other.arguments;
            returnType = other.returnType;
            body.clear();
            for (auto &node: other.body) {
                body.emplace_back(node->copy());
            }
            return *this;
        }

        ShaderFunction &operator=(ShaderFunction &&other) noexcept {
            if (this == &other)
                return *this;
            name = std::move(other.name);
            arguments = std::move(other.arguments);
            body = std::move(other.body);
            returnType = other.returnType;
            return *this;
        }
    };
}

#endif //XENGINE_SHADERFUNCTION_HPP
