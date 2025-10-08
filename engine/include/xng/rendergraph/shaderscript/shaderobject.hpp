/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
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

#ifndef XENGINE_ShaderObject_HPP
#define XENGINE_ShaderObject_HPP

#include <compare>

#include "xng/rendergraph/shader/shaderinstruction.hpp"
#include "xng/rendergraph/shaderscript/shaderbuilder.hpp"

#define _SWIZZLE2(x, y) [[nodiscard]] ShaderObject x##y() { return swizzle_vec2(ShaderInstruction::COMPONENT_##x, ShaderInstruction::COMPONENT_##y); }
#define _SWIZZLE3(x, y, z) [[nodiscard]] ShaderObject x##y##z() { return swizzle_vec3(ShaderInstruction::COMPONENT_##x, ShaderInstruction::COMPONENT_##y, ShaderInstruction::COMPONENT_##z); }
#define _SWIZZLE4(x, y, z, w) [[nodiscard]] ShaderObject x##y##z##w() { return swizzle_vec4(ShaderInstruction::COMPONENT_##x, ShaderInstruction::COMPONENT_##y, ShaderInstruction::COMPONENT_##z, ShaderInstruction::COMPONENT_##w); }

namespace xng::ShaderScript {
    class ShaderObject {
    public:
        ShaderOperand operand;
        bool assignable = false;

        ShaderObject() = default;

        ShaderObject(const ShaderInstruction &instruction, const bool assignable = false)
            : operand(instruction), assignable(assignable) {
        }

        ShaderObject(ShaderOperand operand, const bool assignable = false)
            : operand(std::move(operand)), assignable(assignable) {
        }

        ShaderObject(const ShaderObject &other) = default;

        ShaderObject(ShaderObject &&other) noexcept = default;

        // Literal constructor
        ShaderObject(const bool literal)
            : ShaderObject(ShaderOperand(literal)) {
        }

        ShaderObject(const int literal)
            : ShaderObject(ShaderOperand(literal)) {
        }

        ShaderObject(const unsigned int literal)
            : ShaderObject(ShaderOperand(literal)) {
        }

        ShaderObject(const float literal)
            : ShaderObject(ShaderOperand(literal)) {
        }

        ShaderObject(const double literal)
            : ShaderObject(ShaderOperand(literal)) {
        }

        [[nodiscard]] ShaderObject x() {
            return {
                ShaderInstructionFactory::vectorSwizzle(operand, {
                                                            ShaderInstruction::COMPONENT_x
                                                        }),
                assignable
            };
        }

        [[nodiscard]] ShaderObject y() {
            return {
                ShaderInstructionFactory::vectorSwizzle(operand, {
                                                            ShaderInstruction::COMPONENT_y
                                                        }),
                assignable
            };
        }

        [[nodiscard]] ShaderObject z() {
            return {
                ShaderInstructionFactory::vectorSwizzle(operand, {
                                                            ShaderInstruction::COMPONENT_z
                                                        }),
                assignable
            };
        }

        [[nodiscard]] ShaderObject w() {
            return {
                ShaderInstructionFactory::vectorSwizzle(operand, {
                                                            ShaderInstruction::COMPONENT_w
                                                        }),
                assignable
            };
        }

        // 4^2 = 16 Vector2 Swizzle Combinations (Macro calls generated with ChatGPT)
        _SWIZZLE2(x, x)
        _SWIZZLE2(x, y)
        _SWIZZLE2(x, z)
        _SWIZZLE2(x, w)
        _SWIZZLE2(y, x)
        _SWIZZLE2(y, y)
        _SWIZZLE2(y, z)
        _SWIZZLE2(y, w)
        _SWIZZLE2(z, x)
        _SWIZZLE2(z, y)
        _SWIZZLE2(z, z)
        _SWIZZLE2(z, w)
        _SWIZZLE2(w, x)
        _SWIZZLE2(w, y)
        _SWIZZLE2(w, z)
        _SWIZZLE2(w, w)

        // 4^3 = 64 Vector3 Swizzle Combinations (Macro calls generated with ChatGPT)
        _SWIZZLE3(x, x, x)
        _SWIZZLE3(x, x, y)
        _SWIZZLE3(x, x, z)
        _SWIZZLE3(x, x, w)
        _SWIZZLE3(x, y, x)
        _SWIZZLE3(x, y, y)
        _SWIZZLE3(x, y, z)
        _SWIZZLE3(x, y, w)
        _SWIZZLE3(x, z, x)
        _SWIZZLE3(x, z, y)
        _SWIZZLE3(x, z, z)
        _SWIZZLE3(x, z, w)
        _SWIZZLE3(x, w, x)
        _SWIZZLE3(x, w, y)
        _SWIZZLE3(x, w, z)
        _SWIZZLE3(x, w, w)
        _SWIZZLE3(y, x, x)
        _SWIZZLE3(y, x, y)
        _SWIZZLE3(y, x, z)
        _SWIZZLE3(y, x, w)
        _SWIZZLE3(y, y, x)
        _SWIZZLE3(y, y, y)
        _SWIZZLE3(y, y, z)
        _SWIZZLE3(y, y, w)
        _SWIZZLE3(y, z, x)
        _SWIZZLE3(y, z, y)
        _SWIZZLE3(y, z, z)
        _SWIZZLE3(y, z, w)
        _SWIZZLE3(y, w, x)
        _SWIZZLE3(y, w, y)
        _SWIZZLE3(y, w, z)
        _SWIZZLE3(y, w, w)
        _SWIZZLE3(z, x, x)
        _SWIZZLE3(z, x, y)
        _SWIZZLE3(z, x, z)
        _SWIZZLE3(z, x, w)
        _SWIZZLE3(z, y, x)
        _SWIZZLE3(z, y, y)
        _SWIZZLE3(z, y, z)
        _SWIZZLE3(z, y, w)
        _SWIZZLE3(z, z, x)
        _SWIZZLE3(z, z, y)
        _SWIZZLE3(z, z, z)
        _SWIZZLE3(z, z, w)
        _SWIZZLE3(z, w, x)
        _SWIZZLE3(z, w, y)
        _SWIZZLE3(z, w, z)
        _SWIZZLE3(z, w, w)
        _SWIZZLE3(w, x, x)
        _SWIZZLE3(w, x, y)
        _SWIZZLE3(w, x, z)
        _SWIZZLE3(w, x, w)
        _SWIZZLE3(w, y, x)
        _SWIZZLE3(w, y, y)
        _SWIZZLE3(w, y, z)
        _SWIZZLE3(w, y, w)
        _SWIZZLE3(w, z, x)
        _SWIZZLE3(w, z, y)
        _SWIZZLE3(w, z, z)
        _SWIZZLE3(w, z, w)
        _SWIZZLE3(w, w, x)
        _SWIZZLE3(w, w, y)
        _SWIZZLE3(w, w, z)
        _SWIZZLE3(w, w, w)

        // 4^4 = 256 Vector4 Swizzle Combinations (Macro calls generated with ChatGPT)
        _SWIZZLE4(x, x, x, x)
        _SWIZZLE4(x, x, x, y)
        _SWIZZLE4(x, x, x, z)
        _SWIZZLE4(x, x, x, w)
        _SWIZZLE4(x, x, y, x)
        _SWIZZLE4(x, x, y, y)
        _SWIZZLE4(x, x, y, z)
        _SWIZZLE4(x, x, y, w)
        _SWIZZLE4(x, x, z, x)
        _SWIZZLE4(x, x, z, y)
        _SWIZZLE4(x, x, z, z)
        _SWIZZLE4(x, x, z, w)
        _SWIZZLE4(x, x, w, x)
        _SWIZZLE4(x, x, w, y)
        _SWIZZLE4(x, x, w, z)
        _SWIZZLE4(x, x, w, w)
        _SWIZZLE4(x, y, x, x)
        _SWIZZLE4(x, y, x, y)
        _SWIZZLE4(x, y, x, z)
        _SWIZZLE4(x, y, x, w)
        _SWIZZLE4(x, y, y, x)
        _SWIZZLE4(x, y, y, y)
        _SWIZZLE4(x, y, y, z)
        _SWIZZLE4(x, y, y, w)
        _SWIZZLE4(x, y, z, x)
        _SWIZZLE4(x, y, z, y)
        _SWIZZLE4(x, y, z, z)
        _SWIZZLE4(x, y, z, w)
        _SWIZZLE4(x, y, w, x)
        _SWIZZLE4(x, y, w, y)
        _SWIZZLE4(x, y, w, z)
        _SWIZZLE4(x, y, w, w)
        _SWIZZLE4(x, z, x, x)
        _SWIZZLE4(x, z, x, y)
        _SWIZZLE4(x, z, x, z)
        _SWIZZLE4(x, z, x, w)
        _SWIZZLE4(x, z, y, x)
        _SWIZZLE4(x, z, y, y)
        _SWIZZLE4(x, z, y, z)
        _SWIZZLE4(x, z, y, w)
        _SWIZZLE4(x, z, z, x)
        _SWIZZLE4(x, z, z, y)
        _SWIZZLE4(x, z, z, z)
        _SWIZZLE4(x, z, z, w)
        _SWIZZLE4(x, z, w, x)
        _SWIZZLE4(x, z, w, y)
        _SWIZZLE4(x, z, w, z)
        _SWIZZLE4(x, z, w, w)
        _SWIZZLE4(x, w, x, x)
        _SWIZZLE4(x, w, x, y)
        _SWIZZLE4(x, w, x, z)
        _SWIZZLE4(x, w, x, w)
        _SWIZZLE4(x, w, y, x)
        _SWIZZLE4(x, w, y, y)
        _SWIZZLE4(x, w, y, z)
        _SWIZZLE4(x, w, y, w)
        _SWIZZLE4(x, w, z, x)
        _SWIZZLE4(x, w, z, y)
        _SWIZZLE4(x, w, z, z)
        _SWIZZLE4(x, w, z, w)
        _SWIZZLE4(x, w, w, x)
        _SWIZZLE4(x, w, w, y)
        _SWIZZLE4(x, w, w, z)
        _SWIZZLE4(x, w, w, w)
        _SWIZZLE4(y, x, x, x)
        _SWIZZLE4(y, x, x, y)
        _SWIZZLE4(y, x, x, z)
        _SWIZZLE4(y, x, x, w)
        _SWIZZLE4(y, x, y, x)
        _SWIZZLE4(y, x, y, y)
        _SWIZZLE4(y, x, y, z)
        _SWIZZLE4(y, x, y, w)
        _SWIZZLE4(y, x, z, x)
        _SWIZZLE4(y, x, z, y)
        _SWIZZLE4(y, x, z, z)
        _SWIZZLE4(y, x, z, w)
        _SWIZZLE4(y, x, w, x)
        _SWIZZLE4(y, x, w, y)
        _SWIZZLE4(y, x, w, z)
        _SWIZZLE4(y, x, w, w)
        _SWIZZLE4(y, y, x, x)
        _SWIZZLE4(y, y, x, y)
        _SWIZZLE4(y, y, x, z)
        _SWIZZLE4(y, y, x, w)
        _SWIZZLE4(y, y, y, x)
        _SWIZZLE4(y, y, y, y)
        _SWIZZLE4(y, y, y, z)
        _SWIZZLE4(y, y, y, w)
        _SWIZZLE4(y, y, z, x)
        _SWIZZLE4(y, y, z, y)
        _SWIZZLE4(y, y, z, z)
        _SWIZZLE4(y, y, z, w)
        _SWIZZLE4(y, y, w, x)
        _SWIZZLE4(y, y, w, y)
        _SWIZZLE4(y, y, w, z)
        _SWIZZLE4(y, y, w, w)
        _SWIZZLE4(y, z, x, x)
        _SWIZZLE4(y, z, x, y)
        _SWIZZLE4(y, z, x, z)
        _SWIZZLE4(y, z, x, w)
        _SWIZZLE4(y, z, y, x)
        _SWIZZLE4(y, z, y, y)
        _SWIZZLE4(y, z, y, z)
        _SWIZZLE4(y, z, y, w)
        _SWIZZLE4(y, z, z, x)
        _SWIZZLE4(y, z, z, y)
        _SWIZZLE4(y, z, z, z)
        _SWIZZLE4(y, z, z, w)
        _SWIZZLE4(y, z, w, x)
        _SWIZZLE4(y, z, w, y)
        _SWIZZLE4(y, z, w, z)
        _SWIZZLE4(y, z, w, w)
        _SWIZZLE4(y, w, x, x)
        _SWIZZLE4(y, w, x, y)
        _SWIZZLE4(y, w, x, z)
        _SWIZZLE4(y, w, x, w)
        _SWIZZLE4(y, w, y, x)
        _SWIZZLE4(y, w, y, y)
        _SWIZZLE4(y, w, y, z)
        _SWIZZLE4(y, w, y, w)
        _SWIZZLE4(y, w, z, x)
        _SWIZZLE4(y, w, z, y)
        _SWIZZLE4(y, w, z, z)
        _SWIZZLE4(y, w, z, w)
        _SWIZZLE4(y, w, w, x)
        _SWIZZLE4(y, w, w, y)
        _SWIZZLE4(y, w, w, z)
        _SWIZZLE4(y, w, w, w)
        _SWIZZLE4(z, x, x, x)
        _SWIZZLE4(z, x, x, y)
        _SWIZZLE4(z, x, x, z)
        _SWIZZLE4(z, x, x, w)
        _SWIZZLE4(z, x, y, x)
        _SWIZZLE4(z, x, y, y)
        _SWIZZLE4(z, x, y, z)
        _SWIZZLE4(z, x, y, w)
        _SWIZZLE4(z, x, z, x)
        _SWIZZLE4(z, x, z, y)
        _SWIZZLE4(z, x, z, z)
        _SWIZZLE4(z, x, z, w)
        _SWIZZLE4(z, x, w, x)
        _SWIZZLE4(z, x, w, y)
        _SWIZZLE4(z, x, w, z)
        _SWIZZLE4(z, x, w, w)
        _SWIZZLE4(z, y, x, x)
        _SWIZZLE4(z, y, x, y)
        _SWIZZLE4(z, y, x, z)
        _SWIZZLE4(z, y, x, w)
        _SWIZZLE4(z, y, y, x)
        _SWIZZLE4(z, y, y, y)
        _SWIZZLE4(z, y, y, z)
        _SWIZZLE4(z, y, y, w)
        _SWIZZLE4(z, y, z, x)
        _SWIZZLE4(z, y, z, y)
        _SWIZZLE4(z, y, z, z)
        _SWIZZLE4(z, y, z, w)
        _SWIZZLE4(z, y, w, x)
        _SWIZZLE4(z, y, w, y)
        _SWIZZLE4(z, y, w, z)
        _SWIZZLE4(z, y, w, w)
        _SWIZZLE4(z, z, x, x)
        _SWIZZLE4(z, z, x, y)
        _SWIZZLE4(z, z, x, z)
        _SWIZZLE4(z, z, x, w)
        _SWIZZLE4(z, z, y, x)
        _SWIZZLE4(z, z, y, y)
        _SWIZZLE4(z, z, y, z)
        _SWIZZLE4(z, z, y, w)
        _SWIZZLE4(z, z, z, x)
        _SWIZZLE4(z, z, z, y)
        _SWIZZLE4(z, z, z, z)
        _SWIZZLE4(z, z, z, w)
        _SWIZZLE4(z, z, w, x)
        _SWIZZLE4(z, z, w, y)
        _SWIZZLE4(z, z, w, z)
        _SWIZZLE4(z, z, w, w)
        _SWIZZLE4(z, w, x, x)
        _SWIZZLE4(z, w, x, y)
        _SWIZZLE4(z, w, x, z)
        _SWIZZLE4(z, w, x, w)
        _SWIZZLE4(z, w, y, x)
        _SWIZZLE4(z, w, y, y)
        _SWIZZLE4(z, w, y, z)
        _SWIZZLE4(z, w, y, w)
        _SWIZZLE4(z, w, z, x)
        _SWIZZLE4(z, w, z, y)
        _SWIZZLE4(z, w, z, z)
        _SWIZZLE4(z, w, z, w)
        _SWIZZLE4(z, w, w, x)
        _SWIZZLE4(z, w, w, y)
        _SWIZZLE4(z, w, w, z)
        _SWIZZLE4(z, w, w, w)
        _SWIZZLE4(w, x, x, x)
        _SWIZZLE4(w, x, x, y)
        _SWIZZLE4(w, x, x, z)
        _SWIZZLE4(w, x, x, w)
        _SWIZZLE4(w, x, y, x)
        _SWIZZLE4(w, x, y, y)
        _SWIZZLE4(w, x, y, z)
        _SWIZZLE4(w, x, y, w)
        _SWIZZLE4(w, x, z, x)
        _SWIZZLE4(w, x, z, y)
        _SWIZZLE4(w, x, z, z)
        _SWIZZLE4(w, x, z, w)
        _SWIZZLE4(w, x, w, x)
        _SWIZZLE4(w, x, w, y)
        _SWIZZLE4(w, x, w, z)
        _SWIZZLE4(w, x, w, w)
        _SWIZZLE4(w, y, x, x)
        _SWIZZLE4(w, y, x, y)
        _SWIZZLE4(w, y, x, z)
        _SWIZZLE4(w, y, x, w)
        _SWIZZLE4(w, y, y, x)
        _SWIZZLE4(w, y, y, y)
        _SWIZZLE4(w, y, y, z)
        _SWIZZLE4(w, y, y, w)
        _SWIZZLE4(w, y, z, x)
        _SWIZZLE4(w, y, z, y)
        _SWIZZLE4(w, y, z, z)
        _SWIZZLE4(w, y, z, w)
        _SWIZZLE4(w, y, w, x)
        _SWIZZLE4(w, y, w, y)
        _SWIZZLE4(w, y, w, z)
        _SWIZZLE4(w, y, w, w)
        _SWIZZLE4(w, z, x, x)
        _SWIZZLE4(w, z, x, y)
        _SWIZZLE4(w, z, x, z)
        _SWIZZLE4(w, z, x, w)
        _SWIZZLE4(w, z, y, x)
        _SWIZZLE4(w, z, y, y)
        _SWIZZLE4(w, z, y, z)
        _SWIZZLE4(w, z, y, w)
        _SWIZZLE4(w, z, z, x)
        _SWIZZLE4(w, z, z, y)
        _SWIZZLE4(w, z, z, z)
        _SWIZZLE4(w, z, z, w)
        _SWIZZLE4(w, z, w, x)
        _SWIZZLE4(w, z, w, y)
        _SWIZZLE4(w, z, w, z)
        _SWIZZLE4(w, z, w, w)
        _SWIZZLE4(w, w, x, x)
        _SWIZZLE4(w, w, x, y)
        _SWIZZLE4(w, w, x, z)
        _SWIZZLE4(w, w, x, w)
        _SWIZZLE4(w, w, y, x)
        _SWIZZLE4(w, w, y, y)
        _SWIZZLE4(w, w, y, z)
        _SWIZZLE4(w, w, y, w)
        _SWIZZLE4(w, w, z, x)
        _SWIZZLE4(w, w, z, y)
        _SWIZZLE4(w, w, z, z)
        _SWIZZLE4(w, w, z, w)
        _SWIZZLE4(w, w, w, x)
        _SWIZZLE4(w, w, w, y)
        _SWIZZLE4(w, w, w, z)
        _SWIZZLE4(w, w, w, w)

        [[nodiscard]] ShaderObject element(const ShaderObject &column,
                                           const ShaderObject &row) {
            return ShaderObject(ShaderInstructionFactory::matrixSubscript(operand, column.operand, row.operand),
                                assignable);
        }

        ShaderObject column(const ShaderObject &column) {
            return ShaderObject(ShaderInstructionFactory::matrixSubscript(operand, column.operand),
                                assignable);
        }

        ShaderObject length() {
            if (operand.type == ShaderOperand::Buffer) {
                return ShaderObject(ShaderInstructionFactory::bufferSize(operand.name));
            }
            throw std::runtime_error("length() called on non buffer");
        }

        /**
         * The move and copy assignment operators are overloaded to translate logical assignments to shader instructions.
         *
         * This enables the user to assign variables in glsl style.
         *
         * @param rhs
         * @return
         */
        ShaderObject &operator=(ShaderObject &&rhs) {
            assignValue(rhs);
            return *this;
        }

        /**
         * The move and copy assignment operators are overloaded to translate logical assignments to shader instructions.
         *
         * This enables the user to assign variables in glsl style.
         *
         * @param rhs
         * @return
         */
        ShaderObject &operator=(const ShaderObject &rhs) {
            assignValue(rhs);
            return *this;
        }

        ShaderObject operator+(const ShaderObject &rhs) {
            return ShaderObject(ShaderInstructionFactory::add(operand, rhs.operand));
        }

        ShaderObject operator-(const ShaderObject &rhs) {
            return ShaderObject(ShaderInstructionFactory::subtract(operand, rhs.operand));
        }

        ShaderObject operator*(const ShaderObject &rhs) {
            return ShaderObject(ShaderInstructionFactory::multiply(operand, rhs.operand));
        }

        ShaderObject operator/(const ShaderObject &rhs) {
            return ShaderObject(ShaderInstructionFactory::divide(operand, rhs.operand));
        }

        ShaderObject operator+=(const ShaderObject &rhs) {
            assignValue(*this + rhs);
            return *this;
        }

        ShaderObject operator-=(const ShaderObject &rhs) {
            assignValue(*this - rhs);
            return *this;
        }

        ShaderObject operator*=(const ShaderObject &rhs) {
            assignValue(*this * rhs);
            return *this;
        }

        ShaderObject operator/=(const ShaderObject &rhs) {
            assignValue(*this / rhs);
            return *this;
        }

        ShaderObject operator==(const ShaderObject &rhs) {
            return ShaderObject(ShaderInstructionFactory::compareEqual(operand, rhs.operand));
        }

        ShaderObject operator!=(const ShaderObject &rhs) {
            return ShaderObject(ShaderInstructionFactory::compareNotEqual(operand, rhs.operand));
        }

        ShaderObject operator<(const ShaderObject &rhs) {
            return ShaderObject(ShaderInstructionFactory::compareLess(operand, rhs.operand));
        }

        ShaderObject operator>(const ShaderObject &rhs) {
            return ShaderObject(ShaderInstructionFactory::compareGreater(operand, rhs.operand));
        }

        ShaderObject operator <=(const ShaderObject &rhs) {
            return ShaderObject(ShaderInstructionFactory::compareLessEqual(operand, rhs.operand));
        }

        ShaderObject operator >=(const ShaderObject &rhs) {
            return ShaderObject(ShaderInstructionFactory::compareGreaterEqual(operand, rhs.operand));
        }

        ShaderObject operator||(const ShaderObject &rhs) {
            return ShaderObject(ShaderInstructionFactory::logicalOr(operand, rhs.operand));
        }

        ShaderObject operator&&(const ShaderObject &rhs) {
            return ShaderObject(ShaderInstructionFactory::logicalAnd(operand, rhs.operand));
        }

        ShaderObject operator[](const char *elementName) {
            return ShaderObject(ShaderInstructionFactory::objectMember(operand, elementName), assignable);
        }

        ShaderObject operator[](const ShaderObject &index) {
            return ShaderObject(ShaderInstructionFactory::arraySubscript(operand, index.operand), assignable);
        }

    protected:
        ShaderObject swizzle_vec2(const ShaderInstruction::VectorComponent &x,
                                  const ShaderInstruction::VectorComponent &y) {
            return {
                ShaderInstructionFactory::vectorSwizzle(operand, {
                                                            x,
                                                            y,
                                                        }),
                assignable
            };
        }

        ShaderObject swizzle_vec3(const ShaderInstruction::VectorComponent &x,
                                  const ShaderInstruction::VectorComponent &y,
                                  const ShaderInstruction::VectorComponent &z) {
            return {
                ShaderInstructionFactory::vectorSwizzle(operand, {
                                                            x,
                                                            y,
                                                            z
                                                        }),
                assignable
            };
        }

        ShaderObject swizzle_vec4(const ShaderInstruction::VectorComponent &x,
                                  const ShaderInstruction::VectorComponent &y,
                                  const ShaderInstruction::VectorComponent &z,
                                  const ShaderInstruction::VectorComponent &w) {
            return {
                ShaderInstructionFactory::vectorSwizzle(operand, {
                                                            x,
                                                            y,
                                                            z,
                                                            w
                                                        }),
                assignable
            };
        }

    private:
        /**
         * Because copy initialization in C++17 can be elided (e.g. vec2 v = vec2(0, 0)), and explicitly deleting
         * copy operators does not affect the copy initialization if the initializer value is a prvalue,
         * I cannot rely on the assignment operator overload for defining shader variables for prvalue initializers.
         * (See http://en.cppreference.com/w/cpp/language/copy_initialization.html)
         *
         * Therefore, copy initialized variables (e.g. vec2 v = vec2(0, 0)) are inlined in the resulting shader and are not assignable.
         * Move initialized variables (e.g. vec2 v = other.xy()) are not inlined and are assignable.
         */
        void assignValue(const ShaderObject &value) {
            if (!assignable) {
                throw std::runtime_error(
                    "Attempted assignment to non-assignable variable (Copy initialized variable?)");
            }
            ShaderBuilder::instance().addInstruction(ShaderInstructionFactory::assign(operand, value.operand));
        }
    };

    /**
     * A node wrapper with specific type information in a value template.
     *
     * This allows users to create variables in the style of glsl through the typedefs in shaderscript.hpp
     *
     * @tparam VALUE_TYPE
     * @tparam VALUE_COMPONENT
     * @tparam VALUE_COUNT
     */
    template<ShaderDataType::Type VALUE_TYPE, ShaderDataType::Component VALUE_COMPONENT, size_t VALUE_COUNT>
    class ShaderDataObject : public ShaderObject {
    public:
        /**
         * Default constructor creates new variable.
         */
        ShaderDataObject() {
            const auto varName = ShaderBuilder::instance().getVariableName();
            ShaderBuilder::instance().addInstruction(ShaderInstructionFactory::declareVariable(varName,
                ShaderDataType{
                    VALUE_TYPE, VALUE_COMPONENT,
                    VALUE_COUNT
                }));
            operand = ShaderOperand(ShaderOperand::Variable, varName);
            assignable = true;
        }

        // Copy operators and constructors for non prvalues are deleted to force move semantics and avoid copy eliding.
        ShaderDataObject(const ShaderObject &base) = delete;

        ShaderObject &operator=(const ShaderObject &other) = delete;

        // Move constructor can be deleted because in the dsl syntax variables are not initialized with vec2 v(other);
        ShaderDataObject(ShaderDataObject &&other) noexcept = delete;

        // Move operator is allowed to support assignments to default initialized variables from prvalues (e.g. vec2 v; v = vec2(0, 0);)
        ShaderObject &operator=(ShaderDataObject &&other) noexcept {
            return ShaderObject::operator=(std::move(other));
        }

        // Copy operators is allowed to support copying variables (e.g. vec2 u = vec2(0, 0); vec2 d = u;)
        ShaderObject &operator=(const ShaderDataObject &other) {
            return ShaderObject::operator=(other);
        }

        // Move operation to a previously default constructed object (e.g. vec2 u; u = other.xyz())
        ShaderObject &operator=(ShaderObject &&other) {
            return ShaderObject::operator=(std::move(other));
        }

        // Move operations from non prvalues, creates and initializes a variable. (e.g. vec2 v = other.xyz())
        // Automatically handles conversion (e.g. vec3 v = other.xy() turns into vec3 v = vec3(other.xy()))
        ShaderDataObject(ShaderObject &&other) noexcept
            : ShaderObject(std::move(other)) {
            if (VALUE_TYPE >= ShaderDataType::VECTOR2 && VALUE_TYPE <= ShaderDataType::VECTOR4) {
                auto instruction = ShaderInstructionFactory::createVector({VALUE_TYPE, VALUE_COMPONENT, VALUE_COUNT},
                                                                          operand);
                operand = ShaderOperand(instruction);
            } else if (VALUE_TYPE >= ShaderDataType::MAT2 && VALUE_TYPE <= ShaderDataType::MAT4) {
                auto instruction = ShaderInstructionFactory::createMatrix({VALUE_TYPE, VALUE_COMPONENT, VALUE_COUNT},
                                                                          operand);
                operand = ShaderOperand(instruction);
            }
            const auto varName = ShaderBuilder::instance().getVariableName();
            ShaderBuilder::instance().addInstruction(ShaderInstructionFactory::declareVariable(varName, ShaderDataType{
                    VALUE_TYPE, VALUE_COMPONENT,
                    VALUE_COUNT
                },
                operand));
            operand = ShaderOperand(ShaderOperand::Variable, varName);
            assignable = true;
        }

        // Array constructor
        ShaderDataObject(const std::vector<ShaderDataObject<VALUE_TYPE, VALUE_COMPONENT, 1> > &values)
            : ShaderObject(ShaderInstructionFactory::createArray({
                                                                     VALUE_TYPE,
                                                                     VALUE_COMPONENT,
                                                                     1
                                                                 }, getOperands(values))) {
        }

        // Scalar / Vector / Matrix Constructor
        ShaderDataObject(const ShaderDataObject &x) {
            if (VALUE_TYPE >= ShaderDataType::VECTOR2 && VALUE_TYPE <= ShaderDataType::VECTOR4) {
                auto instruction = ShaderInstructionFactory::createVector({VALUE_TYPE, VALUE_COMPONENT, VALUE_COUNT},
                                                                          x.operand);
                operand = ShaderOperand(instruction);
            } else if (VALUE_TYPE >= ShaderDataType::MAT2 && VALUE_TYPE <= ShaderDataType::MAT4) {
                auto instruction = ShaderInstructionFactory::createMatrix({VALUE_TYPE, VALUE_COMPONENT, VALUE_COUNT},
                                                                          x.operand);
                operand = ShaderOperand(instruction);
            } else {
                operand = x.operand;
            }
        }

        // Vector / Matrix Constructors
        ShaderDataObject(const ShaderObject &x, const ShaderObject &y) {
            if (VALUE_TYPE >= ShaderDataType::VECTOR2 && VALUE_TYPE <= ShaderDataType::VECTOR4) {
                auto instruction = ShaderInstructionFactory::createVector({VALUE_TYPE, VALUE_COMPONENT, VALUE_COUNT},
                                                                          x.operand,
                                                                          y.operand);
                operand = ShaderOperand(instruction);
            } else if (VALUE_TYPE >= ShaderDataType::MAT2 && VALUE_TYPE <= ShaderDataType::MAT4) {
                auto instruction = ShaderInstructionFactory::createMatrix({VALUE_TYPE, VALUE_COMPONENT, VALUE_COUNT},
                                                                          x.operand,
                                                                          y.operand);
                ShaderBuilder::instance().addInstruction(instruction);
                operand = ShaderOperand(instruction);
            } else {
                throw std::runtime_error("Invalid scalar constructor invoked");
            }
        }

        ShaderDataObject(const ShaderObject &x, const ShaderObject &y, const ShaderObject &z) {
            if (VALUE_TYPE >= ShaderDataType::VECTOR2 && VALUE_TYPE <= ShaderDataType::VECTOR4) {
                auto instruction = ShaderInstructionFactory::createVector({VALUE_TYPE, VALUE_COMPONENT, VALUE_COUNT},
                                                                          x.operand,
                                                                          y.operand,
                                                                          z.operand);
                operand = ShaderOperand(instruction);
            } else if (VALUE_TYPE >= ShaderDataType::MAT2 && VALUE_TYPE <= ShaderDataType::MAT4) {
                auto instruction = ShaderInstructionFactory::createMatrix({VALUE_TYPE, VALUE_COMPONENT, VALUE_COUNT},
                                                                          x.operand,
                                                                          y.operand,
                                                                          z.operand);
                operand = ShaderOperand(instruction);
            } else {
                throw std::runtime_error("Invalid scalar constructor invoked");
            }
        }

        ShaderDataObject(const ShaderObject &x, const ShaderObject &y, const ShaderObject &z, const ShaderObject &w) {
            if (VALUE_TYPE >= ShaderDataType::VECTOR2 && VALUE_TYPE <= ShaderDataType::VECTOR4) {
                auto instruction = ShaderInstructionFactory::createVector({VALUE_TYPE, VALUE_COMPONENT, VALUE_COUNT},
                                                                          x.operand,
                                                                          y.operand,
                                                                          z.operand,
                                                                          w.operand);
                operand = ShaderOperand(instruction);
            } else if (VALUE_TYPE >= ShaderDataType::MAT2 && VALUE_TYPE <= ShaderDataType::MAT4) {
                auto instruction = ShaderInstructionFactory::createMatrix({VALUE_TYPE, VALUE_COMPONENT, VALUE_COUNT},
                                                                          x.operand,
                                                                          y.operand,
                                                                          z.operand,
                                                                          w.operand);
                operand = ShaderOperand(instruction);
            } else {
                throw std::runtime_error("Invalid scalar constructor invoked");
            }
        }

        // Literal Constructors
        ShaderDataObject(const bool literal)
            : ShaderObject(literal) {
        }

        ShaderDataObject(const int literal)
            : ShaderObject(literal) {
        }

        ShaderDataObject(const unsigned int literal)
            : ShaderObject(literal) {
        }

        ShaderDataObject(const float literal)
            : ShaderObject(literal) {
        }

        ShaderDataObject(const double literal)
            : ShaderObject(literal) {
        }

    private:
        static std::vector<ShaderOperand> getOperands(
            const std::vector<ShaderDataObject<VALUE_TYPE, VALUE_COMPONENT, 1> > &values) {
            std::vector<ShaderOperand> ops;
            for (auto &value: values) {
                ops.push_back(value.operand);
            }
            return ops;
        }
    };

    template<const char * typeName>
    class ShaderStructObject : public ShaderObject {
    public:
        /**
         * Default constructor creates new variable.
         */
        ShaderStructObject() {
            const auto varName = ShaderBuilder::instance().getVariableName();
            ShaderBuilder::instance().addInstruction(ShaderInstructionFactory::declareVariable(varName, typeName));
            operand = ShaderOperand(ShaderOperand::Variable, varName);
            assignable = true;
        }

        // Copy operators and constructors for non prvalues are deleted to force move semantics and avoid copy eliding.
        ShaderStructObject(const ShaderObject &base) = delete;

        ShaderObject &operator=(const ShaderObject &other) = delete;

        // Copy operators and constructors for prvalues are allowed to support copying a variable (e.g. vec2 v = other; or vec2 v; v = other)
        ShaderStructObject(const ShaderStructObject &other)
            : ShaderObject(other) {
        }

        ShaderObject &operator=(const ShaderStructObject &other) {
            return ShaderObject::operator=(other);
        }

        // Move constructor can be deleted because in the dsl syntax variables are not initialized with vec2 v(other);
        ShaderStructObject(ShaderStructObject &&other) noexcept = delete;

        // Move operator is allowed to support assignments to default initialized variables from prvalues (e.g. vec2 v; v = vec2(0, 0);)
        ShaderObject &operator=(ShaderStructObject &&other) noexcept {
            return ShaderObject::operator=(std::move(other));
        }

        // Move operations from non prvalues, creates and initializes a variable.
        ShaderStructObject(ShaderObject &&other) noexcept
            : ShaderObject(std::move(other)) {
            const auto varName = ShaderBuilder::instance().getVariableName();
            ShaderBuilder::instance().addInstruction(
                ShaderInstruction(ShaderInstructionFactory::declareVariable(varName, typeName, operand)));
            operand = ShaderOperand(ShaderOperand::Variable, varName);
        }

        ShaderObject &operator=(ShaderObject &&other) {
            return ShaderObject::operator=(std::move(other));
        }
    };
}

#endif //XENGINE_ShaderObject_HPP
