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

#ifndef XENGINE_RENDERGRAPH_SHADERDATATYPE_HPP
#define XENGINE_RENDERGRAPH_SHADERDATATYPE_HPP

#include "xng/rendergraph/shader/shaderprimitive.hpp"

namespace xng::rg {
    typedef std::string ShaderStructTypeName; // The name of the struct type

    struct ShaderDataType {
        std::variant<ShaderPrimitiveType, ShaderStructTypeName> value{};
        size_t count = 1; // If larger than 1, this type is a fixed size array

        ShaderDataType() = default;

        ShaderDataType(const ShaderStructTypeName &type, const size_t count = 1)
            : value(type), count(count) {
        }

        ShaderDataType(const ShaderPrimitiveType &type, const size_t count = 1)
            : value(type), count(count) {
        }

        bool operator==(const ShaderDataType &other) const {
            return value == other.value && count == other.count;
        }

        bool operator!=(const ShaderDataType &other) const {
            return !(*this == other);
        }

        const ShaderPrimitiveType &getPrimitive() const {
            return std::get<ShaderPrimitiveType>(value);
        }

        const ShaderStructTypeName &getStruct() const {
            return std::get<ShaderStructTypeName>(value);
        }
    };
}

#endif //XENGINE_RENDERGRAPH_SHADERDATATYPE_HPP
