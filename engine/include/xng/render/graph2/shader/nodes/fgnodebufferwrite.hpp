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

#ifndef XENGINE_FGNODEBUFFERWRITE_HPP
#define XENGINE_FGNODEBUFFERWRITE_HPP

#include "xng/render/graph2/shader/fgshadernode.hpp"

namespace xng {
    struct FGNodeBufferWrite final : FGShaderNode {
        std::string bufferName;
        std::string elementName;

        FGShaderNodeInput value = FGShaderNodeInput("value");

        NodeType getType() override {
            return BUFFER_WRITE;
        }

        const std::vector<FGShaderNodeInput> &getInputs() override {
            return {value};
        }

        const std::vector<FGShaderNodeOutput> &getOutputs() override {
            return {};
        }
    };
}

#endif //XENGINE_FGNODEBUFFERWRITE_HPP