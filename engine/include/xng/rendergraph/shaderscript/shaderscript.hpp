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

#include "xng/rendergraph/shaderscript/shaderstructdef.hpp"
#include "xng/rendergraph/shaderscript/shaderfunctiondef.hpp"
#include "xng/rendergraph/shaderscript/shaderobject.hpp"

/**
 * WARNING: This file should only be included in compilation units that specifically handle shader creation,
 *          because it defines loosely named macros such as Struct or Input which can collide with other symbols.
 *
 *          The xng::ShaderScript namespace defines implicit conversion operators for converting literals
 *          to shader objects, which can cause ambiguous overloads when "using" the namespace.
 */

// Optional Helper macros for defining C++ function wrappers for directly calling a function
#define DeclareFunction0(name) inline ShaderObject name(){ return xng::ShaderScript::Call(#name);}
#define DeclareFunction1(name) inline ShaderObject name(const ShaderObject &arg){ return xng::ShaderScript::Call(#name, {arg});}
#define DeclareFunction2(name) inline ShaderObject name(const ShaderObject &arg, const ShaderObject &arg1){ return xng::ShaderScript::Call(#name, {arg, arg1});}
#define DeclareFunction3(name) inline ShaderObject name(const ShaderObject &arg, const ShaderObject &arg1, const ShaderObject &arg2){ return xng::ShaderScript::Call(#name, {arg, arg1, arg2});}
#define DeclareFunction4(name) inline ShaderObject name(const ShaderObject &arg, const ShaderObject &arg1, const ShaderObject &arg2, const ShaderObject &arg3){ return xng::ShaderScript::Call(#name, {arg, arg1, arg2, arg3});}
#define DeclareFunction5(name) inline ShaderObject name(const ShaderObject &arg, const ShaderObject &arg1, const ShaderObject &arg2, const ShaderObject &arg3, const ShaderObject &arg4){ return xng::ShaderScript::Call(#name, {arg, arg1, arg2, arg3, arg4});}
#define DeclareFunction6(name) inline ShaderObject name(const ShaderObject &arg, const ShaderObject &arg1, const ShaderObject &arg2, const ShaderObject &arg3, const ShaderObject &arg4, const ShaderObject &arg5){ return xng::ShaderScript::Call(#name, {arg, arg1, arg2, arg3, arg4, arg5});}
#define DeclareFunction7(name) inline ShaderObject name(const ShaderObject &arg, const ShaderObject &arg1, const ShaderObject &arg2, const ShaderObject &arg3, const ShaderObject &arg4, const ShaderObject &arg5, const ShaderObject &arg6){ return xng::ShaderScript::Call(#name, {arg, arg1, arg2, arg3, arg4, arg5, arg6});}
#define DeclareFunction8(name) inline ShaderObject name(const ShaderObject &arg, const ShaderObject &arg1, const ShaderObject &arg2, const ShaderObject &arg3, const ShaderObject &arg4, const ShaderObject &arg5, const ShaderObject &arg6, const ShaderObject &arg7){ return xng::ShaderScript::Call(#name, {arg, arg1, arg2, arg3, arg4, arg5, arg6, arg7});}
#define DeclareFunction9(name) inline ShaderObject name(const ShaderObject &arg, const ShaderObject &arg1, const ShaderObject &arg2, const ShaderObject &arg3, const ShaderObject &arg4, const ShaderObject &arg5, const ShaderObject &arg6, const ShaderObject &arg7, const ShaderObject &arg8){ return xng::ShaderScript::Call(#name, {arg, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8});}
#define DeclareFunction10(name) inline ShaderObject name(const ShaderObject &arg, const ShaderObject &arg1, const ShaderObject &arg2, const ShaderObject &arg3, const ShaderObject &arg4, const ShaderObject &arg5, const ShaderObject &arg6, const ShaderObject &arg7, const ShaderObject &arg8, const ShaderObject &arg9){ return xng::ShaderScript::Call(#name, {arg, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9});}
#define DeclareFunction11(name) inline ShaderObject name(const ShaderObject &arg, const ShaderObject &arg1, const ShaderObject &arg2, const ShaderObject &arg3, const ShaderObject &arg4, const ShaderObject &arg5, const ShaderObject &arg6, const ShaderObject &arg7, const ShaderObject &arg8, const ShaderObject &arg9, const ShaderObject &arg10){ return xng::ShaderScript::Call(#name, {arg, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10});}
#define DeclareFunction12(name) inline ShaderObject name(const ShaderObject &arg, const ShaderObject &arg1, const ShaderObject &arg2, const ShaderObject &arg3, const ShaderObject &arg4, const ShaderObject &arg5, const ShaderObject &arg6, const ShaderObject &arg7, const ShaderObject &arg8, const ShaderObject &arg9, const ShaderObject &arg10, const ShaderObject &arg11){ return xng::ShaderScript::Call(#name, {arg, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11});}

/**
 * Optional Helper Macros for abstracting the shader builder interactions
 *
 * BeginShader(VERTEX);
 *
 * Input(ShaderPrimitiveType::vec3(), vPosition);
 * Input(ShaderPrimitiveType::vec2(), vUv);
 *
 * Output(ShaderPrimitiveType::vec4(), fPosition);
 *
 * Struct(BufferSubData, {
 *      {ShaderPrimitiveType::float(), "value"}
 * });
 *
 * Struct(BufferData, {
 *      {ShaderPrimitiveType::vec2(), "color"},
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

#define DeclareStruct(name) builder.addTypeDefinition(name::getShaderStruct());

#define Input(type, name) builder.addInput(#name, ShaderPrimitiveType::type()); ShaderObject name = xng::ShaderScript::inputAttribute(#name);
#define Output(type, name) builder.addOutput(#name, ShaderPrimitiveType::type()); ShaderObject name = xng::ShaderScript::outputAttribute(#name);

#define Parameter(type, name) builder.addParameter(#name, ShaderPrimitiveType::type()); ShaderObject name = xng::ShaderScript::parameter(#name);

#define Buffer(bufferType, bufferName) builder.addBuffer(#bufferName, ShaderBuffer(false, false, bufferType::getShaderStruct().typeName)); bufferType bufferName(ShaderScript::buffer(#bufferName));
#define DynamicBuffer(bufferType, bufferName) builder.addBuffer(#bufferName, ShaderBuffer(false, true, bufferType::getShaderStruct().typeName)); DynamicBufferWrapper<bufferType> bufferName(ShaderScript::buffer(#bufferName));

#define BufferRW(bufferType, bufferName) builder.addBuffer(#bufferName, ShaderBuffer(true, false, bufferType::getShaderStruct().typeName)); bufferType bufferName(ShaderScript::buffer(#bufferName));
#define DynamicBufferRW(bufferType, bufferName) builder.addBuffer(#bufferName, ShaderBuffer(true, true, bufferType::getShaderStruct().typeName)); DynamicBufferWrapper<bufferType> bufferName(ShaderScript::buffer(#bufferName));

#define Texture(type, format, name) builder.addTextureArray(#name, ShaderTextureArray(ShaderTexture(type, format))); ShaderObject name = xng::ShaderScript::textureSampler(#name)[Int(0)];
#define TextureArray(type, format, count, name) builder.addTextureArray(#name, ShaderTextureArray(ShaderTexture(type, format), count)); ShaderObject name = xng::ShaderScript::textureSampler(#name);

#define InputPrimitive(type) builder.setGeometryInput(type);
#define OutputPrimitive(type, maxVertices) builder.setGeometryOutput(type, maxVertices);

#define BuildShader() builder.build()

#define If(condition) ShaderBuilder::instance().BeginIf(condition);{
#define Else }ShaderBuilder::instance().DoElse();{
#define Fi }ShaderBuilder::instance().EndIf();
#define For(variableType, variableName, initializer, predicate, iterator) { std::string _variableName = ShaderBuilder::instance().getVariableName();\
    variableType variableName(ShaderOperand::variable(_variableName));\
    ShaderBuilder::instance().BeginFor(\
        ShaderOperand::instruction(ShaderInstructionFactory::declareVariable(_variableName, variableType::TYPE, variableType(initializer).operand)),\
        (predicate).operand,\
        ShaderOperand::instruction(ShaderInstructionFactory::assign(variableName.operand, (iterator).operand)));
#define Done } ShaderBuilder::instance().EndFor();

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

    template<ShaderPrimitiveType::Type TYPE, ShaderPrimitiveType::Component COMPONENT, int C>
    using Array = ShaderDataObject<TYPE, COMPONENT, C>;

    template<int C>
    using ArrayBool = Array<ShaderPrimitiveType::SCALAR, ShaderPrimitiveType::BOOLEAN, C>;

    template<int C>
    using ArrayInt = Array<ShaderPrimitiveType::SCALAR, ShaderPrimitiveType::SIGNED_INT, C>;

    template<int C>
    using ArrayUInt = Array<ShaderPrimitiveType::SCALAR, ShaderPrimitiveType::UNSIGNED_INT, C>;

    template<int C>
    using ArrayFloat = Array<ShaderPrimitiveType::SCALAR, ShaderPrimitiveType::FLOAT, C>;

    template<int C>
    using ArrayDouble = Array<ShaderPrimitiveType::SCALAR, ShaderPrimitiveType::DOUBLE, C>;

    template<int C>
    using ArrayBVec2 = Array<ShaderPrimitiveType::VECTOR2, ShaderPrimitiveType::BOOLEAN, C>;

    template<int C>
    using ArrayBVec3 = Array<ShaderPrimitiveType::VECTOR3, ShaderPrimitiveType::BOOLEAN, C>;

    template<int C>
    using ArrayBVec4 = Array<ShaderPrimitiveType::VECTOR4, ShaderPrimitiveType::BOOLEAN, C>;

    template<int C>
    using ArrayIVec2 = Array<ShaderPrimitiveType::VECTOR2, ShaderPrimitiveType::SIGNED_INT, C>;

    template<int C>
    using ArrayIVec3 = Array<ShaderPrimitiveType::VECTOR3, ShaderPrimitiveType::SIGNED_INT, C>;

    template<int C>
    using ArrayIVec4 = Array<ShaderPrimitiveType::VECTOR4, ShaderPrimitiveType::SIGNED_INT, C>;

    template<int C>
    using ArrayUVec2 = Array<ShaderPrimitiveType::VECTOR2, ShaderPrimitiveType::UNSIGNED_INT, C>;

    template<int C>
    using ArrayUVec3 = Array<ShaderPrimitiveType::VECTOR3, ShaderPrimitiveType::UNSIGNED_INT, C>;

    template<int C>
    using ArrayUVec4 = Array<ShaderPrimitiveType::VECTOR4, ShaderPrimitiveType::UNSIGNED_INT, C>;

    template<int C>
    using ArrayVec2 = Array<ShaderPrimitiveType::VECTOR2, ShaderPrimitiveType::FLOAT, C>;

    template<int C>
    using ArrayVec3 = Array<ShaderPrimitiveType::VECTOR3, ShaderPrimitiveType::FLOAT, C>;

    template<int C>
    using ArrayVec4 = Array<ShaderPrimitiveType::VECTOR4, ShaderPrimitiveType::FLOAT, C>;

    template<int C>
    using ArrayMat2 = Array<ShaderPrimitiveType::MAT2, ShaderPrimitiveType::FLOAT, C>;

    template<int C>
    using ArrayMat3 = Array<ShaderPrimitiveType::MAT3, ShaderPrimitiveType::FLOAT, C>;

    template<int C>
    using ArrayMat4 = Array<ShaderPrimitiveType::MAT4, ShaderPrimitiveType::FLOAT, C>;

    template<int C>
    using ArrayDVec2 = Array<ShaderPrimitiveType::VECTOR2, ShaderPrimitiveType::DOUBLE, C>;

    template<int C>
    using ArrayDVec3 = Array<ShaderPrimitiveType::VECTOR3, ShaderPrimitiveType::DOUBLE, C>;

    template<int C>
    using ArrayDVec4 = Array<ShaderPrimitiveType::VECTOR4, ShaderPrimitiveType::DOUBLE, C>;

    template<int C>
    using ArrayDMat2 = Array<ShaderPrimitiveType::MAT2, ShaderPrimitiveType::DOUBLE, C>;

    template<int C>
    using ArrayDMat3 = Array<ShaderPrimitiveType::MAT3, ShaderPrimitiveType::DOUBLE, C>;

    template<int C>
    using ArrayDMat4 = Array<ShaderPrimitiveType::MAT4, ShaderPrimitiveType::DOUBLE, C>;

    template<const char * typeName>
    using Object = ShaderStructObject<typeName, 1>;

    // Texture definitions only needed for function arguments.
    template<ColorFormat C>
    using Texture2D = ShaderObject;

    template<ColorFormat C>
    using Texture2DMS = ShaderObject;

    template<ColorFormat C>
    using TextureCube = ShaderObject;

    template<ColorFormat C>
    using Texture2DArray = ShaderObject;

    template<ColorFormat C>
    using Texture2DMSArray = ShaderObject;

    template<ColorFormat C>
    using TextureCubeArray = ShaderObject;

    typedef ShaderDataObject<ShaderPrimitiveType::SCALAR, ShaderPrimitiveType::BOOLEAN, 1> Bool;

    typedef ShaderDataObject<ShaderPrimitiveType::SCALAR, ShaderPrimitiveType::SIGNED_INT, 1> Int;

    typedef ShaderDataObject<ShaderPrimitiveType::SCALAR, ShaderPrimitiveType::UNSIGNED_INT, 1> UInt;

    typedef ShaderDataObject<ShaderPrimitiveType::SCALAR, ShaderPrimitiveType::FLOAT, 1> Float;

    typedef ShaderDataObject<ShaderPrimitiveType::SCALAR, ShaderPrimitiveType::DOUBLE, 1> Double;

    typedef ShaderDataObject<ShaderPrimitiveType::VECTOR2, ShaderPrimitiveType::FLOAT, 1> vec2;

    typedef ShaderDataObject<ShaderPrimitiveType::VECTOR3, ShaderPrimitiveType::FLOAT, 1> vec3;

    typedef ShaderDataObject<ShaderPrimitiveType::VECTOR4, ShaderPrimitiveType::FLOAT, 1> vec4;

    typedef ShaderDataObject<ShaderPrimitiveType::MAT2, ShaderPrimitiveType::FLOAT, 1> mat2;

    typedef ShaderDataObject<ShaderPrimitiveType::MAT3, ShaderPrimitiveType::FLOAT, 1> mat3;

    typedef ShaderDataObject<ShaderPrimitiveType::MAT4, ShaderPrimitiveType::FLOAT, 1> mat4;

    typedef ShaderDataObject<ShaderPrimitiveType::VECTOR2, ShaderPrimitiveType::DOUBLE, 1> dvec2;

    typedef ShaderDataObject<ShaderPrimitiveType::VECTOR3, ShaderPrimitiveType::DOUBLE, 1> dvec3;

    typedef ShaderDataObject<ShaderPrimitiveType::VECTOR4, ShaderPrimitiveType::DOUBLE, 1> dvec4;

    typedef ShaderDataObject<ShaderPrimitiveType::MAT2, ShaderPrimitiveType::DOUBLE, 1> dmat2;

    typedef ShaderDataObject<ShaderPrimitiveType::MAT3, ShaderPrimitiveType::DOUBLE, 1> dmat3;

    typedef ShaderDataObject<ShaderPrimitiveType::MAT4, ShaderPrimitiveType::DOUBLE, 1> dmat4;

    typedef ShaderDataObject<ShaderPrimitiveType::VECTOR2, ShaderPrimitiveType::BOOLEAN, 1> bvec2;

    typedef ShaderDataObject<ShaderPrimitiveType::VECTOR3, ShaderPrimitiveType::BOOLEAN, 1> bvec3;

    typedef ShaderDataObject<ShaderPrimitiveType::VECTOR4, ShaderPrimitiveType::BOOLEAN, 1> bvec4;

    typedef ShaderDataObject<ShaderPrimitiveType::VECTOR2, ShaderPrimitiveType::SIGNED_INT, 1> ivec2;

    typedef ShaderDataObject<ShaderPrimitiveType::VECTOR3, ShaderPrimitiveType::SIGNED_INT, 1> ivec3;

    typedef ShaderDataObject<ShaderPrimitiveType::VECTOR4, ShaderPrimitiveType::SIGNED_INT, 1> ivec4;

    typedef ShaderDataObject<ShaderPrimitiveType::VECTOR2, ShaderPrimitiveType::UNSIGNED_INT, 1> uvec2;

    typedef ShaderDataObject<ShaderPrimitiveType::VECTOR3, ShaderPrimitiveType::UNSIGNED_INT, 1> uvec3;

    typedef ShaderDataObject<ShaderPrimitiveType::VECTOR4, ShaderPrimitiveType::UNSIGNED_INT, 1> uvec4;

    inline ShaderObject inputAttribute(const std::string &attributeName) {
        return ShaderObject(ShaderOperand(ShaderOperand::InputAttribute, attributeName),
                            true);
    }

    inline ShaderObject outputAttribute(const std::string &attributeName) {
        return ShaderObject(ShaderOperand(ShaderOperand::OutputAttribute, attributeName),
                            true);
    }

    inline ShaderObject parameter(const std::string &name) {
        return ShaderObject(ShaderOperand(ShaderOperand::Parameter, name));
    }

    inline ShaderObject buffer(const std::string &name) {
        return ShaderObject(ShaderOperand(ShaderOperand::Buffer, name));
    }

    inline ShaderObject textureSampler(const std::string &name) {
        return ShaderObject(ShaderOperand(ShaderOperand::Texture, name));
    }

    inline ShaderObject argument(const std::string &name) {
        return ShaderObject(ShaderOperand(ShaderOperand::Argument, name), true);
    }

    inline void setVertexPosition(const ShaderObject &value) {
        ShaderBuilder::instance().addInstruction(ShaderInstructionFactory::setVertexPosition(value.operand));
    }

    inline void setFragmentDepth(const ShaderObject &value) {
        ShaderBuilder::instance().addInstruction((ShaderInstructionFactory::setFragmentDepth(value.operand)));
    }

    inline void setLayer(const ShaderObject &value) {
        ShaderBuilder::instance().addInstruction((ShaderInstructionFactory::setLayer(value.operand)));
    }

    inline void EmitVertex() {
        ShaderBuilder::instance().addInstruction(ShaderInstructionFactory::emitVertex());
    }

    inline void EndPrimitive() {
        ShaderBuilder::instance().addInstruction(ShaderInstructionFactory::endPrimitive());
    }

    inline ShaderObject Call(const std::string &functionName,
                              const std::vector<ShaderObject> &wArgs = {}) {
        std::vector<ShaderOperand> args;
        for (auto &arg: wArgs) {
            args.push_back(arg.operand);
        }
        return ShaderObject(ShaderInstructionFactory::call(functionName, args));
    }

    inline ShaderObject Call(const std::string &functionName,
                             const ShaderObject &arg0) {
        return Call(functionName, std::vector{arg0});
    }

    inline ShaderObject Call(const std::string &functionName,
                             const ShaderObject &arg0,
                             const ShaderObject &arg1) {
        return Call(functionName, std::vector{arg0, arg1});
    }

    inline ShaderObject Call(const std::string &functionName,
                             const ShaderObject &arg0,
                             const ShaderObject &arg1,
                             const ShaderObject &arg2) {
        return Call(functionName, std::vector{arg0, arg1, arg2});
    }

    inline ShaderObject Call(const std::string &functionName,
                             const ShaderObject &arg0,
                             const ShaderObject &arg1,
                             const ShaderObject &arg2,
                             const ShaderObject &arg3) {
        return Call(functionName, std::vector{arg0, arg1, arg2, arg3});
    }

    inline void Return(const ShaderObject &value) {
        ShaderBuilder::instance().addInstruction(ShaderInstructionFactory::ret(value.operand));
    }

    inline void Return() {
        ShaderBuilder::instance().addInstruction(ShaderInstructionFactory::ret());
    }

    inline ShaderObject textureSize(const ShaderObject &texture) {
        return ShaderObject(ShaderInstructionFactory::textureSize(texture.operand));
    }

    inline ShaderObject textureSize(const ShaderObject &texture, const ShaderObject &lod) {
        return ShaderObject(ShaderInstructionFactory::textureSize(texture.operand, lod.operand));
    }

    inline ShaderObject textureSample(const ShaderObject &texture, const ShaderObject &coords) {
        return ShaderObject(ShaderInstructionFactory::textureSample(texture.operand, coords.operand));
    }

    inline ShaderObject textureSample(const ShaderObject &texture,
                                      const ShaderObject &coords,
                                      const ShaderObject &bias) {
        return ShaderObject(ShaderInstructionFactory::textureSample(texture.operand,
                                                                    coords.operand,
                                                                    bias.operand));
    }

    inline ShaderObject textureSampleArray(const ShaderObject &texture, const ShaderObject &coords) {
        return ShaderObject(ShaderInstructionFactory::textureSampleArray(texture.operand, coords.operand));
    }

    inline ShaderObject textureSampleArray(const ShaderObject &texture,
                                           const ShaderObject &coords,
                                           const ShaderObject &bias) {
        return ShaderObject(ShaderInstructionFactory::textureSampleArray(texture.operand,
                                                                         coords.operand,
                                                                         bias.operand));
    }

    inline ShaderObject texelFetch(const ShaderObject &texture,
                                   const ShaderObject &coords,
                                   const ShaderObject &index) {
        return ShaderObject(ShaderInstructionFactory::textureFetch(texture.operand,
                                                                   coords.operand,
                                                                   index.operand));
    }

    inline ShaderObject texelFetchArray(const ShaderObject &texture,
                                        const ShaderObject &coords,
                                        const ShaderObject &index) {
        return ShaderObject(ShaderInstructionFactory::textureFetchArray(texture.operand,
                                                                        coords.operand,
                                                                        index.operand));
    }

    inline ShaderObject texelFetchMS(const ShaderObject &texture,
                                     const ShaderObject &coords,
                                     const ShaderObject &index) {
        return ShaderObject(ShaderInstructionFactory::textureFetchMS(texture.operand,
                                                                     coords.operand,
                                                                     index.operand));
    }

    inline ShaderObject texelFetchMSArray(const ShaderObject &texture,
                                          const ShaderObject &coords,
                                          const ShaderObject &index) {
        return ShaderObject(ShaderInstructionFactory::textureFetchMSArray(texture.operand,
                                                                          coords.operand,
                                                                          index.operand));
    }

    inline ShaderObject textureSampleCube(const ShaderObject &texture,
                                          const ShaderObject &coords) {
        return ShaderObject(ShaderInstructionFactory::textureSampleCubeMap(texture.operand, coords.operand));
    }

    inline ShaderObject textureSampleCube(const ShaderObject &texture,
                                          const ShaderObject &coords,
                                          const ShaderObject &bias) {
        return ShaderObject(ShaderInstructionFactory::textureSampleCubeMap(texture.operand,
                                                                           coords.operand,
                                                                           bias.operand));
    }

    inline ShaderObject textureSampleCubeArray(const ShaderObject &texture,
                                               const ShaderObject &coords) {
        return ShaderObject(ShaderInstructionFactory::textureSampleCubeMapArray(texture.operand, coords.operand));
    }

    inline ShaderObject textureSampleCubeArray(const ShaderObject &texture,
                                               const ShaderObject &coords,
                                               const ShaderObject &bias) {
        return ShaderObject(ShaderInstructionFactory::textureSampleCubeMapArray(texture.operand,
            coords.operand,
            bias.operand));
    }

    inline ShaderObject abs(const ShaderObject &value) {
        return ShaderObject(ShaderInstructionFactory::abs(value.operand));
    }

    inline ShaderObject sin(const ShaderObject &value) {
        return ShaderObject(ShaderInstructionFactory::sin(value.operand));
    }

    inline ShaderObject cos(const ShaderObject &value) {
        return ShaderObject(ShaderInstructionFactory::cos(value.operand));
    }

    inline ShaderObject tan(const ShaderObject &value) {
        return ShaderObject(ShaderInstructionFactory::tan(value.operand));
    }

    inline ShaderObject asin(const ShaderObject &value) {
        return ShaderObject(ShaderInstructionFactory::asin(value.operand));
    }

    inline ShaderObject acos(const ShaderObject &value) {
        return ShaderObject(ShaderInstructionFactory::acos(value.operand));
    }

    inline ShaderObject atan(const ShaderObject &value) {
        return ShaderObject(ShaderInstructionFactory::atan(value.operand));
    }

    inline ShaderObject pow(const ShaderObject &value, const ShaderObject &exponent) {
        return ShaderObject(ShaderInstructionFactory::pow(value.operand, exponent.operand));
    }

    inline ShaderObject exp(const ShaderObject &value) {
        return ShaderObject(ShaderInstructionFactory::exp(value.operand));
    }

    inline ShaderObject log(const ShaderObject &value) {
        return ShaderObject(ShaderInstructionFactory::log(value.operand));
    }

    inline ShaderObject sqrt(const ShaderObject &value) {
        return ShaderObject(ShaderInstructionFactory::sqrt(value.operand));
    }

    inline ShaderObject inverseSqrt(const ShaderObject &value) {
        return ShaderObject(ShaderInstructionFactory::inverseSqrt(value.operand));
    }

    inline ShaderObject floor(const ShaderObject &value) {
        return ShaderObject(ShaderInstructionFactory::floor(value.operand));
    }

    inline ShaderObject ceil(const ShaderObject &value) {
        return ShaderObject(ShaderInstructionFactory::ceil(value.operand));
    }

    inline ShaderObject round(const ShaderObject &value) {
        return ShaderObject(ShaderInstructionFactory::round(value.operand));
    }

    inline ShaderObject fract(const ShaderObject &value) {
        return ShaderObject(ShaderInstructionFactory::fract(value.operand));
    }

    inline ShaderObject mod(const ShaderObject &value, const ShaderObject &modulus) {
        return ShaderObject(ShaderInstructionFactory::mod(value.operand, modulus.operand));
    }

    inline ShaderObject min(const ShaderObject &x, const ShaderObject &y) {
        return ShaderObject(ShaderInstructionFactory::min(x.operand, y.operand));
    }

    inline ShaderObject max(const ShaderObject &x, const ShaderObject &y) {
        return ShaderObject(ShaderInstructionFactory::max(x.operand, y.operand));
    }

    inline ShaderObject clamp(const ShaderObject &value,
                              const ShaderObject &min,
                              const ShaderObject &max) {
        return ShaderObject(ShaderInstructionFactory::clamp(value.operand, min.operand, max.operand));
    }

    inline ShaderObject mix(const ShaderObject &x,
                            const ShaderObject &y,
                            const ShaderObject &a) {
        return ShaderObject(ShaderInstructionFactory::mix(x.operand, y.operand, a.operand));
    }

    inline ShaderObject step(const ShaderObject &edge, const ShaderObject &x) {
        return ShaderObject(ShaderInstructionFactory::step(edge.operand, x.operand));
    }

    inline ShaderObject smoothstep(const ShaderObject &edge0,
                                   const ShaderObject &edge1,
                                   const ShaderObject &x) {
        return ShaderObject(ShaderInstructionFactory::smoothstep(edge0.operand, edge1.operand, x.operand));
    }

    inline ShaderObject dot(const ShaderObject &x, const ShaderObject &y) {
        return ShaderObject(ShaderInstructionFactory::dot(x.operand, y.operand));
    }

    inline ShaderObject cross(const ShaderObject &x, const ShaderObject &y) {
        return ShaderObject(ShaderInstructionFactory::cross(x.operand, y.operand));
    }

    inline ShaderObject normalize(const ShaderObject &x) {
        return ShaderObject(ShaderInstructionFactory::normalize(x.operand));
    }

    inline ShaderObject length(const ShaderObject &x) {
        return ShaderObject(ShaderInstructionFactory::length(x.operand));
    }

    inline ShaderObject distance(const ShaderObject &x, const ShaderObject &y) {
        return ShaderObject(ShaderInstructionFactory::distance(x.operand, y.operand));
    }

    inline ShaderObject reflect(const ShaderObject &I, const ShaderObject &N) {
        return ShaderObject(ShaderInstructionFactory::reflect(I.operand, N.operand));
    }

    inline ShaderObject refract(const ShaderObject &I,
                                const ShaderObject &N,
                                const ShaderObject &eta) {
        return ShaderObject(ShaderInstructionFactory::refract(I.operand, N.operand, eta.operand));
    }

    inline ShaderObject faceforward(const ShaderObject &N,
                                    const ShaderObject &I,
                                    const ShaderObject &Nref) {
        return ShaderObject(ShaderInstructionFactory::faceForward(N.operand, I.operand, Nref.operand));
    }

    inline ShaderObject transpose(const ShaderObject &v) {
        return ShaderObject(ShaderInstructionFactory::transpose(v.operand));
    }

    inline ShaderObject inverse(const ShaderObject &v) {
        return ShaderObject(ShaderInstructionFactory::inverse(v.operand));
    }

    template<typename T>
    struct DynamicBufferWrapper {
        ShaderObject object;

        explicit DynamicBufferWrapper(ShaderObject &&buffer) : object(buffer) {
        }

        T operator[](const Int &index) {
            return object[index];
        }

        T operator[](const int index) {
            return object[Int(index)];
        }

        ShaderObject length() {
            return object.length();
        }
    };
}

#endif //XENGINE_SHADERSCRIPT_HPP
