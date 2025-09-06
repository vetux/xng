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
#include "xng/render/graph2/shader/fgattributelayout.hpp"
#include "xng/render/graph2/shader/fgshadervalue.hpp"

namespace xng {
    class FGShaderBuilder {
    public:
        /**
         * Read a value from an attribute that was written to by a previous shader stage or the pipeline (e.g. in a vertex shader).
         *
         * @param binding
         * @return
         */
        void readAttribute(uint32_t binding, const FGShaderVariable &output);

        /**
         * Write a value to an attribute that will be read by a subsequent shader stage or the pipeline (e.g. in a fragment shader).
         *
         * @param binding
         * @param value
         */
        void writeAttribute(uint32_t binding, const FGShaderValue &value);

        /**
         * Read from a bound parameter (Implemented as Shader Storage Buffer, Push Constants, etc.)
         *
         * @param name
         * @return
         */
        void readParameter(const std::string &name, const FGShaderVariable &output);

        void writeParameter(const std::string &name, const FGShaderValue &value);

        void readTexture(const std::string &name,
                         const FGShaderValue &x,
                         const FGShaderValue &y,
                         const FGShaderValue &z,
                         const FGShaderValue &bias,
                         const FGShaderVariable &output);

        void writeTexture(const std::string &name,
                          const FGShaderValue &x,
                          const FGShaderValue &y,
                          const FGShaderValue &z,
                          const FGShaderValue &bias,
                          const FGShaderValue &color);

        FGShaderVariable variable(const std::string &name,
                                  FGShaderVariable::Type type,
                                  FGShaderVariable::Component component,
                                  const FGShaderLiteral &value = {});

        FGShaderVariable array(size_t size);

        void assign(const FGShaderVariable &target, const FGShaderValue &source);

        // Math
        void add(const FGShaderValue &valA, const FGShaderValue &valB, const FGShaderVariable &output);

        void subtract(const FGShaderValue &valA, const FGShaderValue &valB, const FGShaderVariable &output);

        void multiply(const FGShaderValue &valA, const FGShaderValue &valB, const FGShaderVariable &output);

        void divide(const FGShaderValue &valA, const FGShaderValue &valB, const FGShaderVariable &output);

        // Comparison
        void equal(const FGShaderValue &valA, const FGShaderValue &valB, const FGShaderVariable &output);

        void larger(const FGShaderValue &valA, const FGShaderValue &valB, const FGShaderVariable &output);

        void smaller(const FGShaderValue &valA, const FGShaderValue &valB, const FGShaderVariable &output);

        void largerOrEqual(const FGShaderValue &valA, const FGShaderValue &valB, const FGShaderVariable &output);

        void smallerOrEqual(const FGShaderValue &valA, const FGShaderValue &valB, const FGShaderVariable &output);

        // Logical
        void logicalAnd(const FGShaderValue &valA, const FGShaderValue &valB, const FGShaderVariable &output);

        void logicalOr(const FGShaderValue &valA, const FGShaderValue &valB, const FGShaderVariable &output);

        // Function call
        void callFunction(const std::string &functionName,
                          const std::vector<FGShaderValue> &arguments,
                          const FGShaderVariable &output);

        void returnFunction(const FGShaderValue &output);

        // Subscripting
        FGShaderVariable getX(const FGShaderValue &val) {
            return subscript(val, 0);
        }

        FGShaderVariable getY(const FGShaderValue &val) {
            return subscript(val, 1);
        }

        FGShaderVariable getZ(const FGShaderValue &val) {
            return subscript(val, 2);
        }

        FGShaderVariable getW(const FGShaderValue &val) {
            return subscript(val, 3);
        }

        // Array Or Vector
        FGShaderVariable subscript(const FGShaderValue &val, const FGShaderValue &index);

        // Matrix
        FGShaderVariable subscript(const FGShaderValue &val,
                                   const FGShaderVariable &row,
                                   const FGShaderValue &column);

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
                             const FGAttributeLayout &inputLayout,
                             const FGAttributeLayout &outputLayout);

    private:
        size_t variableIndex = 0;

        std::vector<FGShaderOperation> operations;
        std::map<std::string, std::vector<FGShaderOperation>> functions;
    };
}

#endif //XENGINE_FGSHADERBUILDER_HPP
