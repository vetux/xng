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

#ifndef XENGINE_FGBUFFERLAYOUT_HPP
#define XENGINE_FGBUFFERLAYOUT_HPP

#include <vector>

#include "xng/render/graph2/buffer/fgbufferelement.hpp"

namespace xng {
    struct FGBufferLayout {
        std::vector<FGBufferElement> elements;

        FGBufferLayout() = default;

        explicit FGBufferLayout(std::vector<FGBufferElement> elements) : elements(std::move(elements)) {}

        bool operator==(const FGBufferLayout &other) const = default;

        size_t getSize() const {
            size_t ret = 0;
            for (auto &attr : elements)
                ret += attr.stride();
            return ret;
        }
    };
}

#endif //XENGINE_FGBUFFERLAYOUT_HPP
