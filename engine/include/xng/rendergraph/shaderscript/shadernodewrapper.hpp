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
            return {{ShaderDataType::SCALAR, type.component}, ShaderNodeFactory::getX(node)};
        }

        [[nodiscard]] ShaderNodeWrapper y() {
            return {{ShaderDataType::SCALAR, type.component}, ShaderNodeFactory::getY(node)};
        }

        [[nodiscard]] ShaderNodeWrapper z() {
            return {{ShaderDataType::SCALAR, type.component}, ShaderNodeFactory::getZ(node)};
        }

        [[nodiscard]] ShaderNodeWrapper w() {
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
            if (node->getType() != ShaderNode::VARIABLE) {
                throw std::runtime_error(".element() invoked on a copy initialized variable");
            }
            return ShaderNodeWrapper({ShaderDataType::SCALAR, type.component},
                                     ShaderNodeFactory::matrixSubscript(node, column.node, row.node));
        }

        ShaderNodeWrapper column(const ShaderNodeWrapper &column) {
            if (node->getType() != ShaderNode::VARIABLE) {
                throw std::runtime_error(".column() invoked on a copy initialized variable");
            }
            switch (type.type) {
                case ShaderDataType::MAT2:
                    return ShaderNodeWrapper({ShaderDataType::VECTOR2, type.component},
                                             ShaderNodeFactory::matrixSubscript(node, column.node, nullptr));
                case ShaderDataType::MAT3:
                    return ShaderNodeWrapper({ShaderDataType::VECTOR3, type.component},
                                             ShaderNodeFactory::matrixSubscript(node, column.node, nullptr));
                case ShaderDataType::MAT4:
                    return ShaderNodeWrapper({ShaderDataType::VECTOR4, type.component},
                                             ShaderNodeFactory::matrixSubscript(node, column.node, nullptr));
                default:
                    throw std::runtime_error("column call on non matrix");
            }
        }

        /**
         * The move and copy assignment operators are overloaded to translate logical assignments to shader node operations.
         *
         * This enables the user to assign variables in glsl style.
         *
         * @param rhs
         * @return
         */
        ShaderNodeWrapper &operator=(ShaderNodeWrapper &&rhs) {
            assignValue(rhs);
            return *this;
        }

        /**
         * The move and copy assignment operators are overloaded to translate logical assignments to shader node operations.
         *
         * This enables the user to assign variables in glsl style.
         *
         * @param rhs
         * @return
         */
        ShaderNodeWrapper &operator=(const ShaderNodeWrapper &rhs) {
            assignValue(rhs);
            return *this;
        }

        // TODO: Handle implicit type promotion
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
            return ShaderNodeWrapper(outType, ShaderNodeFactory::divide(node, rhs.node));
        }

        ShaderNodeWrapper operator+=(const ShaderNodeWrapper &rhs) {
            assignValue(*this + rhs);
            return *this;
        }

        ShaderNodeWrapper operator-=(const ShaderNodeWrapper &rhs) {
            assignValue(*this - rhs);
            return *this;
        }

        ShaderNodeWrapper operator*=(const ShaderNodeWrapper &rhs) {
            assignValue(*this * rhs);
            return *this;
        }

        ShaderNodeWrapper operator/=(const ShaderNodeWrapper &rhs) {
            assignValue(*this / rhs);
            return *this;
        }

        ShaderNodeWrapper operator==(const ShaderNodeWrapper &rhs) const {
            auto ret = ShaderNodeWrapper(ShaderDataType::boolean(),
                                         ShaderNodeFactory::compareEqual(node, rhs.node));
            return ret;
        }

        ShaderNodeWrapper operator!=(const ShaderNodeWrapper &rhs) const {
            auto ret = ShaderNodeWrapper(ShaderDataType::boolean(),
                                         ShaderNodeFactory::compareNotEqual(node, rhs.node));
            return ret;
        }

        ShaderNodeWrapper operator<(const ShaderNodeWrapper &rhs) const {
            auto ret = ShaderNodeWrapper(ShaderDataType::boolean(),
                                         ShaderNodeFactory::compareLess(node, rhs.node));
            return ret;
        }

        ShaderNodeWrapper operator>(const ShaderNodeWrapper &rhs) const {
            auto ret = ShaderNodeWrapper(ShaderDataType::boolean(),
                                         ShaderNodeFactory::compareGreater(node, rhs.node));
            return ret;
        }

        ShaderNodeWrapper operator <=(const ShaderNodeWrapper &rhs) const {
            auto ret = ShaderNodeWrapper(ShaderDataType::boolean(),
                                         ShaderNodeFactory::compareLessEqual(node, rhs.node));
            return ret;
        }

        ShaderNodeWrapper operator >=(const ShaderNodeWrapper &rhs) const {
            auto ret = ShaderNodeWrapper(ShaderDataType::boolean(),
                                         ShaderNodeFactory::compareGreaterEqual(node, rhs.node));
            return ret;
        }

        ShaderNodeWrapper operator||(const ShaderNodeWrapper &rhs) const {
            auto ret = ShaderNodeWrapper(ShaderDataType::boolean(),
                                         ShaderNodeFactory::logicalOr(node, rhs.node));
            return ret;
        }

        ShaderNodeWrapper operator&&(const ShaderNodeWrapper &rhs) const {
            auto ret = ShaderNodeWrapper(ShaderDataType::boolean(),
                                         ShaderNodeFactory::logicalAnd(node, rhs.node));
            return ret;
        }

        ShaderNodeWrapper operator[](const ShaderNodeWrapper &rhs) {
            return ShaderNodeWrapper({type.type, type.component, 1},
                                     ShaderNodeFactory::arraySubscript(node, rhs.node));
        }

    protected:
        ShaderNodeWrapper swizzle_vec2(const ShaderNodeWrapper &x, const ShaderNodeWrapper &y) {
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
        void assignValue(const ShaderNodeWrapper &value) {
            if (node->getType() == ShaderNode::VARIABLE_CREATE) {
                // Assign to uninitialized variable
                auto &varNode = down_cast<const NodeVariableCreate &>(*node);
                ShaderBuilder::instance().addNode(
                    ShaderNodeFactory::createVariable(varNode.variableName,
                                                      type,
                                                      value.node));
                node = ShaderNodeFactory::variable(varNode.variableName);
            } else if (node->getType() == ShaderNode::VECTOR_SWIZZLE) {
                // Allow assignment to swizzle of a vector variable
                auto &swizzleNode = down_cast<const NodeVectorSwizzle &>(*node);
                if (swizzleNode.vector->getType() != ShaderNode::VARIABLE
                    && swizzleNode.vector->getType() != ShaderNode::ATTRIBUTE_OUT
                    && swizzleNode.vector->getType() != ShaderNode::BUFFER) {
                    throw std::runtime_error(
                        "Invalid vector swizzle assignment (Copy initialized or uninitialized variable)");
                }
                ShaderBuilder::instance().addNode(ShaderNodeFactory::assign(node, value.node));
            } else if (node->getType() == ShaderNode::SUBSCRIPT_MATRIX) {
                // Allow assignment to subscript of a matrix variable
                auto &subNode = down_cast<const NodeMatrixSubscript &>(*node);
                if (subNode.matrix->getType() != ShaderNode::VARIABLE
                    && subNode.matrix->getType() != ShaderNode::ATTRIBUTE_OUT
                    && subNode.matrix->getType() != ShaderNode::BUFFER) {
                    throw std::runtime_error(
                        "Invalid matrix subscript assignment (Copy initialized or uninitialized variable)");
                }
                ShaderBuilder::instance().addNode(ShaderNodeFactory::assign(node, value.node));
            } else if (node->getType() == ShaderNode::SUBSCRIPT_ARRAY) {
                // Allow assignment to a subscript of a vector variable
                auto &subNode = down_cast<const NodeArraySubscript &>(*node);
                if (subNode.array->getType() != ShaderNode::VARIABLE
                    && subNode.array->getType() != ShaderNode::ATTRIBUTE_OUT
                    && subNode.array->getType() != ShaderNode::BUFFER) {
                    throw std::runtime_error(
                        "Invalid array subscript assignment (Copy initialized or uninitialized variable)");
                }
                ShaderBuilder::instance().addNode(ShaderNodeFactory::assign(node, value.node));
            } else if (node->getType() == ShaderNode::VARIABLE
                       || node->getType() == ShaderNode::ATTRIBUTE_OUT
                       || node->getType() == ShaderNode::BUFFER) {
                ShaderBuilder::instance().addNode(ShaderNodeFactory::assign(node, value.node));
            } else {
                throw std::runtime_error("Invalid assignment (Copy initialized variable or temporary)");
            }
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
         * Default constructor creates new variable.
         */
        ShaderNodeWrapperTyped()
            : ShaderNodeWrapper({VALUE_TYPE, VALUE_COMPONENT, VALUE_COUNT}, nullptr) {
            type = {VALUE_TYPE, VALUE_COMPONENT, VALUE_COUNT};
            const auto varName = ShaderBuilder::instance().getVariableName();
            node = ShaderNodeFactory::createVariable(varName,
                                                     type,
                                                     nullptr);
        }

        // Copy operators and constructors for non prvalues are deleted to force move semantics and avoid copy eliding.
        ShaderNodeWrapperTyped(const ShaderNodeWrapper &base) = delete;

        ShaderNodeWrapper &operator=(const ShaderNodeWrapper &other) = delete;

        // Copy operators and constructors for prvalues are allowed to support copying a variable (e.g. vec2 v = other; or vec2 v; v = other)
        ShaderNodeWrapperTyped(const ShaderNodeWrapperTyped &other)
            : ShaderNodeWrapper(other) {
        }

        ShaderNodeWrapper &operator=(const ShaderNodeWrapperTyped &other) {
            return ShaderNodeWrapper::operator=(other);
        }

        // Move constructor can be deleted because in the dsl syntax variables are not initialized with vec2 v(other);
        ShaderNodeWrapperTyped(ShaderNodeWrapperTyped &&other) noexcept = delete;

        // Move operator is allowed to support assignments to default initialized variables from prvalues (e.g. vec2 v; v = vec2(0, 0);)
        ShaderNodeWrapper &operator=(ShaderNodeWrapperTyped &&other) noexcept {
            return ShaderNodeWrapper::operator=(std::move(other));
        }

        // Move operations from non prvalues, creates and initializes a variable.
        ShaderNodeWrapperTyped(ShaderNodeWrapper &&other) noexcept
            : ShaderNodeWrapper(std::move(other)) {
            type = {VALUE_TYPE, VALUE_COMPONENT, VALUE_COUNT};
            const auto varName = ShaderBuilder::instance().getVariableName();
            ShaderBuilder::instance().addNode(ShaderNodeFactory::createVariable(varName,
                type,
                node));
            node = ShaderNodeFactory::variable(varName);
        }

        ShaderNodeWrapper &operator=(ShaderNodeWrapper &&other) {
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
        ShaderNodeWrapperTyped(const ShaderNodeWrapper &x,
                               const ShaderNodeWrapper &y)
            : ShaderNodeWrapper({VALUE_TYPE, VALUE_COMPONENT, VALUE_COUNT},
                                ShaderNodeFactory::vector({VALUE_TYPE, VALUE_COMPONENT, VALUE_COUNT},
                                                          x.node,
                                                          y.node)) {
            static_assert(VALUE_TYPE == ShaderDataType::VECTOR4
                          || VALUE_TYPE == ShaderDataType::VECTOR3
                          || VALUE_TYPE == ShaderDataType::VECTOR2);
        }

        ShaderNodeWrapperTyped(const ShaderNodeWrapper &x,
                               const ShaderNodeWrapper &y,
                               const ShaderNodeWrapper &z)
            : ShaderNodeWrapper({VALUE_TYPE, VALUE_COMPONENT, VALUE_COUNT},
                                ShaderNodeFactory::vector({VALUE_TYPE, VALUE_COMPONENT, VALUE_COUNT},
                                                          x.node,
                                                          y.node,
                                                          z.node)) {
            static_assert(VALUE_TYPE == ShaderDataType::VECTOR4
                          || VALUE_TYPE == ShaderDataType::VECTOR3);
        }

        ShaderNodeWrapperTyped(const ShaderNodeWrapper &x,
                               const ShaderNodeWrapper &y,
                               const ShaderNodeWrapper &z,
                               const ShaderNodeWrapper &w)
            : ShaderNodeWrapper({VALUE_TYPE, VALUE_COMPONENT, VALUE_COUNT},
                                ShaderNodeFactory::vector({VALUE_TYPE, VALUE_COMPONENT, VALUE_COUNT},
                                                          x.node,
                                                          y.node,
                                                          z.node,
                                                          w.node)) {
            static_assert(VALUE_TYPE == ShaderDataType::VECTOR4);
        }

        // Literal Constructors
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
        static std::vector<std::unique_ptr<ShaderNode> > getNodes(
            const std::vector<ShaderNodeWrapperTyped<VALUE_TYPE, VALUE_COMPONENT, 1> > &values) {
            std::vector<std::unique_ptr<ShaderNode> > nodes;
            for (auto &value: values) {
                nodes.push_back(value.node->copy());
            }
            return nodes;
        }
    };
}

#endif //XENGINE_SHADERNODEWRAPPER_HPP
