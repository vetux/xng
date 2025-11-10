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

#ifndef XENGINE_SHADERSTRUCT_HPP
#define XENGINE_SHADERSTRUCT_HPP

#include "xng/rendergraph/shader/shaderdatatype.hpp"

namespace xng {
    struct ShaderStructElement;

    struct ShaderStruct {
        std::string typeName;
        std::vector<ShaderStructElement> elements;

        ShaderStruct(std::string name, const std::vector<ShaderStructElement> &elements)
            : typeName(std::move(name)), elements(elements) {
        }

        const ShaderStructElement &get(const std::string &name) const;
    };

    struct ShaderStructElement {
        ShaderDataType type; // The type of this element.
        std::string name; // The name of this element. must be unique inside the structure.

        ShaderStructElement() = default;

        ShaderStructElement(ShaderDataType type, std::string name)
            : type(std::move(type)),
              name(std::move(name)) {
        }
    };
}

#endif //XENGINE_SHADERSTRUCT_HPP
