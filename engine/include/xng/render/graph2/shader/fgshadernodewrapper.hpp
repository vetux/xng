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

#ifndef XENGINE_FGSHADERNODEWRAPPER_HPP
#define XENGINE_FGSHADERNODEWRAPPER_HPP

#include "xng/render/graph2/shader/fgshaderfactory.hpp"
#include "xng/render/graph2/shader/nodes.hpp"

namespace xng {
    class FGShaderNodeWrapper {
    public:
        FGShaderValue type;
        std::unique_ptr<FGShaderNode> node{};

        FGShaderNodeWrapper(const FGShaderValue type,
                            std::unique_ptr<FGShaderNode> valueNode,
                            bool createTempVariable = true)
            : type(type) {
            if (createTempVariable) {
                const auto varName = FGShaderBuilder::instance().getVariableName();
                FGShaderBuilder::instance().addNode(
                    FGShaderFactory::createVariable(varName, type, std::move(valueNode)));
                node = FGShaderFactory::variable(varName);
            } else {
                node = std::move(valueNode);
            }
        }

        FGShaderNodeWrapper(const FGShaderNodeWrapper &other) {
            type = other.type;
            node = other.node->copy();
        }

        FGShaderNodeWrapper(FGShaderNodeWrapper &&other) noexcept {
            type = other.type;
            node = std::move(other.node);
        }

        // Literal constructor
        FGShaderNodeWrapper(const bool literal)
            : FGShaderNodeWrapper(getLiteralType(literal),
                                  FGShaderFactory::literal(literal)) {
        }

        FGShaderNodeWrapper(const int literal)
            : FGShaderNodeWrapper(getLiteralType(literal),
                                  FGShaderFactory::literal(literal)) {
        }

        FGShaderNodeWrapper(const unsigned int literal)
            : FGShaderNodeWrapper(getLiteralType(literal),
                                  FGShaderFactory::literal(literal)) {
        }

        FGShaderNodeWrapper(const float literal)
            : FGShaderNodeWrapper(getLiteralType(literal),
                                  FGShaderFactory::literal(literal)) {
        }

        FGShaderNodeWrapper(const double literal)
            : FGShaderNodeWrapper(getLiteralType(literal),
                                  FGShaderFactory::literal(literal)) {
        }

        [[nodiscard]] FGShaderNodeWrapper x() const {
            return FGShaderNodeWrapper({FGShaderValue::SCALAR, type.component},
                                       FGShaderFactory::subscriptVector(node, 0));
        }

        [[nodiscard]] FGShaderNodeWrapper y() const {
            return FGShaderNodeWrapper({FGShaderValue::SCALAR, type.component},
                                       FGShaderFactory::subscriptVector(node, 1));
        }

        [[nodiscard]] FGShaderNodeWrapper z() const {
            return FGShaderNodeWrapper({FGShaderValue::SCALAR, type.component},
                                       FGShaderFactory::subscriptVector(node, 2));
        }

        [[nodiscard]] FGShaderNodeWrapper w() const {
            return FGShaderNodeWrapper({FGShaderValue::SCALAR, type.component},
                                       FGShaderFactory::subscriptVector(node, 3));
        }

        [[nodiscard]] FGShaderNodeWrapper element(const FGShaderNodeWrapper &column,
                                                  const FGShaderNodeWrapper &row) const {
            return FGShaderNodeWrapper({FGShaderValue::SCALAR, type.component},
                                       FGShaderFactory::subscriptMatrix(node, column.node, row.node));
        }


        void setX(const FGShaderNodeWrapper &value) const {
            FGShaderBuilder::instance().addNode(FGShaderFactory::assign(FGShaderFactory::subscriptVector(node, 0),
                                                                        value.node));
        }

        void setY(const FGShaderNodeWrapper &value) const {
            FGShaderBuilder::instance().addNode(FGShaderFactory::assign(FGShaderFactory::subscriptVector(node, 1),
                                                                        value.node));
        }

        void setZ(const FGShaderNodeWrapper &value) const {
            FGShaderBuilder::instance().addNode(FGShaderFactory::assign(FGShaderFactory::subscriptVector(node, 2),
                                                                        value.node));
        }

        void setW(const FGShaderNodeWrapper &value) const {
            FGShaderBuilder::instance().addNode(FGShaderFactory::assign(FGShaderFactory::subscriptVector(node, 3),
                                                                        value.node));
        }

        void setElement(const FGShaderNodeWrapper &column,
                        const FGShaderNodeWrapper &row,
                        const FGShaderNodeWrapper &value) const {
            FGShaderBuilder::instance().addNode(FGShaderFactory::assign(
                FGShaderFactory::subscriptMatrix(node,
                                                 row.node,
                                                 column.node),
                value.node));
        }

        void setElement(const FGShaderNodeWrapper &index, const FGShaderNodeWrapper &value) const {
            FGShaderBuilder::instance().addNode(FGShaderFactory::assign(
                FGShaderFactory::subscriptArray(node, index.node),
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
        FGShaderNodeWrapper &operator=(FGShaderNodeWrapper &&rhs) {
            // Hook logical assignments to variables
            if (node->getType() == FGShaderNode::VARIABLE) {
                // Assign value
                FGShaderBuilder::instance().addNode(FGShaderFactory::assign(node, rhs.node));
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
        FGShaderNodeWrapper &operator=(const FGShaderNodeWrapper &rhs) {
            // Hook logical assignments
            if (node->getType() == FGShaderNode::VARIABLE) {
                // Assign value
                FGShaderBuilder::instance().addNode(FGShaderFactory::assign(node, rhs.node));
                return *this;
            }
            throw std::runtime_error("Node wrapper copy assignment not supported.");
        }

        FGShaderNodeWrapper operator+(const FGShaderNodeWrapper &rhs) const {
            FGShaderValue outType;
            if (type == rhs.type) {
                // Arithmetic with identical types
                outType = type;
            } else {
                // Mixed Arithmetic e.g. vec3 + float
                if (type.type == FGShaderValue::SCALAR) {
                    outType = rhs.type;
                } else {
                    outType = type;
                }
            }
            return FGShaderNodeWrapper(outType, FGShaderFactory::add(node, rhs.node));
        }

        FGShaderNodeWrapper operator-(const FGShaderNodeWrapper &rhs) const {
            FGShaderValue outType;
            if (type == rhs.type) {
                // Arithmetic with identical types
                outType = type;
            } else {
                // Mixed Arithmetic e.g. vec3 + float
                if (type.type == FGShaderValue::SCALAR) {
                    outType = rhs.type;
                } else {
                    outType = type;
                }
            }
            return FGShaderNodeWrapper(outType, FGShaderFactory::subtract(node, rhs.node));
        }

        FGShaderNodeWrapper operator*(const FGShaderNodeWrapper &rhs) const {
            FGShaderValue outType;
            if (type == rhs.type) {
                // Arithmetic with identical types
                outType = type;
            } else {
                // Mixed Arithmetic

                // Matrix * Vector or Vector * Matrix
                if (type.type == FGShaderValue::MAT4 || rhs.type.type == FGShaderValue::MAT4) {
                    outType = {FGShaderValue::VECTOR4, type.component, 1};
                } else if (type.type == FGShaderValue::MAT3 || rhs.type.type == FGShaderValue::MAT3) {
                    outType = {FGShaderValue::VECTOR3, type.component, 1};
                } else if (type.type == FGShaderValue::MAT2 || rhs.type.type == FGShaderValue::MAT2) {
                    outType = {FGShaderValue::VECTOR2, type.component, 1};
                } else {
                    // Scalar * Vector
                    if (type.type == FGShaderValue::SCALAR) {
                        outType = rhs.type;
                    } else {
                        outType = type;
                    }
                }
            }
            return FGShaderNodeWrapper(outType, FGShaderFactory::multiply(node, rhs.node));
        }

        FGShaderNodeWrapper operator/(const FGShaderNodeWrapper &rhs) const {
            FGShaderValue outType;
            if (type == rhs.type) {
                // Arithmetic with identical types
                outType = type;
            } else {
                // Mixed Arithmetic e.g. vec3 / float
                if (type.type == FGShaderValue::SCALAR) {
                    outType = rhs.type;
                } else {
                    outType = type;
                }
            }
            return FGShaderNodeWrapper(outType, FGShaderFactory::divide(node, rhs.node));
        }

        FGShaderNodeWrapper operator+=(const FGShaderNodeWrapper &rhs) const {
            if (node->getType() != FGShaderNode::VARIABLE) {
                throw std::runtime_error("Assignment to non-variable node wrapper not supported.");
            }
            FGShaderBuilder::instance().addNode(FGShaderFactory::assign(node, FGShaderFactory::add(node, rhs.node)));
            return *this;
        }

        FGShaderNodeWrapper operator-=(const FGShaderNodeWrapper &rhs) const {
            if (node->getType() != FGShaderNode::VARIABLE) {
                throw std::runtime_error("Assignment to non-variable node wrapper not supported.");
            }
            FGShaderBuilder::instance().addNode(FGShaderFactory::assign(node, FGShaderFactory::subtract(node, rhs.node)));
            return *this;
        }

        FGShaderNodeWrapper operator*=(const FGShaderNodeWrapper &rhs) const {
            if (node->getType() != FGShaderNode::VARIABLE) {
                throw std::runtime_error("Assignment to non-variable node wrapper not supported.");
            }
            FGShaderBuilder::instance().addNode(FGShaderFactory::assign(node, FGShaderFactory::multiply(node, rhs.node)));
            return *this;
        }

        FGShaderNodeWrapper operator/=(const FGShaderNodeWrapper &rhs) const {
            if (node->getType() != FGShaderNode::VARIABLE) {
                throw std::runtime_error("Assignment to non-variable node wrapper not supported.");
            }
            FGShaderBuilder::instance().addNode(FGShaderFactory::assign(node, FGShaderFactory::divide(node, rhs.node)));
            return *this;
        }

        FGShaderNodeWrapper operator==(const FGShaderNodeWrapper &rhs) const {
            return FGShaderNodeWrapper(FGShaderValue::boolean(),
                                       FGShaderFactory::compareEqual(node, rhs.node));
        }

        FGShaderNodeWrapper operator!=(const FGShaderNodeWrapper &rhs) const {
            return FGShaderNodeWrapper(FGShaderValue::boolean(),
                                       FGShaderFactory::compareNotEqual(node, rhs.node));
        }

        FGShaderNodeWrapper operator<(const FGShaderNodeWrapper &rhs) const {
            return FGShaderNodeWrapper(FGShaderValue::boolean(),
                                       FGShaderFactory::compareLess(node, rhs.node));
        }

        FGShaderNodeWrapper operator>(const FGShaderNodeWrapper &rhs) const {
            return FGShaderNodeWrapper(FGShaderValue::boolean(),
                                       FGShaderFactory::compareGreater(node, rhs.node));
        }

        FGShaderNodeWrapper operator <=(const FGShaderNodeWrapper &rhs) const {
            return FGShaderNodeWrapper(FGShaderValue::boolean(),
                                       FGShaderFactory::compareLessEqual(node, rhs.node));
        }

        FGShaderNodeWrapper operator >=(const FGShaderNodeWrapper &rhs) const {
            return FGShaderNodeWrapper(FGShaderValue::boolean(),
                                       FGShaderFactory::compareGreaterEqual(node, rhs.node));
        }

        FGShaderNodeWrapper operator||(const FGShaderNodeWrapper &rhs) const {
            return FGShaderNodeWrapper(FGShaderValue::boolean(),
                                       FGShaderFactory::logicalOr(node, rhs.node));
        }

        FGShaderNodeWrapper operator&&(const FGShaderNodeWrapper &rhs) const {
            return FGShaderNodeWrapper(FGShaderValue::boolean(),
                                       FGShaderFactory::logicalAnd(node, rhs.node));
        }

        FGShaderNodeWrapper operator[](const FGShaderNodeWrapper &rhs) const {
            return FGShaderNodeWrapper({type.type, type.component, 1},
                                       FGShaderFactory::subscriptArray(node, rhs.node));
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
    template<FGShaderValue::Type VALUE_TYPE, FGShaderValue::Component VALUE_COMPONENT, size_t VALUE_COUNT>
    class FGShaderNodeWrapperTyped : public FGShaderNodeWrapper {
    public:
        FGShaderNodeWrapperTyped() = default;

        FGShaderNodeWrapperTyped(const FGShaderNodeWrapper &base)
            : FGShaderNodeWrapper(base) {
        }

        FGShaderNodeWrapperTyped(const FGShaderNodeWrapperTyped &other)
            : FGShaderNodeWrapper(other) {
        }

        FGShaderNodeWrapperTyped(FGShaderNodeWrapperTyped &&other) noexcept
            : FGShaderNodeWrapper(std::move(other)) {
        }

        FGShaderNodeWrapper &operator=(const FGShaderNodeWrapperTyped &other) {
            return FGShaderNodeWrapper::operator=(other);
        }

        FGShaderNodeWrapper &operator=(FGShaderNodeWrapperTyped &&other) noexcept {
            return FGShaderNodeWrapper::operator=(std::move(other));
        }

        FGShaderNodeWrapper &operator=(const FGShaderNodeWrapper &other) {
            return FGShaderNodeWrapper::operator=(other);
        }

        FGShaderNodeWrapper &operator=(FGShaderNodeWrapper &&other) noexcept {
            return FGShaderNodeWrapper::operator=(std::move(other));
        }

        // Array constructor
        FGShaderNodeWrapperTyped(const std::vector<FGShaderNodeWrapperTyped<VALUE_TYPE, VALUE_COMPONENT, 1> > &values)
            : FGShaderNodeWrapper({VALUE_TYPE, VALUE_COMPONENT, VALUE_COUNT},
                                  FGShaderFactory::array({
                                                             VALUE_TYPE,
                                                             VALUE_COMPONENT, 1
                                                         }, getNodes(values))) {
        }

        // Vector constructors
        FGShaderNodeWrapperTyped(const FGShaderNodeWrapperTyped<FGShaderValue::SCALAR, VALUE_COMPONENT, 1> &x,
                                 const FGShaderNodeWrapperTyped<FGShaderValue::SCALAR, VALUE_COMPONENT, 1> &y)
            : FGShaderNodeWrapper({VALUE_TYPE, VALUE_COMPONENT, VALUE_COUNT},
                                  FGShaderFactory::vector({VALUE_TYPE, VALUE_COMPONENT, VALUE_COUNT},
                                                          x.node,
                                                          y.node)) {
        }

        FGShaderNodeWrapperTyped(const FGShaderNodeWrapperTyped<FGShaderValue::SCALAR, VALUE_COMPONENT, 1> &x,
                                 const FGShaderNodeWrapperTyped<FGShaderValue::SCALAR, VALUE_COMPONENT, 1> &y,
                                 const FGShaderNodeWrapperTyped<FGShaderValue::SCALAR, VALUE_COMPONENT, 1> &z)
            : FGShaderNodeWrapper({VALUE_TYPE, VALUE_COMPONENT, VALUE_COUNT},
                                  FGShaderFactory::vector({VALUE_TYPE, VALUE_COMPONENT, VALUE_COUNT},
                                                          x.node,
                                                          y.node,
                                                          z.node)) {
        }

        FGShaderNodeWrapperTyped(const FGShaderNodeWrapperTyped<FGShaderValue::SCALAR, VALUE_COMPONENT, 1> &x,
                                 const FGShaderNodeWrapperTyped<FGShaderValue::SCALAR, VALUE_COMPONENT, 1> &y,
                                 const FGShaderNodeWrapperTyped<FGShaderValue::SCALAR, VALUE_COMPONENT, 1> &z,
                                 const FGShaderNodeWrapperTyped<FGShaderValue::SCALAR, VALUE_COMPONENT, 1> &w)
            : FGShaderNodeWrapper({VALUE_TYPE, VALUE_COMPONENT, VALUE_COUNT},
                                  FGShaderFactory::vector({VALUE_TYPE, VALUE_COMPONENT, VALUE_COUNT},
                                                          x.node,
                                                          y.node,
                                                          z.node,
                                                          w.node)) {
        }

        FGShaderNodeWrapperTyped(const bool literal)
            : FGShaderNodeWrapper({VALUE_TYPE, VALUE_COMPONENT, VALUE_COUNT},
                                  FGShaderFactory::literal(literal)) {
        }

        FGShaderNodeWrapperTyped(const int literal)
            : FGShaderNodeWrapper({VALUE_TYPE, VALUE_COMPONENT, VALUE_COUNT},
                                  FGShaderFactory::literal(literal)) {
        }

        FGShaderNodeWrapperTyped(const unsigned int literal)
            : FGShaderNodeWrapper({VALUE_TYPE, VALUE_COMPONENT, VALUE_COUNT},
                                  FGShaderFactory::literal(literal)) {
        }

        FGShaderNodeWrapperTyped(const float literal)
            : FGShaderNodeWrapper({VALUE_TYPE, VALUE_COMPONENT, VALUE_COUNT},
                                  FGShaderFactory::literal(literal)) {
        }

        FGShaderNodeWrapperTyped(const double literal)
            : FGShaderNodeWrapper({VALUE_TYPE, VALUE_COMPONENT, VALUE_COUNT},
                                  FGShaderFactory::literal(literal)) {
        }

    private:
        static std::vector<std::unique_ptr<FGShaderNode> > getNodes(const std::vector<FGShaderNodeWrapper> &values) {
            std::vector<std::unique_ptr<FGShaderNode> > nodes;
            for (auto &value: values) {
                nodes.push_back(value.node->copy());
            }
            return nodes;
        }
    };
}

#endif //XENGINE_FGSHADERNODEWRAPPER_HPP
