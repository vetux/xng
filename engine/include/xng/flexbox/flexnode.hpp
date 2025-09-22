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

#ifndef XENGINE_FLEXNODE_HPP
#define XENGINE_FLEXNODE_HPP

#include "xng/math/vector2.hpp"

#include "xng/flexbox/flexlayout.hpp"

namespace xng {
    struct FlexNode;

    struct FlexNode {
        FlexLayout layout;

        std::vector<FlexNode> children;

        Vec2i calculatedPosition; // The calculated position relative to the parent
        Vec2i calculatedSize; // The calculated absolute size
    };
}

#endif //XENGINE_FLEXNODE_HPP
