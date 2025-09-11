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

#ifndef XENGINE_FGNODEBUFFERREAD_HPP
#define XENGINE_FGNODEBUFFERREAD_HPP

#include "xng/render/graph2/shader/fgshadernode.hpp"
#include "xng/render/graph2/shader/fgshadersource.hpp"

namespace xng {
    struct FGNodeBufferRead final : FGShaderNode {
        std::string bufferName;
        std::string elementName;

        /**
         * Specify the index of the elements if this buffer is defined as dynamic.
         * Unused for static buffers.
         */
        std::unique_ptr<FGShaderNode> index;

        explicit FGNodeBufferRead(std::string buffer_name,
                                  std::string element_name,
                                  std::unique_ptr<FGShaderNode> index)
            : bufferName(std::move(buffer_name)), elementName(std::move(element_name)), index(std::move(index)) {
        }

        NodeType getType() const override {
            return BUFFER_READ;
        }

        std::unique_ptr<FGShaderNode> copy() const override {
            return std::make_unique<FGNodeBufferRead>(bufferName, elementName, index ? index->copy() : nullptr);
        }
    };
}

#endif //XENGINE_FGNODEBUFFERREAD_HPP
