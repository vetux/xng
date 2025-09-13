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

#include "xng/rendergraph/shader/shadernodefactory.hpp"
#include "xng/rendergraph/shader/nodes.hpp"

#include "xng/rendergraph/shaderscript/shaderbuilder.hpp"

namespace xng::ShaderScript {
    class ShaderNodeWrapper {
    public:
        ShaderDataType type;
        std::unique_ptr<ShaderNode> node{};

        ShaderNodeWrapper(const ShaderDataType type,
                          std::unique_ptr<ShaderNode> valueNode,
                          bool createTempVariable = true)
            : type(type) {
            if (createTempVariable) {
                const auto varName = ShaderBuilder::instance().getVariableName();
                ShaderBuilder::instance().addNode(
                    ShaderNodeFactory::createVariable(varName, type, valueNode));
                node = ShaderNodeFactory::variable(varName);
            } else {
                node = std::move(valueNode);
            }
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

        [[nodiscard]] ShaderNodeWrapper x() const {
            return ShaderNodeWrapper({ShaderDataType::SCALAR, type.component},
                                     ShaderNodeFactory::subscriptVector(node, 0));
        }

        [[nodiscard]] ShaderNodeWrapper y() const {
            return ShaderNodeWrapper({ShaderDataType::SCALAR, type.component},
                                     ShaderNodeFactory::subscriptVector(node, 1));
        }

        [[nodiscard]] ShaderNodeWrapper z() const {
            return ShaderNodeWrapper({ShaderDataType::SCALAR, type.component},
                                     ShaderNodeFactory::subscriptVector(node, 2));
        }

        [[nodiscard]] ShaderNodeWrapper w() const {
            return ShaderNodeWrapper({ShaderDataType::SCALAR, type.component},
                                     ShaderNodeFactory::subscriptVector(node, 3));
        }

        [[nodiscard]] ShaderNodeWrapper element(const ShaderNodeWrapper &column,
                                                const ShaderNodeWrapper &row) const {
            return ShaderNodeWrapper({ShaderDataType::SCALAR, type.component},
                                     ShaderNodeFactory::subscriptMatrix(node, column.node, row.node));
        }

        void setX(const ShaderNodeWrapper &value) const {
            ShaderBuilder::instance().addNode(ShaderNodeFactory::assign(ShaderNodeFactory::subscriptVector(node, 0),
                                                                        value.node));
        }

        void setY(const ShaderNodeWrapper &value) const {
            ShaderBuilder::instance().addNode(ShaderNodeFactory::assign(ShaderNodeFactory::subscriptVector(node, 1),
                                                                        value.node));
        }

        void setZ(const ShaderNodeWrapper &value) const {
            ShaderBuilder::instance().addNode(ShaderNodeFactory::assign(ShaderNodeFactory::subscriptVector(node, 2),
                                                                        value.node));
        }

        void setW(const ShaderNodeWrapper &value) const {
            ShaderBuilder::instance().addNode(ShaderNodeFactory::assign(ShaderNodeFactory::subscriptVector(node, 3),
                                                                        value.node));
        }

        void setElement(const ShaderNodeWrapper &column,
                        const ShaderNodeWrapper &row,
                        const ShaderNodeWrapper &value) const {
            ShaderBuilder::instance().addNode(ShaderNodeFactory::assign(
                ShaderNodeFactory::subscriptMatrix(node,
                                                   row.node,
                                                   column.node),
                value.node));
        }

        void setElement(const ShaderNodeWrapper &index, const ShaderNodeWrapper &value) const {
            ShaderBuilder::instance().addNode(ShaderNodeFactory::assign(
                ShaderNodeFactory::subscriptArray(node, index.node),
                value.node));
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
        ShaderNodeWrapper &operator=(ShaderNodeWrapper &&rhs) {
            // Hook logical assignments to variables
            if (node->getType() == ShaderNode::VARIABLE) {
                // Assign value
                ShaderBuilder::instance().addNode(ShaderNodeFactory::assign(node, rhs.node));
                return *this;
            }
            // Not supported
            throw std::runtime_error("Node wrapper move assignment not supported.");
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
            if (node->getType() == ShaderNode::VARIABLE) {
                // Assign value
                ShaderBuilder::instance().addNode(ShaderNodeFactory::assign(node, rhs.node));
                return *this;
            }
            throw std::runtime_error("Node wrapper copy assignment not supported.");
        }

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
            return ShaderNodeWrapper(outType, ShaderNodeFactory::add(node, rhs.node));
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
            return ShaderNodeWrapper(outType, ShaderNodeFactory::subtract(node, rhs.node));
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
            return ShaderNodeWrapper(outType, ShaderNodeFactory::multiply(node, rhs.node));
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

        ShaderNodeWrapper operator+=(const ShaderNodeWrapper &rhs) const {
            if (node->getType() != ShaderNode::VARIABLE) {
                throw std::runtime_error("Assignment to non-variable node wrapper not supported.");
            }
            ShaderBuilder::instance().addNode(ShaderNodeFactory::assign(node, ShaderNodeFactory::add(node, rhs.node)));
            return *this;
        }

        ShaderNodeWrapper operator-=(const ShaderNodeWrapper &rhs) const {
            if (node->getType() != ShaderNode::VARIABLE) {
                throw std::runtime_error("Assignment to non-variable node wrapper not supported.");
            }
            ShaderBuilder::instance().addNode(
                ShaderNodeFactory::assign(node, ShaderNodeFactory::subtract(node, rhs.node)));
            return *this;
        }

        ShaderNodeWrapper operator*=(const ShaderNodeWrapper &rhs) const {
            if (node->getType() != ShaderNode::VARIABLE) {
                throw std::runtime_error("Assignment to non-variable node wrapper not supported.");
            }
            ShaderBuilder::instance().addNode(
                ShaderNodeFactory::assign(node, ShaderNodeFactory::multiply(node, rhs.node)));
            return *this;
        }

        ShaderNodeWrapper operator/=(const ShaderNodeWrapper &rhs) const {
            if (node->getType() != ShaderNode::VARIABLE) {
                throw std::runtime_error("Assignment to non-variable node wrapper not supported.");
            }
            ShaderBuilder::instance().addNode(
                ShaderNodeFactory::assign(node, ShaderNodeFactory::divide(node, rhs.node)));
            return *this;
        }

        ShaderNodeWrapper operator==(const ShaderNodeWrapper &rhs) const {
            return ShaderNodeWrapper(ShaderDataType::boolean(),
                                     ShaderNodeFactory::compareEqual(node, rhs.node));
        }

        ShaderNodeWrapper operator!=(const ShaderNodeWrapper &rhs) const {
            return ShaderNodeWrapper(ShaderDataType::boolean(),
                                     ShaderNodeFactory::compareNotEqual(node, rhs.node));
        }

        ShaderNodeWrapper operator<(const ShaderNodeWrapper &rhs) const {
            return ShaderNodeWrapper(ShaderDataType::boolean(),
                                     ShaderNodeFactory::compareLess(node, rhs.node));
        }

        ShaderNodeWrapper operator>(const ShaderNodeWrapper &rhs) const {
            return ShaderNodeWrapper(ShaderDataType::boolean(),
                                     ShaderNodeFactory::compareGreater(node, rhs.node));
        }

        ShaderNodeWrapper operator <=(const ShaderNodeWrapper &rhs) const {
            return ShaderNodeWrapper(ShaderDataType::boolean(),
                                     ShaderNodeFactory::compareLessEqual(node, rhs.node));
        }

        ShaderNodeWrapper operator >=(const ShaderNodeWrapper &rhs) const {
            return ShaderNodeWrapper(ShaderDataType::boolean(),
                                     ShaderNodeFactory::compareGreaterEqual(node, rhs.node));
        }

        ShaderNodeWrapper operator||(const ShaderNodeWrapper &rhs) const {
            return ShaderNodeWrapper(ShaderDataType::boolean(),
                                     ShaderNodeFactory::logicalOr(node, rhs.node));
        }

        ShaderNodeWrapper operator&&(const ShaderNodeWrapper &rhs) const {
            return ShaderNodeWrapper(ShaderDataType::boolean(),
                                     ShaderNodeFactory::logicalAnd(node, rhs.node));
        }

        ShaderNodeWrapper operator[](const ShaderNodeWrapper &rhs) const {
            return ShaderNodeWrapper({type.type, type.component, 1},
                                     ShaderNodeFactory::subscriptArray(node, rhs.node));
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
        ShaderNodeWrapperTyped() = default;

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
