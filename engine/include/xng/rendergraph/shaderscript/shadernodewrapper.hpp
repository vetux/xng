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

#ifndef XENGINE_SHADERNODEWRAPPER_HPP
#define XENGINE_SHADERNODEWRAPPER_HPP

#include <fcntl.h>

#include "xng/rendergraph/shader/shadernodefactory.hpp"
#include "xng/rendergraph/shader/nodes.hpp"

#include "xng/rendergraph/shaderscript/shaderbuilder.hpp"

#define _SWIZZLE2(x, y) [[nodiscard]] ShaderNodeWrapper x##y() { return swizzle_vec2(x(), y()); }
#define _SWIZZLE3(x, y, z) [[nodiscard]] ShaderNodeWrapper x##y##z() { return swizzle_vec3(x(), y(), z()); }
#define _SWIZZLE4(x, y, z, w) [[nodiscard]] ShaderNodeWrapper x##y##z##w() { return swizzle_vec4(x(), y(), z(), w()); }

namespace xng::ShaderScript {
    class ShaderNodeWrapper {
    public:
        ShaderDataType type;
        std::unique_ptr<ShaderNode> node{};

        ShaderNodeWrapper() = delete;

        ShaderNodeWrapper(const ShaderDataType type,
                          std::unique_ptr<ShaderNode> valueNode)
            : type(type), node(std::move(valueNode)) {
        }

        ShaderNodeWrapper(const ShaderNodeWrapper &other) {
            type = other.type;
            node = other.node->copy();
        }

        ShaderNodeWrapper(ShaderNodeWrapper &&other) noexcept {
            type = other.type;
            node = std::move(other.node);
        }

        // Literal constructor
        ShaderNodeWrapper(const bool literal)
            : ShaderNodeWrapper(getLiteralType(literal),
                                ShaderNodeFactory::literal(literal)) {
        }

        ShaderNodeWrapper(const int literal)
            : ShaderNodeWrapper(getLiteralType(literal),
                                ShaderNodeFactory::literal(literal)) {
        }

        ShaderNodeWrapper(const unsigned int literal)
            : ShaderNodeWrapper(getLiteralType(literal),
                                ShaderNodeFactory::literal(literal)) {
        }

        ShaderNodeWrapper(const float literal)
            : ShaderNodeWrapper(getLiteralType(literal),
                                ShaderNodeFactory::literal(literal)) {
        }

        ShaderNodeWrapper(const double literal)
            : ShaderNodeWrapper(getLiteralType(literal),
                                ShaderNodeFactory::literal(literal)) {
        }

        [[nodiscard]] ShaderNodeWrapper x() {
            promoteToVariable(node);
            return {{ShaderDataType::SCALAR, type.component}, ShaderNodeFactory::getX(node)};
        }

        [[nodiscard]] ShaderNodeWrapper y() {
            promoteToVariable(node);
            return {{ShaderDataType::SCALAR, type.component}, ShaderNodeFactory::getY(node)};
        }

        [[nodiscard]] ShaderNodeWrapper z() {
            promoteToVariable(node);
            return {{ShaderDataType::SCALAR, type.component}, ShaderNodeFactory::getZ(node)};
        }

        [[nodiscard]] ShaderNodeWrapper w() {
            promoteToVariable(node);
            return {{ShaderDataType::SCALAR, type.component}, ShaderNodeFactory::getW(node)};
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

        [[nodiscard]] ShaderNodeWrapper element(const ShaderNodeWrapper &column,
                                                const ShaderNodeWrapper &row) {
            promoteToVariable(node);
            return ShaderNodeWrapper({ShaderDataType::SCALAR, type.component},
                                     ShaderNodeFactory::subscriptMatrix(node, column.node, row.node));
        }

        /**
         * The move and copy assignment operators are overloaded to translate logical assignments to shader node operations.
         *
         * This enables the user to assign variables in glsl style.
         *
         * Move / Copy assignments for non-variable node wrapper instances are not allowed.
         *
         * @param rhs
         * @return
         */
        ShaderNodeWrapper &operator=(ShaderNodeWrapper &&rhs) noexcept {
            // Hook logical assignments to variables
            if (node->getType() == ShaderNode::VARIABLE
                || node->getType() == ShaderNode::VECTOR_SWIZZLE
                || node->getType() == ShaderNode::SUBSCRIPT_MATRIX
                || node->getType() == ShaderNode::SUBSCRIPT_ARRAY) {
                // Assign value
                ShaderBuilder::instance().addNode(ShaderNodeFactory::assign(node, rhs.node));
                return *this;
            } else {
                // Assume assignment to previously in place initialized object eg.
                // vec2 v = vec2(0, 0)
                // v = vec2(1, 1)
                promoteToVariable(rhs.node);
                return *this;
            }
        }

        /**
         * The move and copy assignment operators are overloaded to translate logical assignments to shader node operations.
         *
         * This enables the user to assign variables in glsl style.
         *
         * Move / Copy assignments for non-variable node wrapper instances are not supported.
         *
         * @param rhs
         * @return
         */
        ShaderNodeWrapper &operator=(const ShaderNodeWrapper &rhs) {
            // Hook logical assignments
            if (node->getType() == ShaderNode::VARIABLE
                || node->getType() == ShaderNode::VECTOR_SWIZZLE
                || node->getType() == ShaderNode::SUBSCRIPT_MATRIX
                || node->getType() == ShaderNode::SUBSCRIPT_ARRAY) {
                // Assign value
                ShaderBuilder::instance().addNode(ShaderNodeFactory::assign(node, rhs.node));
                return *this;
            } else {
                // Assume assignment to previously in place initialized object eg.
                // vec2 v = vec2(0, 0)
                // v = vec2(1, 1)
                promoteToVariable(rhs.node);
                return *this;
            }
        }

        // TODO: Handle implicit promotion
        ShaderNodeWrapper operator+(const ShaderNodeWrapper &rhs) const {
            ShaderDataType outType;
            if (type == rhs.type) {
                // Arithmetic with identical types
                outType = type;
            } else {
                // Mixed Arithmetic e.g. vec3 + float
                if (type.type == ShaderDataType::SCALAR) {
                    outType = rhs.type;
                } else {
                    outType = type;
                }
            }
            auto ret = ShaderNodeWrapper(outType, ShaderNodeFactory::add(node, rhs.node));
            ret.promoteToVariable(ret.node);
            return ret;
        }

        ShaderNodeWrapper operator-(const ShaderNodeWrapper &rhs) const {
            ShaderDataType outType;
            if (type == rhs.type) {
                // Arithmetic with identical types
                outType = type;
            } else {
                // Mixed Arithmetic e.g. vec3 + float
                if (type.type == ShaderDataType::SCALAR) {
                    outType = rhs.type;
                } else {
                    outType = type;
                }
            }
            auto ret = ShaderNodeWrapper(outType, ShaderNodeFactory::subtract(node, rhs.node));
            ret.promoteToVariable(ret.node);
            return ret;
        }

        ShaderNodeWrapper operator*(const ShaderNodeWrapper &rhs) const {
            ShaderDataType outType;
            if (type == rhs.type) {
                // Arithmetic with identical types
                outType = type;
            } else {
                // Mixed Arithmetic

                // Matrix * Vector or Vector * Matrix
                if (type.type == ShaderDataType::MAT4 || rhs.type.type == ShaderDataType::MAT4) {
                    outType = {ShaderDataType::VECTOR4, type.component, 1};
                } else if (type.type == ShaderDataType::MAT3 || rhs.type.type == ShaderDataType::MAT3) {
                    outType = {ShaderDataType::VECTOR3, type.component, 1};
                } else if (type.type == ShaderDataType::MAT2 || rhs.type.type == ShaderDataType::MAT2) {
                    outType = {ShaderDataType::VECTOR2, type.component, 1};
                } else {
                    // Scalar * Vector
                    if (type.type == ShaderDataType::SCALAR) {
                        outType = rhs.type;
                    } else {
                        outType = type;
                    }
                }
            }
            auto ret = ShaderNodeWrapper(outType, ShaderNodeFactory::multiply(node, rhs.node));
            ret.promoteToVariable(ret.node);
            return ret;
        }

        ShaderNodeWrapper operator/(const ShaderNodeWrapper &rhs) const {
            ShaderDataType outType;
            if (type == rhs.type) {
                // Arithmetic with identical types
                outType = type;
            } else {
                // Mixed Arithmetic e.g. vec3 / float
                if (type.type == ShaderDataType::SCALAR) {
                    outType = rhs.type;
                } else {
                    outType = type;
                }
            }
            auto ret = ShaderNodeWrapper(outType, ShaderNodeFactory::divide(node, rhs.node));
            ret.promoteToVariable(ret.node);
            return ret;
        }

        ShaderNodeWrapper operator+=(const ShaderNodeWrapper &rhs) {
            if (node->getType() != ShaderNode::VARIABLE) {
                if (node->getType() != ShaderNode::VECTOR_SWIZZLE) {
                    // Assume assignment to previously in place initialized object eg.
                    // vec2 v = vec2(0, 0)
                    // v += vec2(1, 1)
                    promoteToVariable(node);
                }
            }
            ShaderBuilder::instance().addNode(ShaderNodeFactory::assign(node,
                                                                        ShaderNodeFactory::add(node, rhs.node)));
            return *this;
        }

        ShaderNodeWrapper operator-=(const ShaderNodeWrapper &rhs) {
            if (node->getType() != ShaderNode::VARIABLE) {
                if (node->getType() != ShaderNode::VECTOR_SWIZZLE) {
                    // Assume assignment to previously in place initialized object eg.
                    // vec2 v = vec2(0, 0)
                    // v -= vec2(1, 1)
                    promoteToVariable(node);
                }
            }
            ShaderBuilder::instance().addNode(ShaderNodeFactory::assign(node,
                                                                        ShaderNodeFactory::subtract(node, rhs.node)));
            return *this;
        }

        ShaderNodeWrapper operator*=(const ShaderNodeWrapper &rhs) {
            if (node->getType() != ShaderNode::VARIABLE) {
                if (node->getType() != ShaderNode::VECTOR_SWIZZLE) {
                    // Assume assignment to previously in place initialized object eg.
                    // vec2 v = vec2(0, 0)
                    // v *= vec2(1, 1)
                    promoteToVariable(node);
                }
            }
            ShaderBuilder::instance().addNode(
                ShaderNodeFactory::assign(node, ShaderNodeFactory::multiply(node, rhs.node)));
            return *this;
        }

        ShaderNodeWrapper operator/=(const ShaderNodeWrapper &rhs) {
            if (node->getType() != ShaderNode::VARIABLE) {
                if (node->getType() != ShaderNode::VECTOR_SWIZZLE) {
                    // Assume assignment to previously in place initialized object eg.
                    // vec2 v = vec2(0, 0)
                    // v /= vec2(1, 1)
                    promoteToVariable(node);
                }
            }
            ShaderBuilder::instance().addNode(ShaderNodeFactory::assign(node,
                                                                        ShaderNodeFactory::divide(node, rhs.node)));
            return *this;
        }

        ShaderNodeWrapper operator==(const ShaderNodeWrapper &rhs) const {
            auto ret = ShaderNodeWrapper(ShaderDataType::boolean(),
                                         ShaderNodeFactory::compareEqual(node, rhs.node));
            ret.promoteToVariable(ret.node);
            return ret;
        }

        ShaderNodeWrapper operator!=(const ShaderNodeWrapper &rhs) const {
            auto ret = ShaderNodeWrapper(ShaderDataType::boolean(),
                                         ShaderNodeFactory::compareNotEqual(node, rhs.node));
            ret.promoteToVariable(ret.node);
            return ret;
        }

        ShaderNodeWrapper operator<(const ShaderNodeWrapper &rhs) const {
            auto ret = ShaderNodeWrapper(ShaderDataType::boolean(),
                                         ShaderNodeFactory::compareLess(node, rhs.node));
            ret.promoteToVariable(ret.node);
            return ret;
        }

        ShaderNodeWrapper operator>(const ShaderNodeWrapper &rhs) const {
            auto ret = ShaderNodeWrapper(ShaderDataType::boolean(),
                                         ShaderNodeFactory::compareGreater(node, rhs.node));
            ret.promoteToVariable(ret.node);
            return ret;
        }

        ShaderNodeWrapper operator <=(const ShaderNodeWrapper &rhs) const {
            auto ret = ShaderNodeWrapper(ShaderDataType::boolean(),
                                         ShaderNodeFactory::compareLessEqual(node, rhs.node));
            ret.promoteToVariable(ret.node);
            return ret;
        }

        ShaderNodeWrapper operator >=(const ShaderNodeWrapper &rhs) const {
            auto ret = ShaderNodeWrapper(ShaderDataType::boolean(),
                                         ShaderNodeFactory::compareGreaterEqual(node, rhs.node));
            ret.promoteToVariable(ret.node);
            return ret;
        }

        ShaderNodeWrapper operator||(const ShaderNodeWrapper &rhs) const {
            auto ret = ShaderNodeWrapper(ShaderDataType::boolean(),
                                         ShaderNodeFactory::logicalOr(node, rhs.node));
            ret.promoteToVariable(ret.node);
            return ret;
        }

        ShaderNodeWrapper operator&&(const ShaderNodeWrapper &rhs) const {
            auto ret = ShaderNodeWrapper(ShaderDataType::boolean(),
                                         ShaderNodeFactory::logicalAnd(node, rhs.node));
            ret.promoteToVariable(ret.node);
            return ret;
        }

        ShaderNodeWrapper operator[](const ShaderNodeWrapper &rhs) {
            promoteToVariable(node);
            return ShaderNodeWrapper({type.type, type.component, 1},
                                     ShaderNodeFactory::subscriptArray(node, rhs.node));
        }

    protected:
        /**
         * Because the c++ compiler might elide the assignment operator invocation by creating the object in place for something like:
         * vec2 v = vec2(1, 1);
         * I cannot rely on the assignment operator to handle variable definition.
         *
         * Therefore, every object (Except default constructed objects) starts out as a temporary object
         * and is promoted to a variable as soon as it's either
         * - Written to through the assignment operator,
         * - The subscripting operator or a swizzle method is invoked on the object
         * - It is the result of an arithmetic operator (To preserve the order of the expression).
         *
         * This results in variables getting inlined if they are not promoted,
         * which can cause expensive operations to run multiple times
         * and also makes the resulting shader node tree harder to debug.
         *
         * Default constructed objects are variables,
         * so variable definition can be forced by default constructing the variable. (e.g. "vec2 v;")
         *
         * So for optimal performing shader code variables should be
         * default constructed and then assigned in separate statements. (e.g. "vec2 v; v = vec2(0, 0);)
         *
         * TODO: Fix variable assignment in conditional or loop bodies
         * Currently, there's a problem where if a variable is initialized in place and then assigned in a branch or loop body,
         * the variable will be defined inside the loop body or branch even if it is used later outside the scope of the branch or loop.
         */
        void promoteToVariable(const std::unique_ptr<ShaderNode> &variableInitializer) {
            if (node == nullptr
                || node->getType() != ShaderNode::VARIABLE) {
                const auto varName = ShaderBuilder::instance().getVariableName();
                ShaderBuilder::instance().addNode(
                    ShaderNodeFactory::createVariable(varName,
                                                      type,
                                                      variableInitializer ? variableInitializer->copy() : nullptr));
                node = ShaderNodeFactory::variable(varName);
            }
        }

        ShaderNodeWrapper swizzle_vec2(const ShaderNodeWrapper &x, const ShaderNodeWrapper &y) {
            promoteToVariable(node);
            return {
                ShaderDataType{ShaderDataType::VECTOR2, type.component, 1},
                ShaderNodeFactory::vectorSwizzle(node, {
                                                       down_cast<NodeVectorSwizzle &>(*x.node).indices.at(0),
                                                       down_cast<NodeVectorSwizzle &>(*y.node).indices.at(0),
                                                   })
            };
        }

        ShaderNodeWrapper swizzle_vec3(const ShaderNodeWrapper &x,
                                       const ShaderNodeWrapper &y,
                                       const ShaderNodeWrapper &z) {
            promoteToVariable(node);
            return {
                ShaderDataType{ShaderDataType::VECTOR3, type.component, 1},
                ShaderNodeFactory::vectorSwizzle(node, {
                                                       down_cast<NodeVectorSwizzle &>(*x.node).indices.at(0),
                                                       down_cast<NodeVectorSwizzle &>(*y.node).indices.at(0),
                                                       down_cast<NodeVectorSwizzle &>(*z.node).indices.at(0)
                                                   })
            };
        }

        ShaderNodeWrapper swizzle_vec4(const ShaderNodeWrapper &x,
                                       const ShaderNodeWrapper &y,
                                       const ShaderNodeWrapper &z,
                                       const ShaderNodeWrapper &w) {
            promoteToVariable(node);
            return {
                ShaderDataType{ShaderDataType::VECTOR4, type.component, 1},
                ShaderNodeFactory::vectorSwizzle(node, {
                                                       down_cast<NodeVectorSwizzle &>(*x.node).indices.at(0),
                                                       down_cast<NodeVectorSwizzle &>(*y.node).indices.at(0),
                                                       down_cast<NodeVectorSwizzle &>(*z.node).indices.at(0),
                                                       down_cast<NodeVectorSwizzle &>(*w.node).indices.at(0)
                                                   })
            };
        }
    };

    /**
     * A node wrapper with specific type information in a value template.
     *
     * This allows users to create variables in the style of glsl through the typedefs in fgshadernodehelper.hpp
     *
     * @tparam VALUE_TYPE
     * @tparam VALUE_COMPONENT
     * @tparam VALUE_COUNT
     */
    template<ShaderDataType::Type VALUE_TYPE, ShaderDataType::Component VALUE_COMPONENT, size_t VALUE_COUNT>
    class ShaderNodeWrapperTyped : public ShaderNodeWrapper {
    public:
        /**
         * Default constructor creates new variable
         */
        ShaderNodeWrapperTyped()
            : ShaderNodeWrapper({VALUE_TYPE, VALUE_COMPONENT, VALUE_COUNT}, nullptr) {
            type = {VALUE_TYPE, VALUE_COMPONENT, VALUE_COUNT};
            promoteToVariable(node);
        }

        ShaderNodeWrapperTyped(const ShaderNodeWrapper &base)
            : ShaderNodeWrapper(base) {
        }

        ShaderNodeWrapperTyped(const ShaderNodeWrapperTyped &other)
            : ShaderNodeWrapper(other) {
        }

        ShaderNodeWrapperTyped(ShaderNodeWrapperTyped &&other) noexcept
            : ShaderNodeWrapper(std::move(other)) {
        }

        ShaderNodeWrapper &operator=(const ShaderNodeWrapperTyped &other) {
            return ShaderNodeWrapper::operator=(other);
        }

        ShaderNodeWrapper &operator=(ShaderNodeWrapperTyped &&other) noexcept {
            return ShaderNodeWrapper::operator=(std::move(other));
        }

        ShaderNodeWrapper &operator=(const ShaderNodeWrapper &other) {
            return ShaderNodeWrapper::operator=(other);
        }

        ShaderNodeWrapper &operator=(ShaderNodeWrapper &&other) noexcept {
            return ShaderNodeWrapper::operator=(std::move(other));
        }

        // Array constructor
        ShaderNodeWrapperTyped(const std::vector<ShaderNodeWrapperTyped<VALUE_TYPE, VALUE_COMPONENT, 1> > &values)
            : ShaderNodeWrapper({VALUE_TYPE, VALUE_COMPONENT, VALUE_COUNT},
                                ShaderNodeFactory::array({
                                                             VALUE_TYPE,
                                                             VALUE_COMPONENT, 1
                                                         }, getNodes(values))) {
        }

        // Vector constructors
        ShaderNodeWrapperTyped(const ShaderNodeWrapperTyped<ShaderDataType::SCALAR, VALUE_COMPONENT, 1> &x,
                               const ShaderNodeWrapperTyped<ShaderDataType::SCALAR, VALUE_COMPONENT, 1> &y)
            : ShaderNodeWrapper({VALUE_TYPE, VALUE_COMPONENT, VALUE_COUNT},
                                ShaderNodeFactory::vector({VALUE_TYPE, VALUE_COMPONENT, VALUE_COUNT},
                                                          x.node,
                                                          y.node)) {
        }

        ShaderNodeWrapperTyped(const ShaderNodeWrapperTyped<ShaderDataType::SCALAR, VALUE_COMPONENT, 1> &x,
                               const ShaderNodeWrapperTyped<ShaderDataType::SCALAR, VALUE_COMPONENT, 1> &y,
                               const ShaderNodeWrapperTyped<ShaderDataType::SCALAR, VALUE_COMPONENT, 1> &z)
            : ShaderNodeWrapper({VALUE_TYPE, VALUE_COMPONENT, VALUE_COUNT},
                                ShaderNodeFactory::vector({VALUE_TYPE, VALUE_COMPONENT, VALUE_COUNT},
                                                          x.node,
                                                          y.node,
                                                          z.node)) {
        }

        ShaderNodeWrapperTyped(const ShaderNodeWrapperTyped<ShaderDataType::SCALAR, VALUE_COMPONENT, 1> &x,
                               const ShaderNodeWrapperTyped<ShaderDataType::SCALAR, VALUE_COMPONENT, 1> &y,
                               const ShaderNodeWrapperTyped<ShaderDataType::SCALAR, VALUE_COMPONENT, 1> &z,
                               const ShaderNodeWrapperTyped<ShaderDataType::SCALAR, VALUE_COMPONENT, 1> &w)
            : ShaderNodeWrapper({VALUE_TYPE, VALUE_COMPONENT, VALUE_COUNT},
                                ShaderNodeFactory::vector({VALUE_TYPE, VALUE_COMPONENT, VALUE_COUNT},
                                                          x.node,
                                                          y.node,
                                                          z.node,
                                                          w.node)) {
        }

        ShaderNodeWrapperTyped(const bool literal)
            : ShaderNodeWrapper({VALUE_TYPE, VALUE_COMPONENT, VALUE_COUNT},
                                ShaderNodeFactory::literal(literal)) {
        }

        ShaderNodeWrapperTyped(const int literal)
            : ShaderNodeWrapper({VALUE_TYPE, VALUE_COMPONENT, VALUE_COUNT},
                                ShaderNodeFactory::literal(literal)) {
        }

        ShaderNodeWrapperTyped(const unsigned int literal)
            : ShaderNodeWrapper({VALUE_TYPE, VALUE_COMPONENT, VALUE_COUNT},
                                ShaderNodeFactory::literal(literal)) {
        }

        ShaderNodeWrapperTyped(const float literal)
            : ShaderNodeWrapper({VALUE_TYPE, VALUE_COMPONENT, VALUE_COUNT},
                                ShaderNodeFactory::literal(literal)) {
        }

        ShaderNodeWrapperTyped(const double literal)
            : ShaderNodeWrapper({VALUE_TYPE, VALUE_COMPONENT, VALUE_COUNT},
                                ShaderNodeFactory::literal(literal)) {
        }

    private:
        static std::vector<std::unique_ptr<ShaderNode> > getNodes(const std::vector<ShaderNodeWrapper> &values) {
            std::vector<std::unique_ptr<ShaderNode> > nodes;
            for (auto &value: values) {
                nodes.push_back(value.node->copy());
            }
            return nodes;
        }
    };
}

#endif //XENGINE_SHADERNODEWRAPPER_HPP
