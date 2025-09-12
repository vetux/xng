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

#ifndef XENGINE_FGNODESUBSCRIPTARRAY_HPP
#define XENGINE_FGNODESUBSCRIPTARRAY_HPP

#include "xng/rendergraph/shader/fgshadernode.hpp"

namespace xng {
    struct FGNodeSubscriptArray final : FGShaderNode {
        std::unique_ptr<FGShaderNode> array;
        std::unique_ptr<FGShaderNode> index;

        FGNodeSubscriptArray(std::unique_ptr<FGShaderNode> array, std::unique_ptr<FGShaderNode> index)
            : array(std::move(array)),
              index(std::move(index)) {
        }

        NodeType getType() const override {
            return SUBSCRIPT_ARRAY;
        }

        std::unique_ptr<FGShaderNode> copy() const override {
            return std::make_unique<FGNodeSubscriptArray>(array->copy(), index->copy());
        }
    };
}
#endif //XENGINE_FGNODESUBSCRIPTARRAY_HPP