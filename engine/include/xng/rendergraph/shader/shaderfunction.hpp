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

#ifndef XENGINE_RENDERGRAPH_SHADERFUNCTION_HPP
#define XENGINE_RENDERGRAPH_SHADERFUNCTION_HPP

#include <string>
#include <utility>
#include <vector>
#include <memory>
#include <optional>
#include <variant>

#include "xng/rendergraph/shader/shaderinstruction.hpp"
#include "xng/rendergraph/shader/shaderdatatype.hpp"
#include "xng/rendergraph/shader/shadertexture.hpp"

namespace xng::rg {
    struct ShaderFunction {
        struct Argument {
            std::string name;
            std::variant<ShaderDataType, ShaderTexture> type;
            bool isOut;

            Argument() = default;

            Argument(const std::variant<ShaderDataType, ShaderTexture> &type, std::string name, const bool isOut)
                : name(std::move(name)), type(type), isOut(isOut) {
            }

            bool operator==(const Argument &other) const {
                return name == other.name && type == other.type && isOut == other.isOut;
            }
        };

        std::string name{};
        std::vector<Argument> arguments{};
        std::vector<ShaderInstruction> body{};
        std::optional<ShaderDataType> returnType{};

        std::variant<ShaderDataType, ShaderTexture> getArgumentType(const std::string &argName) const {
            for (auto &arg: arguments) {
                if (arg.name == argName) {
                    return arg.type;
                }
            }
            throw std::runtime_error("Argument not found");
        }

        ShaderFunction() = default;

        ShaderFunction(std::string name,
                       std::vector<Argument> arguments,
                       std::vector<ShaderInstruction> body,
                       ShaderDataType returnType)
            : name(std::move(name)),
              arguments(std::move(arguments)),
              body(std::move(body)),
              returnType(std::move(returnType)) {
        }

        ShaderFunction(std::string name,
                       std::vector<Argument> arguments,
                       std::vector<ShaderInstruction> body)
            : name(std::move(name)),
              arguments(std::move(arguments)),
              body(std::move(body)) {
        }

        ShaderFunction(const ShaderFunction &other) = default;

        ShaderFunction &operator=(const ShaderFunction &other) = default;

        ShaderFunction(ShaderFunction &&other) noexcept = default;

        ShaderFunction &operator=(ShaderFunction &&other) noexcept = default;

        bool operator==(const ShaderFunction &other) const {
            return name == other.name && arguments == other.arguments && body == other.body && returnType == other.returnType;
        }
    };
}

#endif //XENGINE_RENDERGRAPH_SHADERFUNCTION_HPP
