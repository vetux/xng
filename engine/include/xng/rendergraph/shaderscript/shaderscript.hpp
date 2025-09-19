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

// Optional Helper macros for defining C++ function wrappers for directly calling a function
#define DEFINE_FUNCTION0(name) inline ShaderNodeWrapper name(){ return Call(#name);}
#define DEFINE_FUNCTION1(name) inline ShaderNodeWrapper name(const ShaderNodeWrapper &arg){ return Call(#name, arg);}
#define DEFINE_FUNCTION2(name) inline ShaderNodeWrapper name(const ShaderNodeWrapper &arg, const ShaderNodeWrapper &arg1){ return Call(#name, arg, arg1);}
#define DEFINE_FUNCTION3(name) inline ShaderNodeWrapper name(const ShaderNodeWrapper &arg, const ShaderNodeWrapper &arg1, const ShaderNodeWrapper &arg2){ return Call(#name, arg, arg1, arg2);}
#define DEFINE_FUNCTION4(name) inline ShaderNodeWrapper name(const ShaderNodeWrapper &arg, const ShaderNodeWrapper &arg1, const ShaderNodeWrapper &arg2, const ShaderNodeWrapper &arg3){ return Call(#name, arg, arg1, arg2, arg3);}
#define DEFINE_FUNCTION5(name) inline ShaderNodeWrapper name(const ShaderNodeWrapper &arg, const ShaderNodeWrapper &arg1, const ShaderNodeWrapper &arg2, const ShaderNodeWrapper &arg3, const ShaderNodeWrapper &arg4){ return Call(#name, arg, arg1, arg2, arg3, arg4);}

// Optional Helper macros for accessing attributes
#define INPUT_ATTRIBUTE(attributeName) ShaderNodeWrapper attributeName = ShaderNodeWrapper(ShaderBuilder::instance().getInputLayout().getElementType(#attributeName), ShaderNodeFactory::attributeInput(#attributeName));
#define OUTPUT_ATTRIBUTE(attributeName) ShaderNodeWrapper attributeName = ShaderNodeWrapper(ShaderBuilder::instance().getOutputLayout().getElementType(#attributeName), ShaderNodeFactory::attributeOutput(#attributeName));

// Optional Helper macros for accessing buffers
#define BUFFER_ELEMENT(bufferName, elementName) ShaderNodeWrapper bufferName##_##elementName = ShaderNodeWrapper(ShaderBuilder::instance().getBuffers().at(#bufferName).getElement(#elementName).value, ShaderNodeFactory::bufferElement(#bufferName, #elementName));
#define DYNAMIC_BUFFER_ELEMENT(bufferName, elementName, index) ShaderNodeWrapper bufferName##_##elementName = ShaderNodeWrapper(ShaderBuilder::instance().getBuffers().at(#bufferName).getElement(#elementName).value, ShaderNodeFactory::bufferElement(#bufferName, #elementName, index.node));

// Optional Helper macro for accessing parameters
#define PARAMETER(parameterName) ShaderNodeWrapper parameterName = ShaderNodeWrapper(ShaderBuilder::instance().getParameters().at(#parameterName), ShaderNodeFactory::parameter(#parameterName));

// Optional Helper macro for accessing textures
#define TEXTURE_SAMPLER(textureName) ShaderNodeWrapper textureName = ShaderNodeWrapper(ShaderDataType{ ShaderDataType::VECTOR4, ShaderDataType::getColorComponent(ShaderBuilder::instance().getTextureArrays().at(#textureName).texture.format) }, ShaderNodeFactory::texture(#textureName));
#define TEXTURE_ARRAY_SAMPLER(textureName, textureIndex, samplerName)  ShaderNodeWrapper samplerName = ShaderNodeWrapper(ShaderDataType{ ShaderDataType::VECTOR4, ShaderDataType::getColorComponent(ShaderBuilder::instance().getTextureArrays().at(#textureName).texture.format) }, ShaderNodeFactory::texture(#textureName, textureIndex.node));

// Optional Helper macro for accessing arguments
#define ARGUMENT(argumentName) ShaderNodeWrapper argumentName = ShaderBuilder::instance().getCurrentFunction().getArgumentType(#argumentName).index() != 0\
                   ? ShaderNodeWrapper(ShaderDataType{\
                                           ShaderDataType::VECTOR4,\
                                           ShaderDataType::getColorComponent(\
                                               std::get<ShaderTexture>(\
                                                   ShaderBuilder::instance().getCurrentFunction().getArgumentType(#argumentName))\
                                               .format),\
                                           std::get<ShaderTexture>(\
                                               ShaderBuilder::instance().getCurrentFunction().getArgumentType(#argumentName)).\
                                           isArrayTexture\
                                               ? 2ul\
                                               : 1ul\
                                       },\
                                       ShaderNodeFactory::argument(#argumentName))\
                   : ShaderNodeWrapper(\
                       std::get<ShaderDataType>(ShaderBuilder::instance().getCurrentFunction().getArgumentType(#argumentName)),\
                       ShaderNodeFactory::argument(#argumentName));

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
    inline ShaderNodeWrapper readAttribute(const std::string &attributeName) {
        return ShaderNodeWrapper(ShaderBuilder::instance().getInputLayout().getElementType(attributeName),
                                 ShaderNodeFactory::attributeInput(attributeName));
    }

    inline ShaderNodeWrapper readParameter(const std::string &name) {
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
            auto targ = std::get<ShaderTexture>(arg);
            return ShaderNodeWrapper(ShaderDataType{
                                         ShaderDataType::VECTOR4,
                                         ShaderDataType::getColorComponent(targ.format),
                                         // Because textures cannot be assigned to variables we can store the information wheter a texture is an array texture in the type count
                                         targ.isArrayTexture ? 2ul : 1ul
                                     },
                                     ShaderNodeFactory::argument(name));
        } else {
            return ShaderNodeWrapper(std::get<ShaderDataType>(arg),
                                     ShaderNodeFactory::argument(name));
        }
    }

    inline ShaderNodeWrapper bufferElement(const std::string &name, const std::string &elementName) {
        return ShaderNodeWrapper(ShaderBuilder::instance().getBuffers().at(name).getElement(elementName).value,
                                 ShaderNodeFactory::bufferElement(name, elementName, nullptr));
    }

    inline ShaderNodeWrapper dynamicBufferElement(const std::string &name,
                                                  const std::string &elementName,
                                                  const ShaderNodeWrapper &index) {
        return ShaderNodeWrapper(ShaderBuilder::instance().getBuffers().at(name).getElement(elementName).value,
                                 ShaderNodeFactory::bufferElement(name, elementName, index.node));
    }

    inline void writeAttribute(const std::string &attributeName, const ShaderNodeWrapper &value) {
        auto attr = ShaderNodeFactory::attributeOutput(attributeName);
        ShaderBuilder::instance().addNode(ShaderNodeFactory::assign(attr, value.node));
    }

    inline ShaderNodeWrapper getDynamicBufferLength(const std::string &name) {
        return ShaderNodeWrapper(ShaderDataType::unsignedInteger(), ShaderNodeFactory::bufferSize(name));
    }

    inline void setVertexPosition(const ShaderNodeWrapper &value) {
        ShaderBuilder::instance().addNode(ShaderNodeFactory::vertexPosition(value.node));
    }

    inline ShaderNodeWrapper CallA(const std::string &functionName,
                                   const std::vector<ShaderNodeWrapper> &wArgs = {}) {
        std::vector<std::unique_ptr<ShaderNode> > args;
        for (auto &arg: wArgs) {
            args.push_back(arg.node->copy());
        }

        // Resolve function to call to retrieve the return type
        int funcIndex = -1;
        auto funcs = ShaderBuilder::instance().getFunctions();
        for (auto i = 0; i < funcs.size(); i++) {
            auto &func = funcs.at(i);
            if (func.name == functionName) {
                bool match = true;
                for (auto fi = 0; fi < func.arguments.size(); fi++) {
                    auto &farg = func.arguments.at(fi);
                    if (farg.type.index() == 0) {
                        if (std::get<ShaderDataType>(farg.type) != wArgs.at(fi).type) {
                            match = false;
                            break;
                        }
                    } else {
                        if (wArgs.at(fi).node->getType() != ShaderNode::TEXTURE) {
                            if (wArgs.at(fi).node->getType() == ShaderNode::ARGUMENT) {
                                auto &argNode = down_cast<NodeArgument &>(*wArgs.at(fi).node);
                                auto targ = ShaderBuilder::instance().getCurrentFunction().getArgumentType(
                                    argNode.argumentName);
                                if (targ.index() != 1) {
                                    match = false;
                                    break;
                                }
                            } else {
                                match = false;
                                break;
                            }
                        }
                    }
                }
                if (match) {
                    funcIndex = i;
                    break;
                }
            }
        }
        if (funcIndex < 0) {
            throw std::runtime_error("Function " + functionName + " not found");
        }
        return ShaderNodeWrapper(ShaderBuilder::instance().getFunctions().at(funcIndex).returnType,
                                 ShaderNodeFactory::call(functionName, args));
    }

    inline ShaderNodeWrapper Call(const std::string &functionName,
                                  const ShaderNodeWrapper &arg0) {
        return CallA(functionName, {arg0});
    }

    inline ShaderNodeWrapper Call(const std::string &functionName,
                                  const ShaderNodeWrapper &arg0,
                                  const ShaderNodeWrapper &arg1) {
        return CallA(functionName, {arg0, arg1});
    }

    inline ShaderNodeWrapper Call(const std::string &functionName,
                                  const ShaderNodeWrapper &arg0,
                                  const ShaderNodeWrapper &arg1,
                                  const ShaderNodeWrapper &arg2) {
        return CallA(functionName, {arg0, arg1, arg2});
    }

    inline ShaderNodeWrapper Call(const std::string &functionName,
                                  const ShaderNodeWrapper &arg0,
                                  const ShaderNodeWrapper &arg1,
                                  const ShaderNodeWrapper &arg2,
                                  const ShaderNodeWrapper &arg3) {
        return CallA(functionName, {arg0, arg1, arg2, arg3});
    }

    inline void Function(const std::string &name,
                         const std::vector<ShaderFunction::Argument> &arguments,
                         ShaderDataType returnType) {
        ShaderBuilder::instance().Function(name, arguments, returnType);
    }

    inline void EndFunction() {
        ShaderBuilder::instance().EndFunction();
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
                    const ShaderNodeWrapper &loopStart,
                    const ShaderNodeWrapper &loopEnd,
                    const ShaderNodeWrapper &incrementor) {
        ShaderBuilder::instance().For(loopVariable, loopStart, loopEnd, incrementor);
    }

    inline void EndFor() {
        ShaderBuilder::instance().EndFor();
    }

    //TODO: Allow texture samplers to be assigned to variables
    /**
     * Cannot be assigned to a variable.
     *
     * @param textureArrayIndex
     * @return
     */
    inline ShaderNodeWrapper textureSampler(const std::string &textureName) {
        auto texArray = ShaderBuilder::instance().getTextureArrays().at(textureName);
        return ShaderNodeWrapper(ShaderDataType{
                                     ShaderDataType::VECTOR4, ShaderDataType::getColorComponent(texArray.texture.format)
                                 },
                                 ShaderNodeFactory::texture(textureName));
    }

    /**
     * Cannot be assigned to a variable.
     *
     * @param textureArrayIndex
     * @return
     */
    inline ShaderNodeWrapper textureSampler(const std::string &textureName,
                                            const ShaderNodeWrapper &textureIndex) {
        auto texArray = ShaderBuilder::instance().getTextureArrays().at(textureName);
        return ShaderNodeWrapper(ShaderDataType{
                                     ShaderDataType::VECTOR4, ShaderDataType::getColorComponent(texArray.texture.format)
                                 },
                                 ShaderNodeFactory::texture(textureName, textureIndex.node));
    }

    inline ShaderNodeWrapper textureSize(const ShaderNodeWrapper &texture) {
        // TODO: Avoid hacky texture array detection for argument nodes, ties into making textures assignable to variables
        if (texture.type.count > 1) {
            return ShaderNodeWrapper(ShaderDataType::ivec3(),
                                     ShaderNodeFactory::textureSize(texture.node));
        } else {
            return ShaderNodeWrapper(ShaderDataType::ivec2(),
                                     ShaderNodeFactory::textureSize(texture.node));
        }
    }

    inline ShaderNodeWrapper textureSize(const ShaderNodeWrapper &texture, const ShaderNodeWrapper &lod) {
        if (texture.type.count > 1) {
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
        return ShaderNodeWrapper(ShaderDataType::float32(), ShaderNodeFactory::dot(x.node, y.node));
    }

    inline ShaderNodeWrapper cross(const ShaderNodeWrapper &x, const ShaderNodeWrapper &y) {
        return ShaderNodeWrapper(ShaderDataType::vec3(), ShaderNodeFactory::cross(x.node, y.node));
    }

    inline ShaderNodeWrapper normalize(const ShaderNodeWrapper &x) {
        return ShaderNodeWrapper(x.type, ShaderNodeFactory::normalize(x.node));
    }

    inline ShaderNodeWrapper length(const ShaderNodeWrapper &x) {
        return ShaderNodeWrapper(ShaderDataType::float32(), ShaderNodeFactory::length(x.node));
    }

    inline ShaderNodeWrapper distance(const ShaderNodeWrapper &x, const ShaderNodeWrapper &y) {
        return ShaderNodeWrapper(ShaderDataType::float32(), ShaderNodeFactory::distance(x.node, y.node));
    }

    inline ShaderNodeWrapper reflect(const ShaderNodeWrapper &I, const ShaderNodeWrapper &N) {
        return ShaderNodeWrapper(I.type, ShaderNodeFactory::reflect(I.node, N.node));
    }

    inline ShaderNodeWrapper refract(const ShaderNodeWrapper &I,
                                     const ShaderNodeWrapper &N,
                                     const ShaderNodeWrapper &eta) {
        return ShaderNodeWrapper(I.type, ShaderNodeFactory::refract(I.node, N.node, eta.node));
    }

    inline ShaderNodeWrapper faceforward(const ShaderNodeWrapper &N,
                                         const ShaderNodeWrapper &I,
                                         const ShaderNodeWrapper &Nref) {
        return ShaderNodeWrapper(N.type, ShaderNodeFactory::faceforward(N.node, I.node, Nref.node));
    }

    inline ShaderNodeWrapper transpose(const ShaderNodeWrapper &v) {
        return ShaderNodeWrapper(v.type, ShaderNodeFactory::transpose(v.node));
    }

    inline ShaderNodeWrapper inverse(const ShaderNodeWrapper &v) {
        return ShaderNodeWrapper(v.type, ShaderNodeFactory::inverse(v.node));
    }

    inline ShaderNodeWrapper matrix2(const ShaderNodeWrapper &x) {
        return ShaderNodeWrapper({ShaderDataType::MAT2, x.type.component},
                                 ShaderNodeFactory::matrix(ShaderDataType(ShaderDataType::MAT2, x.type.component),
                                                           x.node,
                                                           nullptr,
                                                           nullptr,
                                                           nullptr));
    }

    inline ShaderNodeWrapper matrix3(const ShaderNodeWrapper &x) {
        return ShaderNodeWrapper({ShaderDataType::MAT3, x.type.component},
                                 ShaderNodeFactory::matrix(ShaderDataType(ShaderDataType::MAT3, x.type.component),
                                                           x.node,
                                                           nullptr,
                                                           nullptr,
                                                           nullptr));
    }

    inline ShaderNodeWrapper matrix4(const ShaderNodeWrapper &x) {
        return ShaderNodeWrapper({ShaderDataType::MAT4, x.type.component},
                                 ShaderNodeFactory::matrix(ShaderDataType(ShaderDataType::MAT4, x.type.component),
                                                           x.node,
                                                           nullptr,
                                                           nullptr,
                                                           nullptr));
    }

    inline ShaderNodeWrapper matrix(const ShaderNodeWrapper &x, const ShaderNodeWrapper &y) {
        return ShaderNodeWrapper({ShaderDataType::MAT2, x.type.component},
                                 ShaderNodeFactory::matrix(ShaderDataType(ShaderDataType::MAT2, x.type.component),
                                                           x.node,
                                                           y.node,
                                                           nullptr,
                                                           nullptr));
    }

    inline ShaderNodeWrapper matrix(const ShaderNodeWrapper &x,
                                    const ShaderNodeWrapper &y,
                                    const ShaderNodeWrapper &z) {
        return ShaderNodeWrapper({ShaderDataType::MAT3, x.type.component},
                                 ShaderNodeFactory::matrix(ShaderDataType(ShaderDataType::MAT3, x.type.component),
                                                           x.node,
                                                           y.node,
                                                           z.node,
                                                           nullptr));
    }

    inline ShaderNodeWrapper matrix(const ShaderNodeWrapper &x,
                                    const ShaderNodeWrapper &y,
                                    const ShaderNodeWrapper &z,
                                    const ShaderNodeWrapper &w) {
        return ShaderNodeWrapper({ShaderDataType::MAT4, x.type.component},
                                 ShaderNodeFactory::matrix(ShaderDataType(ShaderDataType::MAT4, x.type.component),
                                                           x.node,
                                                           y.node,
                                                           z.node,
                                                           w.node));
    }
}

#endif //XENGINE_SHADERSCRIPT_HPP
