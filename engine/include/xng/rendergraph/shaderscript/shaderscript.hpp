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

#ifndef XENGINE_SHADERSCRIPT_HPP
#define XENGINE_SHADERSCRIPT_HPP

#include "xng/rendergraph/shader/nodes/nodetexture.hpp"
#include "xng/rendergraph/shaderscript/shaderobject.hpp"

// Optional Helper macros for defining C++ function wrappers for directly calling a function
#define DEFINE_FUNCTION0(name) inline ShaderObject name(){ return Call(#name);}
#define DEFINE_FUNCTION1(name) inline ShaderObject name(const ShaderObject &arg){ return Call(#name, arg);}
#define DEFINE_FUNCTION2(name) inline ShaderObject name(const ShaderObject &arg, const ShaderObject &arg1){ return Call(#name, arg, arg1);}
#define DEFINE_FUNCTION3(name) inline ShaderObject name(const ShaderObject &arg, const ShaderObject &arg1, const ShaderObject &arg2){ return Call(#name, arg, arg1, arg2);}
#define DEFINE_FUNCTION4(name) inline ShaderObject name(const ShaderObject &arg, const ShaderObject &arg1, const ShaderObject &arg2, const ShaderObject &arg3){ return Call(#name, arg, arg1, arg2, arg3);}
#define DEFINE_FUNCTION5(name) inline ShaderObject name(const ShaderObject &arg, const ShaderObject &arg1, const ShaderObject &arg2, const ShaderObject &arg3, const ShaderObject &arg4){ return Call(#name, arg, arg1, arg2, arg3, arg4);}

// Optional Helper macro for accessing arguments
#define ARGUMENT(argumentName) ShaderObject argumentName = std::holds_alternative<ShaderTexture>(ShaderBuilder::instance().getCurrentFunction().getArgumentType(#argumentName))\
                    ? ShaderObject(std::get<ShaderTexture>(ShaderBuilder::instance().getCurrentFunction().getArgumentType(#argumentName)), ShaderNodeFactory::argument(#argumentName))\
                    : std::holds_alternative<ShaderStructName>(ShaderBuilder::instance().getCurrentFunction().getArgumentType(#argumentName))\
                        ? ShaderObject(std::get<ShaderStructName>(ShaderBuilder::instance().getCurrentFunction().getArgumentType(#argumentName)),ShaderNodeFactory::argument(#argumentName))\
                        : ShaderObject(std::get<ShaderDataType>(ShaderBuilder::instance().getCurrentFunction().getArgumentType(#argumentName)),ShaderNodeFactory::argument(#argumentName));

/**
 * Optional Helper Macros for abstracting the shader builder interactions
 *
 * BeginShader(VERTEX);
 *
 * Input(ShaderDataType::vec3(), vPosition);
 * Input(ShaderDataType::vec2(), vUv);
 *
 * Output(ShaderDataType::vec4(), fPosition);
 *
 * Struct(BufferSubData, {
 *      {ShaderDataType::float(), "value"}
 * });
 *
 * Struct(BufferData, {
 *      {ShaderDataType::vec2(), "color"},
 *      {BufferSubData, "subData"}
 * });
 *
 * Buffer(data, BufferData);
 * DynamicBuffer(dynData, BufferData);
 *
 * BufferRW(data, BufferData);
 * DynamicBufferRW(dynData, BufferData);
 *
 * Texture(colorTexture, TEXTURE_2D, RGBA);
 * TextureArray(textureArray, TEXTURE_2D, RGBA, 5);
 *
 * ...
 *
 * Shader vs = BuildShader();
 */
#define BeginShader(stage) auto &builder = ShaderBuilder::instance(); builder.setup(stage);

#define Input(type, name) builder.addInput(#name, type); ShaderObject name = ShaderObject(type, ShaderNodeFactory::attributeInput(#name));
#define Output(type, name) builder.addOutput(#name, type); ShaderObject name = ShaderObject(type, ShaderNodeFactory::attributeOutput(#name));

#define Struct(name, ...) builder.addTypeDefinition(ShaderStruct(#name, {__VA_ARGS__})); const ShaderStructName name = #name;

#define Buffer(name, type) builder.addBuffer(#name, ShaderBuffer(false, false, #type)); ShaderObject name = ShaderObject(#type, ShaderNodeFactory::buffer(#name));
#define DynamicBuffer(name, type) builder.addBuffer(#name, ShaderBuffer(false, true, #type)); ShaderObject name = ShaderObject(#type, ShaderNodeFactory::buffer(#name));

#define BufferRW(name, type) builder.addBuffer(#name, ShaderBuffer(true, false, #type)); ShaderObject name = ShaderObject(#type, ShaderNodeFactory::buffer(#name));
#define DynamicBufferRW(name, type) builder.addBuffer(#name, ShaderBuffer(true, true, #type)); ShaderObject name = ShaderObject(#type, ShaderNodeFactory::buffer(#name));

#define Texture(name, type, format) builder.addTextureArray(#name, ShaderTextureArray(ShaderTexture(type, format))); ShaderObject name = ShaderObject(ShaderTexture(type, format), ShaderNodeFactory::texture(#name));
#define TextureArray(name, type, format, count) builder.addTextureArray(#name, ShaderTextureArray(ShaderTexture(type, format), count)); ShaderObject name = ShaderObject(ShaderTextureArray(ShaderTexture(type, format), count), ShaderNodeFactory::texture(#name));

#define InputPrimitive(type) builder.setGeometryInput(type);
#define OutputPrimitive(type, maxVertices) builder.setGeometryOutput(type, maxVertices);

#define BuildShader() builder.build();

/**
 * WARNING: This namespace defines implicit conversion operators for converting literals
 *          to node wrappers, which can cause ambiguous overloads when "using" the namespace,
 *          so this namespace should only be "used" in compilation units that specifically handle shader creation.
 */
namespace xng::ShaderScript {
    // Operators for lhs literals
    template<typename T>
    ShaderObject operator +(const T &lhs, const ShaderObject &rhs) {
        return ShaderObject(lhs) + rhs;
    }

    template<typename T>
    ShaderObject operator -(const T &lhs, const ShaderObject &rhs) {
        return ShaderObject(lhs) - rhs;
    }

    template<typename T>
    ShaderObject operator *(const T &lhs, const ShaderObject &rhs) {
        return ShaderObject(lhs) * rhs;
    }

    template<typename T>
    ShaderObject operator /(const T &lhs, const ShaderObject &rhs) {
        return ShaderObject(lhs) / rhs;
    }

    template<typename T>
    ShaderObject operator ==(const T &lhs, const ShaderObject &rhs) {
        return ShaderObject(lhs) == rhs;
    }

    template<typename T>
    ShaderObject operator !=(const T &lhs, const ShaderObject &rhs) {
        return ShaderObject(lhs) != rhs;
    }

    template<typename T>
    ShaderObject operator <(const T &lhs, const ShaderObject &rhs) {
        return ShaderObject(lhs) < rhs;
    }

    template<typename T>
    ShaderObject operator >(const T &lhs, const ShaderObject &rhs) {
        return ShaderObject(lhs) > rhs;
    }

    template<typename T>
    ShaderObject operator <=(const T &lhs, const ShaderObject &rhs) {
        return ShaderObject(lhs) <= rhs;
    }

    template<typename T>
    ShaderObject operator >=(const T &lhs, const ShaderObject &rhs) {
        return ShaderObject(lhs) >= rhs;
    }

    template<typename T>
    ShaderObject operator ||(const T &lhs, const ShaderObject &rhs) {
        return ShaderObject(lhs) || rhs;
    }

    template<typename T>
    ShaderObject operator &&(const T &lhs, const ShaderObject &rhs) {
        return ShaderObject(lhs) + rhs;
    }

    template<int C>
    using ArrayBool = ShaderObjectTyped<ShaderDataType::SCALAR, ShaderDataType::BOOLEAN, C>;

    template<int C>
    using ArrayInt = ShaderObjectTyped<ShaderDataType::SCALAR, ShaderDataType::SIGNED_INT, C>;

    template<int C>
    using ArrayUInt = ShaderObjectTyped<ShaderDataType::SCALAR, ShaderDataType::UNSIGNED_INT, C>;

    template<int C>
    using ArrayFloat = ShaderObjectTyped<ShaderDataType::SCALAR, ShaderDataType::FLOAT, C>;

    template<int C>
    using ArrayDouble = ShaderObjectTyped<ShaderDataType::SCALAR, ShaderDataType::DOUBLE, C>;

    typedef ShaderObjectTyped<ShaderDataType::SCALAR, ShaderDataType::BOOLEAN, 1> Bool;

    typedef ShaderObjectTyped<ShaderDataType::SCALAR, ShaderDataType::SIGNED_INT, 1> Int;

    typedef ShaderObjectTyped<ShaderDataType::SCALAR, ShaderDataType::UNSIGNED_INT, 1> UInt;

    typedef ShaderObjectTyped<ShaderDataType::SCALAR, ShaderDataType::FLOAT, 1> Float;

    typedef ShaderObjectTyped<ShaderDataType::SCALAR, ShaderDataType::DOUBLE, 1> Double;

    typedef ShaderObjectTyped<ShaderDataType::VECTOR2, ShaderDataType::FLOAT, 1> vec2;

    typedef ShaderObjectTyped<ShaderDataType::VECTOR3, ShaderDataType::FLOAT, 1> vec3;

    typedef ShaderObjectTyped<ShaderDataType::VECTOR4, ShaderDataType::FLOAT, 1> vec4;

    typedef ShaderObjectTyped<ShaderDataType::MAT2, ShaderDataType::FLOAT, 1> mat2;

    typedef ShaderObjectTyped<ShaderDataType::MAT3, ShaderDataType::FLOAT, 1> mat3;

    typedef ShaderObjectTyped<ShaderDataType::MAT4, ShaderDataType::FLOAT, 1> mat4;

    typedef ShaderObjectTyped<ShaderDataType::VECTOR2, ShaderDataType::DOUBLE, 1> dvec2;

    typedef ShaderObjectTyped<ShaderDataType::VECTOR3, ShaderDataType::DOUBLE, 1> dvec3;

    typedef ShaderObjectTyped<ShaderDataType::VECTOR4, ShaderDataType::DOUBLE, 1> dvec4;

    typedef ShaderObjectTyped<ShaderDataType::MAT2, ShaderDataType::DOUBLE, 1> dmat2;

    typedef ShaderObjectTyped<ShaderDataType::MAT3, ShaderDataType::DOUBLE, 1> dmat3;

    typedef ShaderObjectTyped<ShaderDataType::MAT4, ShaderDataType::DOUBLE, 1> dmat4;

    typedef ShaderObjectTyped<ShaderDataType::VECTOR2, ShaderDataType::BOOLEAN, 1> bvec2;

    typedef ShaderObjectTyped<ShaderDataType::VECTOR3, ShaderDataType::BOOLEAN, 1> bvec3;

    typedef ShaderObjectTyped<ShaderDataType::VECTOR4, ShaderDataType::BOOLEAN, 1> bvec4;

    typedef ShaderObjectTyped<ShaderDataType::VECTOR2, ShaderDataType::SIGNED_INT, 1> ivec2;

    typedef ShaderObjectTyped<ShaderDataType::VECTOR3, ShaderDataType::SIGNED_INT, 1> ivec3;

    typedef ShaderObjectTyped<ShaderDataType::VECTOR4, ShaderDataType::SIGNED_INT, 1> ivec4;

    typedef ShaderObjectTyped<ShaderDataType::VECTOR2, ShaderDataType::UNSIGNED_INT, 1> uvec2;

    typedef ShaderObjectTyped<ShaderDataType::VECTOR3, ShaderDataType::UNSIGNED_INT, 1> uvec3;

    typedef ShaderObjectTyped<ShaderDataType::VECTOR4, ShaderDataType::UNSIGNED_INT, 1> uvec4;

    /**
     * Read-only references to input data
     *
     * @param binding
     * @return
     */
    inline ShaderObject readAttribute(const std::string &attributeName) {
        return ShaderObject(ShaderBuilder::instance().getInputLayout().getElementType(attributeName),
                            ShaderNodeFactory::attributeInput(attributeName));
    }

    inline ShaderObject readParameter(const std::string &name) {
        return ShaderObject(ShaderBuilder::instance().getParameters().at(name),
                            ShaderNodeFactory::parameter(name));
    }

    /**
     * If the argument is a texture it cannot be assigned to a variable.
     *
     * @param name
     * @return
     */
    inline ShaderObject argument(const std::string &name) {
        auto func = ShaderBuilder::instance().getCurrentFunction();
        auto arg = func.getArgumentType(name);
        if (std::holds_alternative<ShaderTexture>(arg)) {
            return ShaderObject(std::get<ShaderTexture>(arg), ShaderNodeFactory::argument(name));
        }
        if (std::holds_alternative<ShaderStructName>(arg)) {
            return ShaderObject(std::get<ShaderStructName>(arg), ShaderNodeFactory::argument(name));
        }
        return ShaderObject(std::get<ShaderDataType>(arg), ShaderNodeFactory::argument(name));
    }

    inline ShaderObject buffer(const std::string &name) {
        return ShaderObject(ShaderBuilder::instance().getBuffers().at(name).typeName,
                            ShaderNodeFactory::buffer(name, nullptr));
    }

    inline ShaderObject dynamicBuffer(const std::string &name, const ShaderObject &index) {
        return ShaderObject(ShaderBuilder::instance().getBuffers().at(name).typeName,
                            ShaderNodeFactory::buffer(name, index.node));
    }

    inline void writeAttribute(const std::string &attributeName, const ShaderObject &value) {
        auto attr = ShaderNodeFactory::attributeOutput(attributeName);
        ShaderBuilder::instance().addNode(ShaderNodeFactory::assign(attr, value.node));
    }

    inline ShaderObject getDynamicBufferLength(const std::string &name) {
        return ShaderObject(ShaderDataType::unsignedInteger(), ShaderNodeFactory::bufferSize(name));
    }

    inline void setVertexPosition(const ShaderObject &value) {
        ShaderBuilder::instance().addNode(ShaderNodeFactory::vertexPosition(value.node));
    }

    inline void setFragmentDepth(const ShaderObject &value) {
        ShaderBuilder::instance().addNode(ShaderNodeFactory::fragmentDepth(value.node));
    }

    inline void setLayer(const ShaderObject &value) {
        ShaderBuilder::instance().addNode(ShaderNodeFactory::assign(ShaderNodeFactory::layer(), value.node));
    }

    inline ShaderObject CallA(const std::string &functionName,
                              const std::vector<ShaderObject> &wArgs = {}) {
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
                    if (std::holds_alternative<ShaderDataType>(farg.type)) {
                        if (std::get<ShaderDataType>(farg.type) != wArgs.at(fi).getDataType()) {
                            match = false;
                            break;
                        }
                    } else if (std::holds_alternative<ShaderTexture>(farg.type)) {
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
                    } else {
                        if (std::get<ShaderStructName>(farg.type) != wArgs.at(fi).getStructName()) {
                            match = false;
                            break;
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

        auto &func = ShaderBuilder::instance().getFunctions().at(funcIndex);
        if (std::holds_alternative<ShaderDataType>(func.returnType)) {
            return ShaderObject(std::get<ShaderDataType>(func.returnType),
                                ShaderNodeFactory::call(functionName, args));
        }
        return ShaderObject(std::get<ShaderStructName>(func.returnType),
                            ShaderNodeFactory::call(functionName, args));
    }

    inline ShaderObject Call(const std::string &functionName,
                             const ShaderObject &arg0) {
        return CallA(functionName, {arg0});
    }

    inline ShaderObject Call(const std::string &functionName,
                             const ShaderObject &arg0,
                             const ShaderObject &arg1) {
        return CallA(functionName, {arg0, arg1});
    }

    inline ShaderObject Call(const std::string &functionName,
                             const ShaderObject &arg0,
                             const ShaderObject &arg1,
                             const ShaderObject &arg2) {
        return CallA(functionName, {arg0, arg1, arg2});
    }

    inline ShaderObject Call(const std::string &functionName,
                             const ShaderObject &arg0,
                             const ShaderObject &arg1,
                             const ShaderObject &arg2,
                             const ShaderObject &arg3) {
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

    inline void Return(const ShaderObject &value) {
        ShaderBuilder::instance().addNode(ShaderNodeFactory::ret(value.node));
    }

    inline void If(const ShaderObject &condition) {
        ShaderBuilder::instance().If(condition);
    }

    inline void Else() {
        ShaderBuilder::instance().Else();
    }

    inline void EndIf() {
        ShaderBuilder::instance().EndIf();
    }

    inline void For(const ShaderObject &loopVariable,
                    const ShaderObject &loopStart,
                    const ShaderObject &loopEnd,
                    const ShaderObject &incrementor) {
        ShaderBuilder::instance().For(loopVariable, loopStart, loopEnd, incrementor);
    }

    inline void EndFor() {
        ShaderBuilder::instance().EndFor();
    }

    /**
     * Cannot be assigned to a variable.
     *
     * @param textureName
     * @return
     */
    inline ShaderObject textureSampler(const std::string &textureName) {
        auto texArray = ShaderBuilder::instance().getTextureArrays().at(textureName);
        return ShaderObject(texArray, ShaderNodeFactory::texture(textureName));
    }

    /**
     * Cannot be assigned to a variable.
     *
     * @param textureName
     * @param textureIndex
     * @return
     */
    inline ShaderObject textureSampler(const std::string &textureName,
                                       const ShaderObject &textureIndex) {
        auto texArray = ShaderBuilder::instance().getTextureArrays().at(textureName);
        return ShaderObject(texArray, ShaderNodeFactory::texture(textureName, textureIndex.node));
    }

    inline ShaderObject textureSize(const ShaderObject &texture) {
        if (std::holds_alternative<ShaderTextureArray>(texture.type)
            && texture.getTextureArray().arraySize > 1) {
            return ShaderObject(ShaderDataType::ivec3(), ShaderNodeFactory::textureSize(texture.node));
        }
        return ShaderObject(ShaderDataType::ivec2(), ShaderNodeFactory::textureSize(texture.node));
    }

    inline ShaderObject textureSize(const ShaderObject &texture, const ShaderObject &lod) {
        if (std::holds_alternative<ShaderTextureArray>(texture.type)
            && texture.getTextureArray().arraySize > 1) {
            return ShaderObject(ShaderDataType::ivec3(), ShaderNodeFactory::textureSize(texture.node, lod.node));
        }
        return ShaderObject(ShaderDataType::ivec2(), ShaderNodeFactory::textureSize(texture.node, lod.node));
    }

    inline ShaderObject texture(const ShaderObject &texture,
                                const ShaderObject &coords) {
        return ShaderObject(ShaderDataType{
                                ShaderDataType::VECTOR4,
                                ShaderDataType::getColorComponent(texture.getTexture().format)
                            },
                            ShaderNodeFactory::textureSample(texture.node,
                                                             coords.node,
                                                             nullptr));
    }

    inline ShaderObject texture(const ShaderObject &texture,
                                const ShaderObject &coords,
                                const ShaderObject &bias) {
        return ShaderObject(ShaderDataType{
                                ShaderDataType::VECTOR4,
                                ShaderDataType::getColorComponent(texture.getTexture().format)
                            },
                            ShaderNodeFactory::textureSample(texture.node,
                                                             coords.node,
                                                             bias.node));
    }

    inline ShaderObject texelFetch(const ShaderObject &texture,
                                   const ShaderObject &coords,
                                   const ShaderObject &index) {
        return ShaderObject(ShaderDataType{
                                ShaderDataType::VECTOR4,
                                ShaderDataType::getColorComponent(texture.getTexture().format)
                            },
                            ShaderNodeFactory::textureFetch(texture.node, coords.node, index.node));
    }

    inline ShaderObject abs(const ShaderObject &value) {
        return ShaderObject(value.type, ShaderNodeFactory::abs(value.node));
    }

    inline ShaderObject sin(const ShaderObject &value) {
        return ShaderObject(value.type, ShaderNodeFactory::sin(value.node));
    }

    inline ShaderObject cos(const ShaderObject &value) {
        return ShaderObject(value.type, ShaderNodeFactory::cos(value.node));
    }

    inline ShaderObject tan(const ShaderObject &value) {
        return ShaderObject(value.type, ShaderNodeFactory::tan(value.node));
    }

    inline ShaderObject asin(const ShaderObject &value) {
        return ShaderObject(value.type, ShaderNodeFactory::asin(value.node));
    }

    inline ShaderObject acos(const ShaderObject &value) {
        return ShaderObject(value.type, ShaderNodeFactory::acos(value.node));
    }

    inline ShaderObject atan(const ShaderObject &value) {
        return ShaderObject(value.type, ShaderNodeFactory::atan(value.node));
    }

    inline ShaderObject pow(const ShaderObject &value, const ShaderObject &exponent) {
        return ShaderObject(value.type, ShaderNodeFactory::pow(value.node, exponent.node));
    }

    inline ShaderObject exp(const ShaderObject &value) {
        return ShaderObject(value.type, ShaderNodeFactory::exp(value.node));
    }

    inline ShaderObject log(const ShaderObject &value) {
        return ShaderObject(value.type, ShaderNodeFactory::log(value.node));
    }

    inline ShaderObject sqrt(const ShaderObject &value) {
        return ShaderObject(value.type, ShaderNodeFactory::sqrt(value.node));
    }

    inline ShaderObject inverseSqrt(const ShaderObject &value) {
        return ShaderObject(value.type, ShaderNodeFactory::inverseSqrt(value.node));
    }

    inline ShaderObject floor(const ShaderObject &value) {
        return ShaderObject(value.type, ShaderNodeFactory::floor(value.node));
    }

    inline ShaderObject ceil(const ShaderObject &value) {
        return ShaderObject(value.type, ShaderNodeFactory::ceil(value.node));
    }

    inline ShaderObject round(const ShaderObject &value) {
        return ShaderObject(value.type, ShaderNodeFactory::round(value.node));
    }

    inline ShaderObject fract(const ShaderObject &value) {
        return ShaderObject(value.type, ShaderNodeFactory::fract(value.node));
    }

    inline ShaderObject mod(const ShaderObject &value, const ShaderObject &modulus) {
        return ShaderObject(value.type, ShaderNodeFactory::mod(value.node, modulus.node));
    }

    inline ShaderObject min(const ShaderObject &x, const ShaderObject &y) {
        return ShaderObject(x.type, ShaderNodeFactory::min(x.node, y.node));
    }

    inline ShaderObject max(const ShaderObject &x, const ShaderObject &y) {
        return ShaderObject(x.type, ShaderNodeFactory::max(x.node, y.node));
    }

    inline ShaderObject clamp(const ShaderObject &value,
                              const ShaderObject &min,
                              const ShaderObject &max) {
        return ShaderObject(value.type, ShaderNodeFactory::clamp(value.node, min.node, max.node));
    }

    inline ShaderObject mix(const ShaderObject &x,
                            const ShaderObject &y,
                            const ShaderObject &a) {
        return ShaderObject(x.type, ShaderNodeFactory::mix(x.node, y.node, a.node));
    }

    inline ShaderObject step(const ShaderObject &edge, const ShaderObject &x) {
        return ShaderObject(x.type, ShaderNodeFactory::step(edge.node, x.node));
    }

    inline ShaderObject smoothstep(const ShaderObject &edge0,
                                   const ShaderObject &edge1,
                                   const ShaderObject &x) {
        return ShaderObject(x.type, ShaderNodeFactory::smoothstep(edge0.node, edge1.node, x.node));
    }

    inline ShaderObject dot(const ShaderObject &x, const ShaderObject &y) {
        return ShaderObject(ShaderDataType::float32(), ShaderNodeFactory::dot(x.node, y.node));
    }

    inline ShaderObject cross(const ShaderObject &x, const ShaderObject &y) {
        return ShaderObject(ShaderDataType::vec3(), ShaderNodeFactory::cross(x.node, y.node));
    }

    inline ShaderObject normalize(const ShaderObject &x) {
        return ShaderObject(x.type, ShaderNodeFactory::normalize(x.node));
    }

    inline ShaderObject length(const ShaderObject &x) {
        return ShaderObject(ShaderDataType::float32(), ShaderNodeFactory::length(x.node));
    }

    inline ShaderObject distance(const ShaderObject &x, const ShaderObject &y) {
        return ShaderObject(ShaderDataType::float32(), ShaderNodeFactory::distance(x.node, y.node));
    }

    inline ShaderObject reflect(const ShaderObject &I, const ShaderObject &N) {
        return ShaderObject(I.type, ShaderNodeFactory::reflect(I.node, N.node));
    }

    inline ShaderObject refract(const ShaderObject &I,
                                const ShaderObject &N,
                                const ShaderObject &eta) {
        return ShaderObject(I.type, ShaderNodeFactory::refract(I.node, N.node, eta.node));
    }

    inline ShaderObject faceforward(const ShaderObject &N,
                                    const ShaderObject &I,
                                    const ShaderObject &Nref) {
        return ShaderObject(N.type, ShaderNodeFactory::faceforward(N.node, I.node, Nref.node));
    }

    inline ShaderObject transpose(const ShaderObject &v) {
        return ShaderObject(v.type, ShaderNodeFactory::transpose(v.node));
    }

    inline ShaderObject inverse(const ShaderObject &v) {
        return ShaderObject(v.type, ShaderNodeFactory::inverse(v.node));
    }

    inline ShaderObject matrix2(const ShaderObject &x) {
        return ShaderObject(ShaderDataType{ShaderDataType::MAT2, x.getDataType().component},
                            ShaderNodeFactory::matrix(ShaderDataType(ShaderDataType::MAT2, x.getDataType().component),
                                                      x.node,
                                                      nullptr,
                                                      nullptr,
                                                      nullptr));
    }

    inline ShaderObject matrix3(const ShaderObject &x) {
        return ShaderObject(ShaderDataType{ShaderDataType::MAT3, x.getDataType().component},
                            ShaderNodeFactory::matrix(ShaderDataType(ShaderDataType::MAT3, x.getDataType().component),
                                                      x.node,
                                                      nullptr,
                                                      nullptr,
                                                      nullptr));
    }

    inline ShaderObject matrix4(const ShaderObject &x) {
        return ShaderObject(ShaderDataType{ShaderDataType::MAT4, x.getDataType().component},
                            ShaderNodeFactory::matrix(ShaderDataType(ShaderDataType::MAT4, x.getDataType().component),
                                                      x.node,
                                                      nullptr,
                                                      nullptr,
                                                      nullptr));
    }

    inline ShaderObject matrix(const ShaderObject &x, const ShaderObject &y) {
        return ShaderObject(ShaderDataType{ShaderDataType::MAT2, x.getDataType().component},
                            ShaderNodeFactory::matrix(ShaderDataType(ShaderDataType::MAT2, x.getDataType().component),
                                                      x.node,
                                                      y.node,
                                                      nullptr,
                                                      nullptr));
    }

    inline ShaderObject matrix(const ShaderObject &x,
                               const ShaderObject &y,
                               const ShaderObject &z) {
        return ShaderObject(ShaderDataType{ShaderDataType::MAT3, x.getDataType().component},
                            ShaderNodeFactory::matrix(ShaderDataType(ShaderDataType::MAT3, x.getDataType().component),
                                                      x.node,
                                                      y.node,
                                                      z.node,
                                                      nullptr));
    }

    inline ShaderObject matrix(const ShaderObject &x,
                               const ShaderObject &y,
                               const ShaderObject &z,
                               const ShaderObject &w) {
        return ShaderObject(ShaderDataType{ShaderDataType::MAT4, x.getDataType().component},
                            ShaderNodeFactory::matrix(ShaderDataType(ShaderDataType::MAT4, x.getDataType().component),
                                                      x.node,
                                                      y.node,
                                                      z.node,
                                                      w.node));
    }
}

#endif //XENGINE_SHADERSCRIPT_HPP
