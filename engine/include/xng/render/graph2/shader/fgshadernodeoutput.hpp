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

#ifndef XENGINE_FGSHADERNODEOUTPUT_HPP
#define XENGINE_FGSHADERNODEOUTPUT_HPP

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace xng {
    struct FGShaderNode;

    struct FGShaderNodeOutput {
        std::string name;
        std::vector<std::shared_ptr<FGShaderNode> > consumers{};

        FGShaderNodeOutput() = default;

        explicit FGShaderNodeOutput(std::string name)
            : name(std::move(name)) {
        }
    };
}

#endif //XENGINE_FGSHADERNODEOUTPUT_HPP
