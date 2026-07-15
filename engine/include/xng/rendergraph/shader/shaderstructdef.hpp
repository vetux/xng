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

#ifndef XENGINE_RENDERGRAPH_SHADERSTRUCTDEF_HPP
#define XENGINE_RENDERGRAPH_SHADERSTRUCTDEF_HPP

#include "xng/rendergraph/shader/shaderdatatype.hpp"

namespace xng::rg {
    struct ShaderStructElement;

    struct ShaderStructDef {
        std::string typeName;
        std::vector<ShaderStructElement> elements;

        ShaderStructDef() = default;

        ShaderStructDef(std::string name, const std::vector<ShaderStructElement> &elements)
            : typeName(std::move(name)), elements(elements) {
        }

        const ShaderStructElement &get(const std::string &name) const;

        bool operator==(const ShaderStructDef &o) const {
            return typeName == o.typeName && elements == o.elements;
        }
    };

    struct ShaderStructElement {
        ShaderDataType type; // The type of this element.
        std::string name; // The name of this element. must be unique inside the structure.

        ShaderStructElement() = default;

        ShaderStructElement(ShaderDataType type, std::string name)
            : type(std::move(type)),
              name(std::move(name)) {
        }

        bool operator==(const ShaderStructElement &o) const {
            return type == o.type && name == o.name;
        }
    };
}

#endif //XENGINE_RENDERGRAPH_SHADERSTRUCTDEF_HPP
