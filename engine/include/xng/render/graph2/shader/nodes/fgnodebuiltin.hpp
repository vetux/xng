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

#ifndef XENGINE_FGNODEBUILTIN_HPP
#define XENGINE_FGNODEBUILTIN_HPP

#include "xng/render/graph2/shader/fgshadernode.hpp"

namespace xng {
    /**
     * The runtime implementation must respect the format of builtins defined by the shader builder.
     */
    struct FGNodeBuiltin final : FGShaderNode {
        enum BuiltInType {
            ABS,
            SIN,
            COS,
            TAN,
            ASIN,
            ACOS,
            ATAN,
            POW,
            EXP,
            LOG,
            SQRT,
            INVERSESQRT,
            FLOOR,
            CEIL,
            ROUND,
            FRACT,
            MOD,
            MIN,
            MAX,
            CLAMP,
            MIX,
            STEP,
            SMOOTHSTEP,

            DOT,
            CROSS,
            NORMALIZE,
            LENGTH,
            DISTANCE,
            REFLECT,
            REFRACT,
            FACEFORWARD,
        } type;

        std::unique_ptr<FGShaderNode> valA;
        std::unique_ptr<FGShaderNode> valB;
        std::unique_ptr<FGShaderNode> valC;

        FGNodeBuiltin(const BuiltInType type,
                      std::unique_ptr<FGShaderNode> val_a,
                      std::unique_ptr<FGShaderNode> val_b,
                      std::unique_ptr<FGShaderNode> val_c)
            : type(type),
              valA(std::move(val_a)),
              valB(std::move(val_b)),
              valC(std::move(val_c)) {
        }

        NodeType getType() const override {
            return BUILTIN;
        }

        std::unique_ptr<FGShaderNode> copy() const override {
            return std::make_unique<FGNodeBuiltin>(type,
                                                   valA->copy(),
                                                   valB ? valB->copy() : nullptr,
                                                   valC ? valC->copy() : nullptr);
        }
    };
}

#endif //XENGINE_FGNODEBUILTIN_HPP
