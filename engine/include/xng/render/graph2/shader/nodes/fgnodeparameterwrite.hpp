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

#ifndef XENGINE_FGNODEPARAMETERWRITE_HPP
#define XENGINE_FGNODEPARAMETERWRITE_HPP

#include "xng/render/graph2/shader/fgshadernode.hpp"

namespace xng {
    struct FGNodeParameterWrite final : FGShaderNode {
        std::string parameterName;

        FGShaderNodeOutput output = FGShaderNodeOutput("output");

        explicit FGNodeParameterWrite(std::string parameter_name)
            : parameterName(std::move(parameter_name)) {
        }

        NodeType getType() override {
            return PARAMETER_WRITE;
        }

        const std::vector<FGShaderNodeInput> &getInputs() override {
            return {};
        }

        const std::vector<FGShaderNodeOutput> &getOutputs() override {
            return {output};
        }
    };
}
#endif //XENGINE_FGNODEPARAMETERWRITE_HPP
