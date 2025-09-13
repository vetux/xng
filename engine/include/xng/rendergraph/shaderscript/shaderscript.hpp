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

#ifndef XENGINE_SHADERSCRIPT_HPP
#define XENGINE_SHADERSCRIPT_HPP

#include "xng/rendergraph/shader/nodes/nodetexture.hpp"
#include "xng/rendergraph/shaderscript/shadernodewrapper.hpp"

/**
 * WARNING: This namespace defines implicit conversion operators for converting literals
 *          to node wrappers, which can cause ambiguous overloads when "using" the namespace,
 *          so this namespace should only be "used" in compilation units that specifically handle shader creation.
 */
namespace xng::ShaderScript {
    // Operators for lhs literals
    template<typename T>
    ShaderNodeWrapper operator +(const T &lhs, const ShaderNodeWrapper &rhs) {
        return ShaderNodeWrapper(lhs) + rhs;
    }

    template<typename T>
    ShaderNodeWrapper operator -(const T &lhs, const ShaderNodeWrapper &rhs) {
        return ShaderNodeWrapper(lhs) - rhs;
    }

    template<typename T>
    ShaderNodeWrapper operator *(const T &lhs, const ShaderNodeWrapper &rhs) {
        return ShaderNodeWrapper(lhs) * rhs;
    }

    template<typename T>
    ShaderNodeWrapper operator /(const T &lhs, const ShaderNodeWrapper &rhs) {
        return ShaderNodeWrapper(lhs) / rhs;
    }

    template<typename T>
    ShaderNodeWrapper operator ==(const T &lhs, const ShaderNodeWrapper &rhs) {
        return ShaderNodeWrapper(lhs) == rhs;
    }

    template<typename T>
    ShaderNodeWrapper operator !=(const T &lhs, const ShaderNodeWrapper &rhs) {
        return ShaderNodeWrapper(lhs) != rhs;
    }

    template<typename T>
    ShaderNodeWrapper operator <(const T &lhs, const ShaderNodeWrapper &rhs) {
        return ShaderNodeWrapper(lhs) < rhs;
    }

    template<typename T>
    ShaderNodeWrapper operator >(const T &lhs, const ShaderNodeWrapper &rhs) {
        return ShaderNodeWrapper(lhs) > rhs;
    }

    template<typename T>
    ShaderNodeWrapper operator <=(const T &lhs, const ShaderNodeWrapper &rhs) {
        return ShaderNodeWrapper(lhs) <= rhs;
    }

    template<typename T>
    ShaderNodeWrapper operator >=(const T &lhs, const ShaderNodeWrapper &rhs) {
        return ShaderNodeWrapper(lhs) >= rhs;
    }

    template<typename T>
    ShaderNodeWrapper operator ||(const T &lhs, const ShaderNodeWrapper &rhs) {
        return ShaderNodeWrapper(lhs) || rhs;
    }

    template<typename T>
    ShaderNodeWrapper operator &&(const T &lhs, const ShaderNodeWrapper &rhs) {
        return ShaderNodeWrapper(lhs) + rhs;
    }

    template<int C>
    using ArrayBool = ShaderNodeWrapperTyped<ShaderDataType::SCALAR, ShaderDataType::BOOLEAN, C>;

    template<int C>
    using ArrayInt = ShaderNodeWrapperTyped<ShaderDataType::SCALAR, ShaderDataType::SIGNED_INT, C>;

    template<int C>
    using ArrayUInt = ShaderNodeWrapperTyped<ShaderDataType::SCALAR, ShaderDataType::UNSIGNED_INT, C>;

    template<int C>
    using ArrayFloat = ShaderNodeWrapperTyped<ShaderDataType::SCALAR, ShaderDataType::FLOAT, C>;

    template<int C>
    using ArrayDouble = ShaderNodeWrapperTyped<ShaderDataType::SCALAR, ShaderDataType::DOUBLE, C>;

    typedef ShaderNodeWrapperTyped<ShaderDataType::SCALAR, ShaderDataType::BOOLEAN, 1> Bool;

    typedef ShaderNodeWrapperTyped<ShaderDataType::SCALAR, ShaderDataType::SIGNED_INT, 1> Int;

    typedef ShaderNodeWrapperTyped<ShaderDataType::SCALAR, ShaderDataType::UNSIGNED_INT, 1> UInt;

    typedef ShaderNodeWrapperTyped<ShaderDataType::SCALAR, ShaderDataType::FLOAT, 1> Float;

    typedef ShaderNodeWrapperTyped<ShaderDataType::SCALAR, ShaderDataType::DOUBLE, 1> Double;

    typedef ShaderNodeWrapperTyped<ShaderDataType::VECTOR2, ShaderDataType::FLOAT, 1> vec2;

    typedef ShaderNodeWrapperTyped<ShaderDataType::VECTOR3, ShaderDataType::FLOAT, 1> vec3;

    typedef ShaderNodeWrapperTyped<ShaderDataType::VECTOR4, ShaderDataType::FLOAT, 1> vec4;

    typedef ShaderNodeWrapperTyped<ShaderDataType::MAT2, ShaderDataType::FLOAT, 1> mat2;

    typedef ShaderNodeWrapperTyped<ShaderDataType::MAT3, ShaderDataType::FLOAT, 1> mat3;

    typedef ShaderNodeWrapperTyped<ShaderDataType::MAT4, ShaderDataType::FLOAT, 1> mat4;

    typedef ShaderNodeWrapperTyped<ShaderDataType::VECTOR2, ShaderDataType::DOUBLE, 1> dvec2;

    typedef ShaderNodeWrapperTyped<ShaderDataType::VECTOR3, ShaderDataType::DOUBLE, 1> dvec3;

    typedef ShaderNodeWrapperTyped<ShaderDataType::VECTOR4, ShaderDataType::DOUBLE, 1> dvec4;

    typedef ShaderNodeWrapperTyped<ShaderDataType::MAT2, ShaderDataType::DOUBLE, 1> dmat2;

    typedef ShaderNodeWrapperTyped<ShaderDataType::MAT3, ShaderDataType::DOUBLE, 1> dmat3;

    typedef ShaderNodeWrapperTyped<ShaderDataType::MAT4, ShaderDataType::DOUBLE, 1> dmat4;

    typedef ShaderNodeWrapperTyped<ShaderDataType::VECTOR2, ShaderDataType::BOOLEAN, 1> bvec2;

    typedef ShaderNodeWrapperTyped<ShaderDataType::VECTOR3, ShaderDataType::BOOLEAN, 1> bvec3;

    typedef ShaderNodeWrapperTyped<ShaderDataType::VECTOR4, ShaderDataType::BOOLEAN, 1> bvec4;

    typedef ShaderNodeWrapperTyped<ShaderDataType::VECTOR2, ShaderDataType::SIGNED_INT, 1> ivec2;

    typedef ShaderNodeWrapperTyped<ShaderDataType::VECTOR3, ShaderDataType::SIGNED_INT, 1> ivec3;

    typedef ShaderNodeWrapperTyped<ShaderDataType::VECTOR4, ShaderDataType::SIGNED_INT, 1> ivec4;

    typedef ShaderNodeWrapperTyped<ShaderDataType::VECTOR2, ShaderDataType::UNSIGNED_INT, 1> uvec2;

    typedef ShaderNodeWrapperTyped<ShaderDataType::VECTOR3, ShaderDataType::UNSIGNED_INT, 1> uvec3;

    typedef ShaderNodeWrapperTyped<ShaderDataType::VECTOR4, ShaderDataType::UNSIGNED_INT, 1> uvec4;

    /**
     * Read-only references to input data
     *
     * @param binding
     * @return
     */
    inline ShaderNodeWrapper attribute(const uint32_t &binding) {
        return ShaderNodeWrapper(ShaderBuilder::instance().getInputLayout().elements.at(binding),
                                 ShaderNodeFactory::attributeInput(binding));
    }

    inline ShaderNodeWrapper parameter(const std::string &name) {
        return ShaderNodeWrapper(ShaderBuilder::instance().getParameters().at(name),
                                 ShaderNodeFactory::parameter(name));
    }

    /**
     * If the argument is a texture it cannot be assigned to a variable.
     *
     * @param name
     * @return
     */
    inline ShaderNodeWrapper argument(const std::string &name) {
        auto func = ShaderBuilder::instance().getCurrentFunction();
        auto arg = func.getArgumentType(name);
        if (arg.index() != 0) {
            return ShaderNodeWrapper(ShaderDataType::unsignedInteger(),
                                     ShaderNodeFactory::argument(name),
                                     false);
        } else {
            return ShaderNodeWrapper(std::get<ShaderDataType>(arg),
                                     ShaderNodeFactory::argument(name));
        }
    }

    inline ShaderNodeWrapper buffer(const std::string &name, const std::string &elementName) {
        return ShaderNodeWrapper(ShaderBuilder::instance().getBuffers().at(name).getElement(elementName).value,
                                 ShaderNodeFactory::bufferRead(name, elementName, nullptr));
    }

    inline ShaderNodeWrapper dynamicBuffer(const std::string &name,
                                           const std::string &elementName,
                                           const ShaderNodeWrapper &index) {
        return ShaderNodeWrapper(ShaderBuilder::instance().getBuffers().at(name).getElement(elementName).value,
                                 ShaderNodeFactory::bufferRead(name, elementName, index.node));
    }

    inline void writeAttribute(const uint32_t &binding, const ShaderNodeWrapper &value) {
        auto attr = ShaderNodeFactory::attributeOutput(binding);
        ShaderBuilder::instance().addNode(ShaderNodeFactory::assign(attr, value.node));
    }

    inline void writeBuffer(const std::string &name, const std::string &elementName, const ShaderNodeWrapper &value) {
        ShaderBuilder::instance().addNode(ShaderNodeFactory::bufferWrite(name, elementName, nullptr, value.node));
    }

    inline void writeDynamicBuffer(const std::string &name,
                                   const std::string &elementName,
                                   const ShaderNodeWrapper &index,
                                   const ShaderNodeWrapper &value) {
        ShaderBuilder::instance().addNode(ShaderNodeFactory::bufferWrite(name, elementName, index.node, value.node));
    }

    inline ShaderNodeWrapper dynamicBufferLength(const std::string &name) {
        return ShaderNodeWrapper(ShaderDataType::unsignedInteger(), ShaderNodeFactory::bufferSize(name));
    }

    inline void setVertexPosition(const ShaderNodeWrapper &value) {
        ShaderBuilder::instance().addNode(ShaderNodeFactory::vertexPosition(value.node));
    }

    inline ShaderNodeWrapper Call(const std::string &functionName,
                                  const std::vector<ShaderNodeWrapper> &wArgs = {}) {
        std::vector<std::unique_ptr<ShaderNode> > args;
        for (auto &arg: wArgs) {
            args.push_back(arg.node->copy());
        }
        return ShaderNodeWrapper(ShaderBuilder::instance().getFunctions().at(functionName).returnType,
                                 ShaderNodeFactory::call(functionName, args));
    }

    inline void Return(const ShaderNodeWrapper &value) {
        ShaderBuilder::instance().addNode(ShaderNodeFactory::ret(value.node));
    }

    inline void If(const ShaderNodeWrapper &condition) {
        ShaderBuilder::instance().If(condition);
    }

    inline void Else() {
        ShaderBuilder::instance().Else();
    }

    inline void EndIf() {
        ShaderBuilder::instance().EndIf();
    }

    inline void For(const ShaderNodeWrapper &loopVariable,
                    const ShaderNodeWrapper &initializer,
                    const ShaderNodeWrapper &condition,
                    const ShaderNodeWrapper &incrementor) {
        ShaderBuilder::instance().For(loopVariable, initializer, condition, incrementor);
    }

    inline void EndFor() {
        ShaderBuilder::instance().EndFor();
    }

    //TODO: Redesign shader script to  handle textures in an easier way
    /**
     * Cannot be assigned to a variable.
     *
     * @param textureBinding
     * @return
     */
    inline ShaderNodeWrapper textureSampler(const uint32_t textureBinding) {
        auto tex = ShaderBuilder::instance().getTextures().at(textureBinding);
        return ShaderNodeWrapper(ShaderDataType{ShaderDataType::VECTOR4, ShaderDataType::getColorComponent(tex.format)},
                                 ShaderNodeFactory::texture(textureBinding),
                                 false);
    }

    inline ShaderNodeWrapper textureSize(const ShaderNodeWrapper &texture) {
        if (ShaderBuilder::instance().getTextures().at(down_cast<NodeTexture &>(*texture.node).textureBinding).
            isArray) {
            return ShaderNodeWrapper(ShaderDataType::ivec3(),
                                     ShaderNodeFactory::textureSize(texture.node));
        } else {
            return ShaderNodeWrapper(ShaderDataType::ivec2(),
                                     ShaderNodeFactory::textureSize(texture.node));
        }
    }

    inline ShaderNodeWrapper textureSize(const ShaderNodeWrapper &texture, const ShaderNodeWrapper &lod) {
        if (ShaderBuilder::instance().getTextures().at(down_cast<NodeTexture &>(*texture.node).textureBinding).
            isArray) {
            return ShaderNodeWrapper(ShaderDataType::ivec3(),
                                     ShaderNodeFactory::textureSize(texture.node, lod.node));
            } else {
                return ShaderNodeWrapper(ShaderDataType::ivec2(),
                                         ShaderNodeFactory::textureSize(texture.node, lod.node));
            }
    }

    inline ShaderNodeWrapper texture(const ShaderNodeWrapper &texture,
                                     const ShaderNodeWrapper &coords) {
        return ShaderNodeWrapper(texture.type,
                                 ShaderNodeFactory::textureSample(texture.node,
                                                                  coords.node,
                                                                  nullptr));
    }

    inline ShaderNodeWrapper texture(const ShaderNodeWrapper &texture,
                                     const ShaderNodeWrapper &coords,
                                     const ShaderNodeWrapper &bias) {
        return ShaderNodeWrapper(texture.type,
                                 ShaderNodeFactory::textureSample(texture.node,
                                                                  coords.node,
                                                                  bias.node));
    }

    inline ShaderNodeWrapper texelFetch(const ShaderNodeWrapper &texture,
                                        const ShaderNodeWrapper &coords,
                                        const ShaderNodeWrapper &index) {
        return ShaderNodeWrapper(texture.type,
                                 ShaderNodeFactory::textureFetch(texture.node, coords.node, index.node));
    }

    inline ShaderNodeWrapper abs(const ShaderNodeWrapper &value) {
        return ShaderNodeWrapper(value.type, ShaderNodeFactory::abs(value.node));
    }

    inline ShaderNodeWrapper sin(const ShaderNodeWrapper &value) {
        return ShaderNodeWrapper(value.type, ShaderNodeFactory::sin(value.node));
    }

    inline ShaderNodeWrapper cos(const ShaderNodeWrapper &value) {
        return ShaderNodeWrapper(value.type, ShaderNodeFactory::cos(value.node));
    }

    inline ShaderNodeWrapper tan(const ShaderNodeWrapper &value) {
        return ShaderNodeWrapper(value.type, ShaderNodeFactory::tan(value.node));
    }

    inline ShaderNodeWrapper asin(const ShaderNodeWrapper &value) {
        return ShaderNodeWrapper(value.type, ShaderNodeFactory::asin(value.node));
    }

    inline ShaderNodeWrapper acos(const ShaderNodeWrapper &value) {
        return ShaderNodeWrapper(value.type, ShaderNodeFactory::acos(value.node));
    }

    inline ShaderNodeWrapper atan(const ShaderNodeWrapper &value) {
        return ShaderNodeWrapper(value.type, ShaderNodeFactory::atan(value.node));
    }

    inline ShaderNodeWrapper pow(const ShaderNodeWrapper &value, const ShaderNodeWrapper &exponent) {
        return ShaderNodeWrapper(value.type, ShaderNodeFactory::pow(value.node, exponent.node));
    }

    inline ShaderNodeWrapper exp(const ShaderNodeWrapper &value) {
        return ShaderNodeWrapper(value.type, ShaderNodeFactory::exp(value.node));
    }

    inline ShaderNodeWrapper log(const ShaderNodeWrapper &value) {
        return ShaderNodeWrapper(value.type, ShaderNodeFactory::log(value.node));
    }

    inline ShaderNodeWrapper sqrt(const ShaderNodeWrapper &value) {
        return ShaderNodeWrapper(value.type, ShaderNodeFactory::sqrt(value.node));
    }

    inline ShaderNodeWrapper inverseSqrt(const ShaderNodeWrapper &value) {
        return ShaderNodeWrapper(value.type, ShaderNodeFactory::inverseSqrt(value.node));
    }

    inline ShaderNodeWrapper floor(const ShaderNodeWrapper &value) {
        return ShaderNodeWrapper(value.type, ShaderNodeFactory::floor(value.node));
    }

    inline ShaderNodeWrapper ceil(const ShaderNodeWrapper &value) {
        return ShaderNodeWrapper(value.type, ShaderNodeFactory::ceil(value.node));
    }

    inline ShaderNodeWrapper round(const ShaderNodeWrapper &value) {
        return ShaderNodeWrapper(value.type, ShaderNodeFactory::round(value.node));
    }

    inline ShaderNodeWrapper fract(const ShaderNodeWrapper &value) {
        return ShaderNodeWrapper(value.type, ShaderNodeFactory::fract(value.node));
    }

    inline ShaderNodeWrapper mod(const ShaderNodeWrapper &value, const ShaderNodeWrapper &modulus) {
        return ShaderNodeWrapper(value.type, ShaderNodeFactory::mod(value.node, modulus.node));
    }

    inline ShaderNodeWrapper min(const ShaderNodeWrapper &x, const ShaderNodeWrapper &y) {
        return ShaderNodeWrapper(x.type, ShaderNodeFactory::min(x.node, y.node));
    }

    inline ShaderNodeWrapper max(const ShaderNodeWrapper &x, const ShaderNodeWrapper &y) {
        return ShaderNodeWrapper(x.type, ShaderNodeFactory::max(x.node, y.node));
    }

    inline ShaderNodeWrapper clamp(const ShaderNodeWrapper &value,
                                   const ShaderNodeWrapper &min,
                                   const ShaderNodeWrapper &max) {
        return ShaderNodeWrapper(value.type, ShaderNodeFactory::clamp(value.node, min.node, max.node));
    }

    inline ShaderNodeWrapper mix(const ShaderNodeWrapper &x,
                                 const ShaderNodeWrapper &y,
                                 const ShaderNodeWrapper &a) {
        return ShaderNodeWrapper(x.type, ShaderNodeFactory::mix(x.node, y.node, a.node));
    }

    inline ShaderNodeWrapper step(const ShaderNodeWrapper &edge, const ShaderNodeWrapper &x) {
        return ShaderNodeWrapper(x.type, ShaderNodeFactory::step(edge.node, x.node));
    }

    inline ShaderNodeWrapper smoothstep(const ShaderNodeWrapper &edge0,
                                        const ShaderNodeWrapper &edge1,
                                        const ShaderNodeWrapper &x) {
        return ShaderNodeWrapper(x.type, ShaderNodeFactory::smoothstep(edge0.node, edge1.node, x.node));
    }

    inline ShaderNodeWrapper dot(const ShaderNodeWrapper &x, const ShaderNodeWrapper &y) {
        return ShaderNodeWrapper(x.type, ShaderNodeFactory::dot(x.node, y.node));
    }

    inline ShaderNodeWrapper cross(const ShaderNodeWrapper &x, const ShaderNodeWrapper &y) {
        return ShaderNodeWrapper(x.type, ShaderNodeFactory::cross(x.node, y.node));
    }

    inline ShaderNodeWrapper normalize(const ShaderNodeWrapper &x) {
        return ShaderNodeWrapper(x.type, ShaderNodeFactory::normalize(x.node));
    }

    inline ShaderNodeWrapper length(const ShaderNodeWrapper &x) {
        return ShaderNodeWrapper(x.type, ShaderNodeFactory::length(x.node));
    }

    inline ShaderNodeWrapper distance(const ShaderNodeWrapper &x, const ShaderNodeWrapper &y) {
        return ShaderNodeWrapper(x.type, ShaderNodeFactory::distance(x.node, y.node));
    }

    inline ShaderNodeWrapper reflect(const ShaderNodeWrapper &I, const ShaderNodeWrapper &N) {
        return ShaderNodeWrapper(N.type, ShaderNodeFactory::reflect(I.node, N.node));
    }

    inline ShaderNodeWrapper refract(const ShaderNodeWrapper &I,
                                     const ShaderNodeWrapper &N,
                                     const ShaderNodeWrapper &eta) {
        return ShaderNodeWrapper(N.type, ShaderNodeFactory::refract(I.node, N.node, eta.node));
    }

    inline ShaderNodeWrapper faceforward(const ShaderNodeWrapper &N,
                                         const ShaderNodeWrapper &I,
                                         const ShaderNodeWrapper &Nref) {
        return ShaderNodeWrapper(N.type, ShaderNodeFactory::faceforward(N.node, I.node, Nref.node));
    }
}

#endif //XENGINE_SHADERSCRIPT_HPP
