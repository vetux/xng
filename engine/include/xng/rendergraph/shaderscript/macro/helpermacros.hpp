/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 3 of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef XENGINE_HELPERMACROS_HPP
#define XENGINE_HELPERMACROS_HPP

/**
 * This header defines various macros to make the resulting DSL syntax more clean. It is completely optional to use/include.
 */

#define If(cond) { BranchBuilder _branch; _branch.RecordIf((cond).operand); {
#define Else } _branch.RecordElse(); {
#define Fi } _branch.Finish(); BlockScope::get().addInstruction(_branch.build()); }

#define For(variableType, variableName, initializer, predicate, iterator) { LoopBuilder _loop;\
    variableType variableName;\
    _loop.BeginFor((variableName.operand), (initializer).operand, (predicate).operand, (iterator).operand); {
#define Done } _loop.EndFor(); BlockScope::get().addInstruction(_loop.build()); }

/**
 * Define the beginning of a shader side function.
 * To be used inside a C++ function with matching signature.
 * E.g.
 * vec2 myFunction(Param<vec2> vecA, Param<vec2> vecB)
 * {
 *      IRFunction
 *      IRReturn(vecA + vecB)
 *      IRFunctionEnd
 * }
 */
#define IRFunction\
    if (ShaderScope::get().hasFunction(__func__))\
    {\
        auto args = IRBaseParam::getArgumentValues();\
        IRBaseParam::clear();\
        return ShaderObject(ShaderInstructionFactory::call(__func__, args));\
    }\
    auto _func = std::make_unique<FunctionScope>(__func__);\

#define IRReturn(value)\
    _func->setReturnType(value);\
    ShaderScript::Return(value);

#define IRFunctionEnd\
    ShaderScope::get().addFunction(_func->build());\
    { auto _callArgs = _func->getArgumentValues();\
    _func.reset();\
    return ShaderObject(ShaderInstructionFactory::call(__func__, std::move(_callArgs))); }

#define BeginShader(stage) ShaderScope _s(stage); FunctionScope _main("main");

#define Input(type, name) ShaderObject name = ShaderScope::get().addInput<type>(#name);
#define Output(type, name) ShaderObject name = ShaderScope::get().addOutput<type>(#name);

#define Parameter(type, name) ShaderScope::get().addParameter(#name, ShaderPrimitiveType::type()); ShaderObject name = xng::ShaderScript::parameter(#name);

#define Buffer(bufferType, bufferName) ShaderScope::get().addBuffer(#bufferName, ShaderBuffer(false, false, bufferType::getShaderStructDef().typeName)); bufferType bufferName(ShaderScript::buffer(#bufferName));
#define DynamicBuffer(bufferType, bufferName) ShaderScope::get().addBuffer(#bufferName, ShaderBuffer(false, true, bufferType::getShaderStructDef().typeName)); DynamicBufferWrapper<bufferType> bufferName(ShaderScript::buffer(#bufferName));

#define BufferRW(bufferType, bufferName) ShaderScope::get().addBuffer(#bufferName, ShaderBuffer(true, false, bufferType::getShaderStructDef().typeName)); bufferType bufferName(ShaderScript::buffer(#bufferName));
#define DynamicBufferRW(bufferType, bufferName) ShaderScope::get().addBuffer(#bufferName, ShaderBuffer(true, true, bufferType::getShaderStructDef().typeName)); DynamicBufferWrapper<bufferType> bufferName(ShaderScript::buffer(#bufferName));

#define Texture(type, format, name) ShaderScope::get().addTextureArray(#name, ShaderTextureArray(ShaderTexture(type, format))); ShaderObject name = xng::ShaderScript::textureSampler(#name)[Int(0)];
#define TextureArray(type, format, count, name) ShaderScope::get().addTextureArray(#name, ShaderTextureArray(ShaderTexture(type, format), count)); ShaderObject name = xng::ShaderScript::textureSampler(#name);

#define InputPrimitive(type) ShaderScope::get().setGeometryInput(type);
#define OutputPrimitive(type, maxVertices) ShaderScope::get().setGeometryOutput(type, maxVertices);

#define EndShader() ShaderScope::get().addFunction(_main.build());

#define BuildShader() ShaderScope::get().build();

#endif //XENGINE_HELPERMACROS_HPP