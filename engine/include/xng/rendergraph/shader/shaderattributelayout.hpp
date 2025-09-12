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

#ifndef XENGINE_SHADERATTRIBUTELAYOUT_HPP
#define XENGINE_SHADERATTRIBUTELAYOUT_HPP

#include <vector>

#include "xng/rendergraph/shader/shaderdatatype.hpp"

namespace xng {
    struct ShaderAttributeLayout {
        std::vector<ShaderDataType> elements;

        ShaderAttributeLayout() = default;

        explicit ShaderAttributeLayout(std::vector<ShaderDataType> elements) : elements(std::move(elements)) {}

        bool operator==(const ShaderAttributeLayout &other) const{
            return elements == other.elements;
        }

        bool operator!=(const ShaderAttributeLayout &other) const{
            return !(*this == other);
        }

        size_t getSize() const {
            size_t ret = 0;
            for (auto &attr : elements)
                ret += attr.stride();
            return ret;
        }
    };
}

#endif //XENGINE_SHADERATTRIBUTELAYOUT_HPP
