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

        // Vector 2 specifying the x / y coordinates for 2d textures
        // Vector 3 specifying a direction for cube-maps
        // Vector 3 specifying x / y coordinates and the array layer in z for 2d array textures
        // Vector 4 specifying x / y / z direction and the array layer in w for cube map array textures
        FGShaderNodeInput coordinate = FGShaderNodeInput("coordinate");
        FGShaderNodeInput bias = FGShaderNodeInput("bias");

        // Vector 4 specifying r/g/b/a
        FGShaderNodeOutput color = FGShaderNodeOutput("color");

        explicit FGNodeTextureSample(std::string texture_name)
            : textureName(std::move(texture_name)) {
        }

        NodeType getType() override {
            return TEXTURE_SAMPLE;
        }

        std::vector<std::reference_wrapper<FGShaderNodeInput>> getInputs() override {
            return {coordinate, bias};
        }

        std::vector<std::reference_wrapper<FGShaderNodeOutput>> getOutputs() override {
            return {color};
        }

        FGShaderValue getOutputType(const FGShaderSource &source) const override {
            return {FGShaderValue::VECTOR4, FGShaderValue::FLOAT, 1};
        }
    };
}

#endif //XENGINE_FGNODETEXTURESAMPLE_HPP
