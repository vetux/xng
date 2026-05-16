/**
 *   xEngine - C++ Game Engine Library
 *   Copyright (C) 2026 Julia Zampiccoli
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the Lesser General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef XENGINE_RENDERGRAPH_HELPERMACROS_HPP
#define XENGINE_RENDERGRAPH_HELPERMACROS_HPP

#include "xng/shaderscript/branchbuilder.hpp"
#include "xng/shaderscript/loopbuilder.hpp"
#include "xng/shaderscript/functionscope.hpp"
#include "xng/shaderscript/shaderobject.hpp"

/**
 * This header defines various macros to make the resulting DSL syntax more clean. It is completely optional to use/include.
 */

#define If(cond) { xng::ShaderScript::BranchBuilder _branch; _branch.RecordIf((cond).operand); {
#define Else } _branch.RecordElse(); {
#define Fi } _branch.Finish(); xng::ShaderScript::BlockScope::get().addInstruction(_branch.build()); }

#define For(variableType, variableName, initializer, predicate, iterator) { xng::ShaderScript::LoopBuilder _loop;\
    variableType variableName;\
    _loop.BeginFor((variableName.operand), (initializer).operand, (predicate).operand, (iterator).operand); {
#define Done } _loop.EndFor(); xng::ShaderScript::BlockScope::get().addInstruction(_loop.build()); }

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
    if (xng::ShaderScript::ShaderScope::get().hasFunction(__func__))\
    {\
        auto args = xng::ShaderScript::IRBaseParam::getArgumentValues();\
        xng::ShaderScript::IRBaseParam::clear();\
        return xng::ShaderScript::ShaderObject(xng::rg::ShaderInstructionFactory::call(__func__, args));\
    }\
    auto _func = std::make_unique<xng::ShaderScript::FunctionScope>(__func__);\

#define IRReturn(value)\
    _func->setReturnType(value);\
    xng::ShaderScript::Return(value);

#define IRFunctionEnd\
    xng::ShaderScript::ShaderScope::get().addFunction(_func->build());\
    { auto _callArgs = _func->getArgumentValues();\
    _func.reset();\
    return xng::ShaderScript::ShaderObject(xng::rg::ShaderInstructionFactory::call(__func__, std::move(_callArgs))); }

#define BeginShader(stage) xng::ShaderScript::ShaderScope _s(stage); xng::ShaderScript::FunctionScope _main("main");

#define Input(type, name) xng::ShaderScript::ShaderObject name = xng::ShaderScript::ShaderScope::get().addInput<type>(#name);
#define Output(type, name) xng::ShaderScript::ShaderObject name = xng::ShaderScript::ShaderScope::get().addOutput<type>(#name);

#define InputFlat(type, name) xng::ShaderScript::ShaderObject name = xng::ShaderScript::ShaderScope::get().addInput<type>(#name, xng::rg::ShaderAttributeLayout::INTERPOLATE_FLAT);
#define OutputFlat(type, name) xng::ShaderScript::ShaderObject name = xng::ShaderScript::ShaderScope::get().addOutput<type>(#name, xng::rg::ShaderAttributeLayout::INTERPOLATE_FLAT);

#define InputNoPerspective(type, name) xng::ShaderScript::ShaderObject name = xng::ShaderScript::ShaderScope::get().addInput<type>(#name, xng::rg::ShaderAttributeLayout::INTERPOLATE_NO_PERSPECTIVE);
#define OutputNoPerspective(type, name) xng::ShaderScript::ShaderObject name = xng::ShaderScript::ShaderScope::get().addOutput<type>(#name, xng::rg::ShaderAttributeLayout::INTERPOLATE_NO_PERSPECTIVE);

#define Parameter(type, name) xng::ShaderScript::ShaderScope::get().addParameter(#name, xng::rg::ShaderPrimitiveType::type()); xng::ShaderScript::ShaderObject name = xng::ShaderScript::parameter(#name);

#define Buffer(bufferType, bufferName) xng::ShaderScript::ShaderScope::get().addBuffer(#bufferName, xng::rg::ShaderBuffer(false, false, bufferType::TYPE)); bufferType bufferName(xng::ShaderScript::buffer(#bufferName));
#define DynamicBuffer(bufferType, bufferName) xng::ShaderScript::ShaderScope::get().addBuffer(#bufferName, xng::rg::ShaderBuffer(false, true, bufferType::TYPE)); xng::ShaderScript::DynamicBufferWrapper<bufferType> bufferName(xng::ShaderScript::buffer(#bufferName));

#define BufferRW(bufferType, bufferName) xng::ShaderScript::ShaderScope::get().addBuffer(#bufferName, xng::rg::ShaderBuffer(true, false, bufferType::TYPE)); bufferType bufferName(xng::ShaderScript::buffer(#bufferName));
#define DynamicBufferRW(bufferType, bufferName) xng::ShaderScript::ShaderScope::get().addBuffer(#bufferName, xng::rg::ShaderBuffer(true, true, bufferType::TYPE)); xng::ShaderScript::DynamicBufferWrapper<bufferType> bufferName(xng::ShaderScript::buffer(#bufferName))

#define Texture(type, format, name) xng::ShaderScript::ShaderScope::get().addTextureArray(#name, xng::rg::ShaderTextureArray(xng::rg::ShaderTexture(type, format))); xng::ShaderScript::ShaderObject name = xng::ShaderScript::textureSampler(#name)[Int(0)];
#define TextureArray(type, format, count, name) xng::ShaderScript::ShaderScope::get().addTextureArray(#name, xng::rg::ShaderTextureArray(xng::rg::ShaderTexture(type, format), count)); xng::ShaderScript::ShaderObject name = xng::ShaderScript::textureSampler(#name);

#define InputPrimitive(type) xng::ShaderScript::ShaderScope::get().setGeometryInput(type);
#define OutputPrimitive(type, maxVertices) xng::ShaderScript::ShaderScope::get().setGeometryOutput(type, maxVertices);

#define EndShader() xng::ShaderScript::ShaderScope::get().addFunction(_main.build());

#define BuildShader() xng::ShaderScript::ShaderScope::get().build();

#endif //XENGINE_RENDERGRAPH_HELPERMACROS_HPP