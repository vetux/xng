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

#ifndef XENGINE_FGSHADERBUFFER_HPP
#define XENGINE_FGSHADERBUFFER_HPP

#include <utility>
#include <vector>

#include "fgshadervalue.hpp"

namespace xng {
    struct FGShaderBuffer {
        struct Element {
            std::string name; // The name of this element. must be unique inside the buffer.
            FGShaderValue value;

            // If larger than 1, this element is a fixed size array, if 0, the element is a dynamic array and must be the only element in this buffer.
            size_t count = 1;

            Element(std::string name, const FGShaderValue &value, const size_t count = 1)
                : name(std::move(name)),
                  value(value),
                  count(count) {
            }
        };

        bool readOnly = true; // Whether shaders are allowed to write to the buffer
        std::vector<Element> elements;
    };
}

#endif //XENGINE_FGSHADERBUFFER_HPP
