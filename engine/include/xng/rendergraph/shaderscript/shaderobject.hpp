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

#include "xng/rendergraph/shader/shaderinstructionfactory.hpp"
#include "xng/rendergraph/shader/shadertexture.hpp"

#include "xng/rendergraph/shaderscript/blockscope.hpp"

#define _SWIZZLE2(x, y) [[nodiscard]] ShaderObject x##y() { return swizzle_vec2(ShaderPrimitiveType::COMPONENT_##x, ShaderPrimitiveType::COMPONENT_##y); }
#define _SWIZZLE3(x, y, z) [[nodiscard]] ShaderObject x##y##z() { return swizzle_vec3(ShaderPrimitiveType::COMPONENT_##x, ShaderPrimitiveType::COMPONENT_##y, ShaderPrimitiveType::COMPONENT_##z); }
#define _SWIZZLE4(x, y, z, w) [[nodiscard]] ShaderObject x##y##z##w() { return swizzle_vec4(ShaderPrimitiveType::COMPONENT_##x, ShaderPrimitiveType::COMPONENT_##y, ShaderPrimitiveType::COMPONENT_##z, ShaderPrimitiveType::COMPONENT_##w); }

namespace xng::ShaderScript {
    class ShaderObject {
    public:
        ShaderOperand operand;

        ShaderObject() = default;

        ShaderObject(const ShaderInstruction &instruction)
            : operand(ShaderOperand::instruction(instruction)) {
        }

        ShaderObject(ShaderOperand operand)
            : operand(std::move(operand)) {
        }

        ShaderObject(const ShaderObject &other) = default;

        // Literal constructors
        ShaderObject(const bool literal)
            : operand(ShaderOperand::literal(literal)) {
        }

        ShaderObject(const int literal)
            : operand(ShaderOperand::literal(literal)) {
        }

        ShaderObject(const unsigned int literal)
            : operand(ShaderOperand::literal(literal)) {
        }

        ShaderObject(const float literal)
            : operand(ShaderOperand::literal(literal)) {
        }

        ShaderObject(const double literal)
            : operand(ShaderOperand::literal(literal)) {
        }

        [[nodiscard]] ShaderObject x() {
            return ShaderObject(ShaderInstructionFactory::vectorSwizzle(operand, {
                ShaderPrimitiveType::COMPONENT_x
            }));
        }

        [[nodiscard]] ShaderObject y() {
            return ShaderObject(ShaderInstructionFactory::vectorSwizzle(operand, {
                ShaderPrimitiveType::COMPONENT_y
            }));
        }

        [[nodiscard]] ShaderObject z() {
            return ShaderObject(ShaderInstructionFactory::vectorSwizzle(operand, {
                ShaderPrimitiveType::COMPONENT_z
            }));
        }

        [[nodiscard]] ShaderObject w() {
            return ShaderObject(ShaderInstructionFactory::vectorSwizzle(operand, {
                ShaderPrimitiveType::COMPONENT_w
            }));
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
            return ShaderObject(ShaderInstructionFactory::matrixSubscript(operand, column.operand, row.operand));
        }

        ShaderObject column(const ShaderObject &column) {
            return ShaderObject(ShaderInstructionFactory::matrixSubscript(operand, column.operand));
        }

        ShaderObject length() {
            if (operand.type == ShaderOperand::Buffer) {
                return ShaderObject(ShaderInstructionFactory::bufferSize(operand));
            }
            throw std::runtime_error("length() called on non buffer");
        }

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

        ShaderObject operator<=(const ShaderObject &rhs) {
            return ShaderObject(ShaderInstructionFactory::compareLessEqual(operand, rhs.operand));
        }

        ShaderObject operator>=(const ShaderObject &rhs) {
            return ShaderObject(ShaderInstructionFactory::compareGreaterEqual(operand, rhs.operand));
        }

        ShaderObject operator||(const ShaderObject &rhs) {
            return ShaderObject(ShaderInstructionFactory::logicalOr(operand, rhs.operand));
        }

        ShaderObject operator&&(const ShaderObject &rhs) {
            return ShaderObject(ShaderInstructionFactory::logicalAnd(operand, rhs.operand));
        }

        ShaderObject operator[](const char *elementName) {
            return ShaderObject(ShaderInstructionFactory::objectMember(operand, elementName));
        }

        ShaderObject operator[](const ShaderObject &index) {
            return ShaderObject(ShaderInstructionFactory::arraySubscript(operand, index.operand));
        }

    protected:
        ShaderObject swizzle_vec2(const ShaderPrimitiveType::VectorComponent &x,
                                  const ShaderPrimitiveType::VectorComponent &y) {
            return ShaderObject(ShaderInstructionFactory::vectorSwizzle(operand, {x, y}));
        }

        ShaderObject swizzle_vec3(const ShaderPrimitiveType::VectorComponent &x,
                                  const ShaderPrimitiveType::VectorComponent &y,
                                  const ShaderPrimitiveType::VectorComponent &z) {
            return ShaderObject(ShaderInstructionFactory::vectorSwizzle(operand, {x, y, z}));
        }

        ShaderObject swizzle_vec4(const ShaderPrimitiveType::VectorComponent &x,
                                  const ShaderPrimitiveType::VectorComponent &y,
                                  const ShaderPrimitiveType::VectorComponent &z,
                                  const ShaderPrimitiveType::VectorComponent &w) {
            return ShaderObject(ShaderInstructionFactory::vectorSwizzle(operand, {x, y, z, w}));
        }

    private:
        void assignValue(const ShaderObject &value) const
        {
            // Assignments to objects must be tracked so we can emit logical assignments
            // When user direct initializes an object like vec2 v = vec2(1, 1);
            // the vec2(1,1) is registered to the current scope as an object with the given initializer.
            // v itself just gets this object reference via direct initialization so no assign value instruction is emitted.
            BlockScope::get().addInstruction(ShaderInstructionFactory::assign(operand, value.operand));
        }
    };

    /**
     * A shader object with type information stored in a value template.
     *
     * This allows users to create variables in the style of glsl through the typedefs in shaderscript.hpp
     *
     * @tparam VALUE_TYPE
     * @tparam VALUE_COMPONENT
     * @tparam VALUE_COUNT
     */
    template<ShaderPrimitiveType::Type VALUE_TYPE, ShaderPrimitiveType::Component VALUE_COMPONENT, size_t VALUE_COUNT>
    class ShaderDataObject : public ShaderObject {
    public:
        static inline ShaderDataType TYPE = ShaderDataType(ShaderPrimitiveType(VALUE_TYPE, VALUE_COMPONENT),
                                                           VALUE_COUNT);

        // Default constructor - creates a new registered variable
        ShaderDataObject() {
            operand = BlockScope::get().registerObject(TYPE, {});
        }

        ShaderDataObject &operator=(const ShaderDataObject &other) {
            ShaderObject::operator=(other);
            return *this;
        }

        ShaderDataObject &operator=(const ShaderObject &other) {
            ShaderObject::operator=(other);
            return *this;
        }

        // Conversion constructor - creates and registers a variable with automatic type conversion
        ShaderDataObject(const ShaderObject &other)
            : ShaderObject(other) {
            ShaderOperand initializer = other.operand;
            if (VALUE_TYPE >= ShaderPrimitiveType::VECTOR2 && VALUE_TYPE <= ShaderPrimitiveType::VECTOR4) {
                initializer = ShaderOperand::instruction(ShaderInstructionFactory::createVector(TYPE.getPrimitive(), other.operand));
            } else if (VALUE_TYPE >= ShaderPrimitiveType::MAT2 && VALUE_TYPE <= ShaderPrimitiveType::MAT4) {
                initializer = ShaderOperand::instruction(ShaderInstructionFactory::createMatrix(TYPE.getPrimitive(), other.operand));
            }
            operand = BlockScope::get().registerObject(TYPE, initializer);
        }

        // Construct from inline instruction (e.g. struct member access)
        explicit ShaderDataObject(const ShaderInstruction &other)
            : ShaderObject(other) {}

        // Construct from inline operand (e.g. for loop variable, function argument)
        explicit ShaderDataObject(ShaderOperand other)
            : ShaderObject(std::move(other)) {}

        // Array constructor
        ShaderDataObject(const std::vector<ShaderDataObject<VALUE_TYPE, VALUE_COMPONENT, 1>> &values)
            : ShaderObject(ShaderInstructionFactory::createArray(ShaderDataType{
                  ShaderPrimitiveType{VALUE_TYPE, VALUE_COMPONENT}, VALUE_COUNT
              }, getOperands(values))) {}

        // Copy constructor - creates and registers a variable with automatic type conversion
        ShaderDataObject(const ShaderDataObject &x) {
            ShaderOperand initializer = x.operand;
            if (VALUE_TYPE >= ShaderPrimitiveType::VECTOR2 && VALUE_TYPE <= ShaderPrimitiveType::VECTOR4) {
                initializer = ShaderOperand::instruction(ShaderInstructionFactory::createVector(TYPE.getPrimitive(), x.operand));
            } else if (VALUE_TYPE >= ShaderPrimitiveType::MAT2 && VALUE_TYPE <= ShaderPrimitiveType::MAT4) {
                initializer = ShaderOperand::instruction(ShaderInstructionFactory::createMatrix(TYPE.getPrimitive(), x.operand));
            }
            operand = BlockScope::get().registerObject(TYPE, initializer);
        }

        // Multi-arg constructors for vector/matrix construction
        ShaderDataObject(const ShaderObject &x, const ShaderObject &y) {
            operand = ShaderOperand::instruction(makeComposite(x.operand, y.operand));
        }

        ShaderDataObject(const ShaderObject &x, const ShaderObject &y, const ShaderObject &z) {
            operand = ShaderOperand::instruction(makeComposite(x.operand, y.operand, z.operand));
        }

        ShaderDataObject(const ShaderObject &x, const ShaderObject &y, const ShaderObject &z, const ShaderObject &w) {
            operand = ShaderOperand::instruction(makeComposite(x.operand, y.operand, z.operand, w.operand));
        }

        // Literal constructors
        ShaderDataObject(const bool literal) : ShaderObject(literal) { wrapLiteralIfVector(); }
        ShaderDataObject(const int literal) : ShaderObject(literal) { wrapLiteralIfVector(); }
        ShaderDataObject(const unsigned int literal) : ShaderObject(literal) { wrapLiteralIfVector(); }
        ShaderDataObject(const float literal) : ShaderObject(literal) { wrapLiteralIfVector(); }
        ShaderDataObject(const double literal) : ShaderObject(literal) { wrapLiteralIfVector(); }

    private:
        void wrapLiteralIfVector() {
            if (VALUE_TYPE >= ShaderPrimitiveType::VECTOR2 && VALUE_TYPE <= ShaderPrimitiveType::VECTOR4) {
                operand = ShaderOperand::instruction(ShaderInstructionFactory::createVector(TYPE.getPrimitive(), operand));
            }
        }

        template<typename... Args>
        ShaderInstruction makeComposite(Args&&... args) {
            if (VALUE_TYPE >= ShaderPrimitiveType::VECTOR2 && VALUE_TYPE <= ShaderPrimitiveType::VECTOR4) {
                return ShaderInstructionFactory::createVector(TYPE.getPrimitive(), std::forward<Args>(args)...);
            } else if (VALUE_TYPE >= ShaderPrimitiveType::MAT2 && VALUE_TYPE <= ShaderPrimitiveType::MAT4) {
                return ShaderInstructionFactory::createMatrix(TYPE.getPrimitive(), std::forward<Args>(args)...);
            }
            throw std::runtime_error("Invalid scalar constructor invoked");
        }

        static std::vector<ShaderOperand> getOperands(
            const std::vector<ShaderDataObject<VALUE_TYPE, VALUE_COMPONENT, 1>> &values) {
            std::vector<ShaderOperand> ops;
            for (auto &value : values) {
                ops.push_back(value.operand);
            }
            return ops;
        }
    };

    template<const char * typeName, int C>
    class ShaderStructObject : public ShaderObject {
    public:
        static inline ShaderDataType TYPE = ShaderDataType(typeName, C);

        // Default constructor - creates a new registered variable
        ShaderStructObject() {
            operand = BlockScope::get().registerObject(TYPE, {});
        }

        ShaderStructObject(const ShaderStructObject &other) : ShaderObject(other) {}

        ShaderStructObject &operator=(const ShaderStructObject &other) {
            ShaderObject::operator=(other);
            return *this;
        }

        ShaderStructObject &operator=(const ShaderObject &other) {
            ShaderObject::operator=(other);
            return *this;
        }

        // Conversion from ShaderObject - creates and registers a variable
        ShaderStructObject(const ShaderObject &other)
            : ShaderObject(other) {
            operand = BlockScope::get().registerObject(TYPE, operand);
        }

        // Construct from inline operand (e.g. for loop variable)
        explicit ShaderStructObject(ShaderOperand other)
            : ShaderObject(std::move(other)) {}
    };

    template<TextureType t, ColorFormat format>
    class ShaderTextureObject : public ShaderObject
    {
    public:
        static inline ShaderTexture TYPE = ShaderTexture(t, format);

        ShaderTextureObject() {}

        ShaderTextureObject(const ShaderTextureObject &other) : ShaderObject(other) {}

        ShaderTextureObject &operator=(const ShaderObject &other) {
            operand = other.operand;
            return *this;
        }

        ShaderTextureObject &operator=(const ShaderTextureObject &other) {
            operand = other.operand;
            return *this;
        }

        ShaderTextureObject(const ShaderObject &other) {
            operand = other.operand;
        }

        explicit ShaderTextureObject(ShaderOperand other)
            : ShaderObject(std::move(other)) {}
    };
}

#endif //XENGINE_ShaderObject_HPP
