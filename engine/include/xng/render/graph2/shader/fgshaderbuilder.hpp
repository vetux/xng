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

#ifndef XENGINE_FGSHADERBUILDER_HPP
#define XENGINE_FGSHADERBUILDER_HPP

#include <cstdint>

#include "xng/render/graph2/shader/fgshadersource.hpp"
#include "xng/render/graph2/shader/fgvertexlayout.hpp"

namespace xng {
    class FGShaderBuilder {
    public:
        /**
         * Read a value from a binding that was written to by a previous shader stage or the pipeline (e.g. in a vertex shader).
         *
         * @param binding
         * @return
         */
        FGShaderValue read(uint32_t binding);

        /**
         * Write a value to a binding that will be read by a subsequent shader stage or the pipeline (e.g. in a fragment shader).
         *
         * @param binding
         * @param value
         */
        void write(uint32_t binding, const FGShaderValue &value);

        // Retrieve or Write to a bound value (Shader Buffer, Push Constants, etc.)
        FGShaderValue readShaderValue(const std::string &name);
        void writeShaderValue(const std::string &name, const FGShaderValue &value);

        FGShaderValue literal(unsigned char value);

        // Math
        FGShaderValue add(const FGShaderValue &valA, const FGShaderValue &valB);
        FGShaderValue subtract(const FGShaderValue &valA, const FGShaderValue &valB);
        FGShaderValue multiply(const FGShaderValue &valA, const FGShaderValue &valB);
        FGShaderValue divide(const FGShaderValue &valA, const FGShaderValue &valB);

        // Comparison
        FGShaderValue equal(const FGShaderValue &valA, const FGShaderValue &valB);
        FGShaderValue larger(const FGShaderValue &valA, const FGShaderValue &valB);
        FGShaderValue smaller(const FGShaderValue &valA, const FGShaderValue &valB);
        FGShaderValue largerOrEqual(const FGShaderValue &valA, const FGShaderValue &valB);
        FGShaderValue smallerOrEqual(const FGShaderValue &valA, const FGShaderValue &valB);

        // Logical
        FGShaderValue logicalAnd(const FGShaderValue &valA, const FGShaderValue &valB);
        FGShaderValue logicalOr(const FGShaderValue &valA, const FGShaderValue &valB);

        // Built-In Functions
        FGShaderValue normalize(const FGShaderValue &value);
        FGShaderValue transpose(const FGShaderValue &value);
        FGShaderValue inverse(const FGShaderValue &value);

        // Texture Sampling
        FGShaderValue sample(const FGShaderValue &texture,
                             const FGShaderValue &x,
                             const FGShaderValue &y,
                             const FGShaderValue &z,
                             const FGShaderValue &bias);

        // Subscripting
        FGShaderValue getX(const FGShaderValue &val) {
            return subscript(val, 0);
        }

        FGShaderValue getY(const FGShaderValue &val) {
            return subscript(val, 1);
        }

        FGShaderValue getZ(const FGShaderValue &val) {
            return subscript(val, 2);
        }

        FGShaderValue getW(const FGShaderValue &val) {
            return subscript(val, 3);
        }

        // Array Or Vector
        FGShaderValue subscript(const FGShaderValue &val, uint32_t index);

        // Matrix
        FGShaderValue subscript(const FGShaderValue &val, uint32_t row, uint32_t column);

        // Conditional
        void conditional_begin(const FGShaderValue &predicate);
        void conditional_else();
        void conditional_end();

        // Loop
        void loop_begin();
        void loop_predicate();
        void loop_iterator();
        void loop_body();
        void loop_end();

        FGShaderSource build(FGShaderSource::ShaderStage stage,
                       const FGVertexLayout &inputLayout,
                       const FGVertexLayout &outputLayout);
    };
}

#endif //XENGINE_FGSHADERBUILDER_HPP
