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

#ifndef XENGINE_FGNODEBUFFERSIZE_HPP
#define XENGINE_FGNODEBUFFERSIZE_HPP

#include "xng/rendergraph/shader/fgshadernode.hpp"

namespace xng {
    /**
     * Retrieve the number of element collections in a dynamic buffer
     */
    struct FGNodeBufferSize final : FGShaderNode {
        std::string bufferName;

        explicit FGNodeBufferSize(std::string buffer_name)
            : bufferName(std::move(buffer_name)) {
        }

        NodeType getType() const override {
            return BUFFER_SIZE;
        }

        std::unique_ptr<FGShaderNode> copy() const override {
            return std::make_unique<FGNodeBufferSize>(bufferName);
        }
    };
}
#endif //XENGINE_FGNODEBUFFERSIZE_HPP
