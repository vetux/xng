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

#ifndef XENGINE_FGNODETEXTURESAMPLE_HPP
#define XENGINE_FGNODETEXTUREREAD_HPP

#include <utility>

#include "xng/render/graph2/shader/fgshadernode.hpp"

namespace xng {
    struct FGNodeTextureSample final : FGShaderNode {
        std::string textureName;

        std::unique_ptr<FGShaderNode> coordinate;
        std::unique_ptr<FGShaderNode> bias;

        explicit FGNodeTextureSample(std::string texture_name,
                                     std::unique_ptr<FGShaderNode> coordinate,
                                     std::unique_ptr<FGShaderNode> bias)
            : textureName(std::move(texture_name)), coordinate(std::move(coordinate)), bias(std::move(bias)) {
        }

        NodeType getType() const override {
            return TEXTURE_SAMPLE;
        }

        std::unique_ptr<FGShaderNode> copy() const override {
            return std::make_unique<FGNodeTextureSample>(textureName, coordinate->copy(), bias->copy());
        }
    };
}

#endif //XENGINE_FGNODETEXTURESAMPLE_HPP
