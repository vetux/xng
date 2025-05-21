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
        FGShaderVariable readVertex(uint32_t binding);

        /**
         * Write a value to a binding that will be read by a subsequent shader stage or the pipeline (e.g. in a fragment shader).
         *
         * @param binding
         * @param value
         */
        void writeVertex(uint32_t binding, const FGShaderVariable &value);

        /**
         * Read from a bound parameter (Implemented as Shader Storage Buffer, Push Constants, etc.)
         *
         * @param name
         * @return
         */
        FGShaderVariable readParameter(const std::string &name);

        void writeParameter(const std::string &name, const FGShaderVariable &value);

        FGShaderVariable literal(const FGShaderValue &value);

        FGShaderVariable array(size_t size);

        void assign(const FGShaderVariable &target, const FGShaderVariable &source);

        // Math
        FGShaderVariable add(const FGShaderVariable &valA, const FGShaderVariable &valB);

        FGShaderVariable subtract(const FGShaderVariable &valA, const FGShaderVariable &valB);

        FGShaderVariable multiply(const FGShaderVariable &valA, const FGShaderVariable &valB);

        FGShaderVariable divide(const FGShaderVariable &valA, const FGShaderVariable &valB);

        // Comparison
        FGShaderVariable equal(const FGShaderVariable &valA, const FGShaderVariable &valB);

        FGShaderVariable larger(const FGShaderVariable &valA, const FGShaderVariable &valB);

        FGShaderVariable smaller(const FGShaderVariable &valA, const FGShaderVariable &valB);

        FGShaderVariable largerOrEqual(const FGShaderVariable &valA, const FGShaderVariable &valB);

        FGShaderVariable smallerOrEqual(const FGShaderVariable &valA, const FGShaderVariable &valB);

        // Logical
        FGShaderVariable logicalAnd(const FGShaderVariable &valA, const FGShaderVariable &valB);

        FGShaderVariable logicalOr(const FGShaderVariable &valA, const FGShaderVariable &valB);

        // Built-In Functions
        FGShaderVariable normalize(const FGShaderVariable &value);

        FGShaderVariable transpose(const FGShaderVariable &value);

        FGShaderVariable inverse(const FGShaderVariable &value);

        // Texture Sampling
        FGShaderVariable sample(const FGShaderVariable &texture,
                                const FGShaderVariable &x,
                                const FGShaderVariable &y,
                                const FGShaderVariable &z,
                                const FGShaderVariable &bias);

        // Subscripting
        FGShaderVariable getX(const FGShaderVariable &val) {
            return subscript(val, 0);
        }

        FGShaderVariable getY(const FGShaderVariable &val) {
            return subscript(val, 1);
        }

        FGShaderVariable getZ(const FGShaderVariable &val) {
            return subscript(val, 2);
        }

        FGShaderVariable getW(const FGShaderVariable &val) {
            return subscript(val, 3);
        }

        // Array Or Vector
        FGShaderVariable subscript(const FGShaderVariable &val, uint32_t index);

        // Matrix
        FGShaderVariable subscript(const FGShaderVariable &val, uint32_t row, uint32_t column);

        // Conditional
        void conditional_begin(const FGShaderVariable &predicate);

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
