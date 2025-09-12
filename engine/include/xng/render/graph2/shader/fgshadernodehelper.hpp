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

#ifndef XENGINE_FGSHADERNODEHELPER_HPP
#define XENGINE_FGSHADERNODEHELPER_HPP

#include "xng/render/graph2/shader/fgshadernodewrapper.hpp"

/**
 * WARNING: This namespace defines operators for converting literals to node wrappers which can cause ambiguous overloads,
 *          so this header should only be included in compilation units that specifically handle shader creation.
 *
 * Defines various types and functions for easy usage of the shader node wrappers.
 *
 * This enables the users to write shader code in c++ that resembles glsl pretty closely.
 *
 * Differences to GLSL:
 * - Type Definition
 *  Literal types are uppercase (e.g. "int" in glsl becomes "Int" in c++)
 *  Arrays must be defined as ArrayX<COUNT> b = ArrayX<COUNT>{VALUES...}
 *  Variables must be assigned at creation time e.g. "Int i;" is not valid
 *
 * - Subscripting
 *  Vector subscripting are functions (e.g. "vec.x" in glsl becomes "vec.x()" in c++)
 *  To assign to a subscripted value users must use setX, setElement (e.g. "vec.x = 1" in glsl becomes "vec.setX(1)" in c++
 *  Matrix subscripting has to be done with the elements() method (e.g. "mat[column][row]" in glsl becomes "mat.element(column, row)" in c++)
 *
 * - Input Data
 *  Input attributes are accessed through FGShaderNodeHelper::attribute
 *  Buffers are accessed through FGShaderNodeHelper::buffer and FGShaderNodeHelper::dynamicBuffer
 *  Parameters are accessed through FGShaderNodeHelper::parameter
 *  User defined function arguments are accessed through FGShaderNodeHelper::argument
 *
 * - Output Data
 *  Writing attributes is done through FGShaderNodeHelper::writeAttribute
 *  Writing buffers is done through FGShaderNodeHelper::writeBuffer / writeDynamicBuffer
 *
 * - Control Flow
 *  Conditionals and Branches are defined through FGShaderBuilder::If / Else / EndIf / For / EndFor
 *  Functions can be defined through FGShaderBuilder::Function / EndFunction
 *  User defined functions must be called through FGShaderNodeHelper::Call (e.g. test(1) in glsl becomes Call("test", {1}) in c++)
 *  Functions are returned from through FGShaderNodeHelper::Return (e.g. "return 5" in glsl becomes "Return(5)" in c++)
 */
namespace xng::FGShaderNodeHelper {
    // Operators for lhs literals
    template<typename T>
    FGShaderNodeWrapper operator +(const T &lhs, const FGShaderNodeWrapper &rhs) {
        return FGShaderNodeWrapper(lhs) + rhs;
    }

    template<typename T>
    FGShaderNodeWrapper operator -(const T &lhs, const FGShaderNodeWrapper &rhs) {
        return FGShaderNodeWrapper(lhs) - rhs;
    }

    template<typename T>
    FGShaderNodeWrapper operator *(const T &lhs, const FGShaderNodeWrapper &rhs) {
        return FGShaderNodeWrapper(lhs) * rhs;
    }

    template<typename T>
    FGShaderNodeWrapper operator /(const T &lhs, const FGShaderNodeWrapper &rhs) {
        return FGShaderNodeWrapper(lhs) / rhs;
    }

    template<typename T>
    FGShaderNodeWrapper operator ==(const T &lhs, const FGShaderNodeWrapper &rhs) {
        return FGShaderNodeWrapper(lhs) == rhs;
    }

    template<typename T>
    FGShaderNodeWrapper operator !=(const T &lhs, const FGShaderNodeWrapper &rhs) {
        return FGShaderNodeWrapper(lhs) != rhs;
    }

    template<typename T>
    FGShaderNodeWrapper operator <(const T &lhs, const FGShaderNodeWrapper &rhs) {
        return FGShaderNodeWrapper(lhs) < rhs;
    }

    template<typename T>
    FGShaderNodeWrapper operator >(const T &lhs, const FGShaderNodeWrapper &rhs) {
        return FGShaderNodeWrapper(lhs) > rhs;
    }

    template<typename T>
    FGShaderNodeWrapper operator <=(const T &lhs, const FGShaderNodeWrapper &rhs) {
        return FGShaderNodeWrapper(lhs) <= rhs;
    }

    template<typename T>
    FGShaderNodeWrapper operator >=(const T &lhs, const FGShaderNodeWrapper &rhs) {
        return FGShaderNodeWrapper(lhs) >= rhs;
    }

    template<typename T>
    FGShaderNodeWrapper operator ||(const T &lhs, const FGShaderNodeWrapper &rhs) {
        return FGShaderNodeWrapper(lhs) || rhs;
    }

    template<typename T>
    FGShaderNodeWrapper operator &&(const T &lhs, const FGShaderNodeWrapper &rhs) {
        return FGShaderNodeWrapper(lhs) + rhs;
    }

    template<int C>
    using ArrayBool = FGShaderNodeWrapperTyped<FGShaderValue::SCALAR, FGShaderValue::BOOLEAN, C>;

    template<int C>
    using ArrayInt = FGShaderNodeWrapperTyped<FGShaderValue::SCALAR, FGShaderValue::SIGNED_INT, C>;

    template<int C>
    using ArrayUInt = FGShaderNodeWrapperTyped<FGShaderValue::SCALAR, FGShaderValue::UNSIGNED_INT, C>;

    template<int C>
    using ArrayFloat = FGShaderNodeWrapperTyped<FGShaderValue::SCALAR, FGShaderValue::FLOAT, C>;

    template<int C>
    using ArrayDouble = FGShaderNodeWrapperTyped<FGShaderValue::SCALAR, FGShaderValue::DOUBLE, C>;

    typedef FGShaderNodeWrapperTyped<FGShaderValue::SCALAR, FGShaderValue::BOOLEAN, 1> Bool;

    typedef FGShaderNodeWrapperTyped<FGShaderValue::SCALAR, FGShaderValue::SIGNED_INT, 1> Int;

    typedef FGShaderNodeWrapperTyped<FGShaderValue::SCALAR, FGShaderValue::UNSIGNED_INT, 1> UInt;

    typedef FGShaderNodeWrapperTyped<FGShaderValue::SCALAR, FGShaderValue::FLOAT, 1> Float;

    typedef FGShaderNodeWrapperTyped<FGShaderValue::SCALAR, FGShaderValue::DOUBLE, 1> Double;

    typedef FGShaderNodeWrapperTyped<FGShaderValue::VECTOR2, FGShaderValue::FLOAT, 1> vec2;

    typedef FGShaderNodeWrapperTyped<FGShaderValue::VECTOR3, FGShaderValue::FLOAT, 1> vec3;

    typedef FGShaderNodeWrapperTyped<FGShaderValue::VECTOR4, FGShaderValue::FLOAT, 1> vec4;

    typedef FGShaderNodeWrapperTyped<FGShaderValue::MAT2, FGShaderValue::FLOAT, 1> mat2;

    typedef FGShaderNodeWrapperTyped<FGShaderValue::MAT3, FGShaderValue::FLOAT, 1> mat3;

    typedef FGShaderNodeWrapperTyped<FGShaderValue::MAT4, FGShaderValue::FLOAT, 1> mat4;

    typedef FGShaderNodeWrapperTyped<FGShaderValue::VECTOR2, FGShaderValue::DOUBLE, 1> dvec2;

    typedef FGShaderNodeWrapperTyped<FGShaderValue::VECTOR3, FGShaderValue::DOUBLE, 1> dvec3;

    typedef FGShaderNodeWrapperTyped<FGShaderValue::VECTOR4, FGShaderValue::DOUBLE, 1> dvec4;

    typedef FGShaderNodeWrapperTyped<FGShaderValue::MAT2, FGShaderValue::DOUBLE, 1> dmat2;

    typedef FGShaderNodeWrapperTyped<FGShaderValue::MAT3, FGShaderValue::DOUBLE, 1> dmat3;

    typedef FGShaderNodeWrapperTyped<FGShaderValue::MAT4, FGShaderValue::DOUBLE, 1> dmat4;

    typedef FGShaderNodeWrapperTyped<FGShaderValue::VECTOR2, FGShaderValue::BOOLEAN, 1> bvec2;

    typedef FGShaderNodeWrapperTyped<FGShaderValue::VECTOR3, FGShaderValue::BOOLEAN, 1> bvec3;

    typedef FGShaderNodeWrapperTyped<FGShaderValue::VECTOR4, FGShaderValue::BOOLEAN, 1> bvec4;

    typedef FGShaderNodeWrapperTyped<FGShaderValue::VECTOR2, FGShaderValue::SIGNED_INT, 1> ivec2;

    typedef FGShaderNodeWrapperTyped<FGShaderValue::VECTOR3, FGShaderValue::SIGNED_INT, 1> ivec3;

    typedef FGShaderNodeWrapperTyped<FGShaderValue::VECTOR4, FGShaderValue::SIGNED_INT, 1> ivec4;

    typedef FGShaderNodeWrapperTyped<FGShaderValue::VECTOR2, FGShaderValue::UNSIGNED_INT, 1> uvec2;

    typedef FGShaderNodeWrapperTyped<FGShaderValue::VECTOR3, FGShaderValue::UNSIGNED_INT, 1> uvec3;

    typedef FGShaderNodeWrapperTyped<FGShaderValue::VECTOR4, FGShaderValue::UNSIGNED_INT, 1> uvec4;

    /**
     * Read-only references to input data
     *
     * @param binding
     * @return
     */
    inline FGShaderNodeWrapper attribute(const uint32_t &binding) {
        return FGShaderNodeWrapper(FGShaderBuilder::instance().getInputLayout().elements.at(binding),
                                   FGShaderFactory::attributeInput(binding));
    }

    inline FGShaderNodeWrapper parameter(const std::string &name) {
        return FGShaderNodeWrapper(FGShaderBuilder::instance().getParameters().at(name),
                                   FGShaderFactory::parameter(name));
    }

    inline FGShaderNodeWrapper argument(const std::string &name) {
        auto func = FGShaderBuilder::instance().getCurrentFunction();
        return FGShaderNodeWrapper(func.arguments.at(name),
                                   FGShaderFactory::argument(name));
    }

    inline FGShaderNodeWrapper buffer(const std::string &name, const std::string &elementName) {
        return FGShaderNodeWrapper(FGShaderBuilder::instance().getBuffers().at(name).getElement(elementName).value,
                                   FGShaderFactory::bufferRead(name, elementName, nullptr));
    }

    inline FGShaderNodeWrapper dynamicBuffer(const std::string &name,
                                             const std::string &elementName,
                                             const FGShaderNodeWrapper &index) {
        return FGShaderNodeWrapper(FGShaderBuilder::instance().getBuffers().at(name).getElement(elementName).value,
                                   FGShaderFactory::bufferRead(name, elementName, index.node));
    }

    inline void writeAttribute(const uint32_t &binding, const FGShaderNodeWrapper &value) {
        auto attr = FGShaderFactory::attributeOutput(binding);
        FGShaderBuilder::instance().addNode(FGShaderFactory::assign(attr, value.node));
    }

    inline void writeBuffer(const std::string &name, const std::string &elementName, const FGShaderNodeWrapper &value) {
        FGShaderBuilder::instance().addNode(FGShaderFactory::bufferWrite(name, elementName, nullptr, value.node));
    }

    inline void writeDynamicBuffer(const std::string &name,
                                   const std::string &elementName,
                                   const FGShaderNodeWrapper &index,
                                   const FGShaderNodeWrapper &value) {
        FGShaderBuilder::instance().addNode(FGShaderFactory::bufferWrite(name, elementName, index.node, value.node));
    }

    inline FGShaderNodeWrapper Call(const std::string &functionName,
                                    const std::vector<FGShaderNodeWrapper> &wArgs = {}) {
        std::vector<std::unique_ptr<FGShaderNode> > args;
        for (auto &arg: wArgs) {
            args.push_back(arg.node->copy());
        }
        return FGShaderNodeWrapper(FGShaderBuilder::instance().getFunctions().at(functionName).returnType,
                                   FGShaderFactory::call(functionName, args));
    }

    inline void Return(const FGShaderNodeWrapper &value) {
        FGShaderBuilder::instance().addNode(FGShaderFactory::ret(value.node));
    }

    inline FGShaderNodeWrapper textureSize(const std::string &textureName) {
        if (FGShaderBuilder::instance().getTextures().at(textureName).arrayLayers > 1) {
            return FGShaderNodeWrapper(FGShaderValue::ivec3(),
                                       FGShaderFactory::textureSize(textureName));
        } else {
            return FGShaderNodeWrapper(FGShaderValue::ivec2(),
                                       FGShaderFactory::textureSize(textureName));
        }
    }

    inline FGShaderNodeWrapper texture(const std::string &texture,
                                       const FGShaderNodeWrapper &coords) {
        return FGShaderNodeWrapper(FGShaderValue::vec4(),
                                   FGShaderFactory::textureSample(texture,
                                                                  coords.node,
                                                                  nullptr));
    }

    inline FGShaderNodeWrapper texture(const std::string &texture,
                                       const FGShaderNodeWrapper &coords,
                                       const FGShaderNodeWrapper &bias) {
        return FGShaderNodeWrapper(FGShaderValue::vec4(),
                                   FGShaderFactory::textureSample(texture,
                                                                  coords.node,
                                                                  bias.node));
    }

    inline FGShaderNodeWrapper abs(const FGShaderNodeWrapper &value) {
        return FGShaderNodeWrapper(value.type, FGShaderFactory::abs(value.node));
    }

    inline FGShaderNodeWrapper sin(const FGShaderNodeWrapper &value) {
        return FGShaderNodeWrapper(value.type, FGShaderFactory::sin(value.node));
    }

    inline FGShaderNodeWrapper cos(const FGShaderNodeWrapper &value) {
        return FGShaderNodeWrapper(value.type, FGShaderFactory::cos(value.node));
    }

    inline FGShaderNodeWrapper tan(const FGShaderNodeWrapper &value) {
        return FGShaderNodeWrapper(value.type, FGShaderFactory::tan(value.node));
    }

    inline FGShaderNodeWrapper asin(const FGShaderNodeWrapper &value) {
        return FGShaderNodeWrapper(value.type, FGShaderFactory::asin(value.node));
    }

    inline FGShaderNodeWrapper acos(const FGShaderNodeWrapper &value) {
        return FGShaderNodeWrapper(value.type, FGShaderFactory::acos(value.node));
    }

    inline FGShaderNodeWrapper atan(const FGShaderNodeWrapper &value) {
        return FGShaderNodeWrapper(value.type, FGShaderFactory::atan(value.node));
    }

    inline FGShaderNodeWrapper pow(const FGShaderNodeWrapper &value, const FGShaderNodeWrapper &exponent) {
        return FGShaderNodeWrapper(value.type, FGShaderFactory::pow(value.node, exponent.node));
    }

    inline FGShaderNodeWrapper exp(const FGShaderNodeWrapper &value) {
        return FGShaderNodeWrapper(value.type, FGShaderFactory::exp(value.node));
    }

    inline FGShaderNodeWrapper log(const FGShaderNodeWrapper &value) {
        return FGShaderNodeWrapper(value.type, FGShaderFactory::log(value.node));
    }

    inline FGShaderNodeWrapper sqrt(const FGShaderNodeWrapper &value) {
        return FGShaderNodeWrapper(value.type, FGShaderFactory::sqrt(value.node));
    }

    inline FGShaderNodeWrapper inverseSqrt(const FGShaderNodeWrapper &value) {
        return FGShaderNodeWrapper(value.type, FGShaderFactory::inverseSqrt(value.node));
    }

    inline FGShaderNodeWrapper floor(const FGShaderNodeWrapper &value) {
        return FGShaderNodeWrapper(value.type, FGShaderFactory::floor(value.node));
    }

    inline FGShaderNodeWrapper ceil(const FGShaderNodeWrapper &value) {
        return FGShaderNodeWrapper(value.type, FGShaderFactory::ceil(value.node));
    }

    inline FGShaderNodeWrapper round(const FGShaderNodeWrapper &value) {
        return FGShaderNodeWrapper(value.type, FGShaderFactory::round(value.node));
    }

    inline FGShaderNodeWrapper fract(const FGShaderNodeWrapper &value) {
        return FGShaderNodeWrapper(value.type, FGShaderFactory::fract(value.node));
    }

    inline FGShaderNodeWrapper mod(const FGShaderNodeWrapper &value, const FGShaderNodeWrapper &modulus) {
        return FGShaderNodeWrapper(value.type, FGShaderFactory::mod(value.node, modulus.node));
    }

    inline FGShaderNodeWrapper min(const FGShaderNodeWrapper &x, const FGShaderNodeWrapper &y) {
        return FGShaderNodeWrapper(x.type, FGShaderFactory::min(x.node, y.node));
    }

    inline FGShaderNodeWrapper max(const FGShaderNodeWrapper &x, const FGShaderNodeWrapper &y) {
        return FGShaderNodeWrapper(x.type, FGShaderFactory::max(x.node, y.node));
    }

    inline FGShaderNodeWrapper clamp(const FGShaderNodeWrapper &value,
                                     const FGShaderNodeWrapper &min,
                                     const FGShaderNodeWrapper &max) {
        return FGShaderNodeWrapper(value.type, FGShaderFactory::clamp(value.node, min.node, max.node));
    }

    inline FGShaderNodeWrapper mix(const FGShaderNodeWrapper &x,
                                   const FGShaderNodeWrapper &y,
                                   const FGShaderNodeWrapper &a) {
        return FGShaderNodeWrapper(x.type, FGShaderFactory::mix(x.node, y.node, a.node));
    }

    inline FGShaderNodeWrapper step(const FGShaderNodeWrapper &edge, const FGShaderNodeWrapper &x) {
        return FGShaderNodeWrapper(x.type, FGShaderFactory::step(edge.node, x.node));
    }

    inline FGShaderNodeWrapper smoothstep(const FGShaderNodeWrapper &edge0,
                                         const FGShaderNodeWrapper &edge1,
                                         const FGShaderNodeWrapper &x) {
        return FGShaderNodeWrapper(x.type, FGShaderFactory::smoothstep(edge0.node, edge1.node, x.node));
    }

    inline FGShaderNodeWrapper dot(const FGShaderNodeWrapper &x, const FGShaderNodeWrapper &y) {
        return FGShaderNodeWrapper(x.type, FGShaderFactory::dot(x.node, y.node));
    }

    inline FGShaderNodeWrapper cross(const FGShaderNodeWrapper &x, const FGShaderNodeWrapper &y) {
        return FGShaderNodeWrapper(x.type, FGShaderFactory::cross(x.node, y.node));
    }

    inline FGShaderNodeWrapper normalize(const FGShaderNodeWrapper &x) {
        return FGShaderNodeWrapper(x.type, FGShaderFactory::normalize(x.node));
    }

    inline FGShaderNodeWrapper length(const FGShaderNodeWrapper &x) {
        return FGShaderNodeWrapper(x.type, FGShaderFactory::length(x.node));
    }

    inline FGShaderNodeWrapper distance(const FGShaderNodeWrapper &x, const FGShaderNodeWrapper &y) {
        return FGShaderNodeWrapper(x.type, FGShaderFactory::distance(x.node, y.node));
    }

    inline FGShaderNodeWrapper reflect(const FGShaderNodeWrapper &I, const FGShaderNodeWrapper &N) {
        return FGShaderNodeWrapper(N.type, FGShaderFactory::reflect(I.node, N.node));
    }

    inline FGShaderNodeWrapper refract(const FGShaderNodeWrapper &I,
                                      const FGShaderNodeWrapper &N,
                                      const FGShaderNodeWrapper &eta) {
        return FGShaderNodeWrapper(N.type, FGShaderFactory::refract(I.node, N.node, eta.node));
    }

    inline FGShaderNodeWrapper faceforward(const FGShaderNodeWrapper &N,
                                          const FGShaderNodeWrapper &I,
                                          const FGShaderNodeWrapper &Nref) {
        return FGShaderNodeWrapper(N.type, FGShaderFactory::faceforward(N.node, I.node, Nref.node));
    }
}


#endif //XENGINE_FGSHADERNODEHELPER_HPP
