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

#ifndef XENGINE_FGSHADEROPERATION_HPP
#define XENGINE_FGSHADEROPERATION_HPP

#include <utility>
#include <vector>
#include <cstddef>
#include <variant>
#include <memory>

#include "xng/render/graph2/shader/fgshadervalue.hpp"

#include "xng/math/vector2.hpp"
#include "xng/math/vector3.hpp"
#include "xng/math/vector4.hpp"
#include "xng/math/matrix.hpp"

namespace xng {
    struct FGShaderOperation {
        enum Op {
            ATTRIBUTE_READ,
            ATTRIBUTE_WRITE,

            PARAMETER_READ,
            PARAMETER_WRITE,

            TEXTURE_READ,
            TEXTURE_WRITE,

            ASSIGN,

            ADD,
            SUBTRACT,
            MULTIPLY,
            DIVIDE,

            COMPARE_EQUAL,
            COMPARE_NEQUAL,
            COMPARE_MORE,
            COMPARE_LESS,
            COMPARE_MORE_EQUAL,
            COMPARE_LESS_EQUAL,

            LOGICAL_AND,
            LOGICAL_OR,

            CALL,
            RETURN, //

            SUBSCRIPT, //Array, Vector or matrix indexing

            CONDITIONAL,
            LOOP,
        } operation;

        FGShaderValue result;
        std::vector<FGShaderValue> inputs;

        std::vector<FGShaderOperation> predicate;

        std::vector<FGShaderOperation> nextBranch;
        std::vector<FGShaderOperation> alternativeBranch;

        std::vector<FGShaderOperation> loopInitializer;
        std::vector<FGShaderOperation> loopIterator;
        std::vector<FGShaderOperation> loopBody;

        std::string targetName; // Either the name of a parameter, a texture or a function
        uint32_t attributeBinding; // The binding index of an attribute

        FGShaderOperation(const Op operation,
                          FGShaderValue result,
                          const std::vector<FGShaderValue> &inputs,
                          const std::vector<FGShaderOperation> &predicate,
                          const std::vector<FGShaderOperation> &next_branch,
                          const std::vector<FGShaderOperation> &alternative_branch,
                          const std::vector<FGShaderOperation> &loop_initializer,
                          const std::vector<FGShaderOperation> &loop_iterator,
                          const std::vector<FGShaderOperation> &loop_body,
                          std::string target_name,
                          const uint32_t attributeBinding)
            : operation(operation),
              result(std::move(result)),
              inputs(inputs),
              predicate(predicate),
              nextBranch(next_branch),
              alternativeBranch(alternative_branch),
              loopInitializer(loop_initializer),
              loopIterator(loop_iterator),
              loopBody(loop_body),
              targetName(std::move(target_name)),
              attributeBinding(attributeBinding) {
        }

        FGShaderOperation(const Op operation, FGShaderValue result, const std::vector<FGShaderValue> &inputs)
            : operation(operation),
              result(std::move(result)),
              inputs(inputs),
              predicate({}),
              nextBranch({}),
              alternativeBranch({}),
              loopInitializer({}),
              loopIterator({}),
              loopBody({}),
              targetName({}),
              attributeBinding() {
        }

        FGShaderOperation(const Op operation,
                          FGShaderValue result,
                          const std::vector<FGShaderValue> &inputs,
                          std::string targetName)
            : operation(operation),
              result(std::move(result)),
              inputs(inputs),
              predicate({}),
              nextBranch({}),
              alternativeBranch({}),
              loopInitializer({}),
              loopIterator({}),
              loopBody({}),
              targetName(std::move(targetName)),
              attributeBinding() {
        }

        FGShaderOperation(const Op operation,
                          FGShaderValue result,
                          const std::vector<FGShaderValue> &inputs,
                          const uint32_t attributeBinding)
            : operation(operation),
              result(std::move(result)),
              inputs(inputs),
              predicate({}),
              nextBranch({}),
              alternativeBranch({}),
              loopInitializer({}),
              loopIterator({}),
              loopBody({}),
              targetName({}),
              attributeBinding(attributeBinding) {
        }

        static FGShaderOperation readAttribute(uint32_t attributeBinding, FGShaderVariable output) {
            return {ATTRIBUTE_READ, std::move(output), {}, attributeBinding};
        }

        static FGShaderOperation writeAttribute(uint32_t attributeBinding, FGShaderValue value) {
            return {ATTRIBUTE_WRITE, {}, {std::move(value)}, attributeBinding};
        }

        static FGShaderOperation readParameter(const std::string &parameterName, FGShaderVariable output) {
            return {PARAMETER_READ, std::move(output), {}, parameterName};
        }

        static FGShaderOperation writeParameter(const std::string &parameterName, FGShaderValue value) {
            return {PARAMETER_WRITE, {}, {std::move(value)}, parameterName};
        }

        static FGShaderOperation readTexture(const std::string &textureName,
                                             FGShaderVariable output,
                                             FGShaderValue x,
                                             FGShaderValue y,
                                             FGShaderValue z,
                                             FGShaderValue bias) {
            return {
                TEXTURE_READ,
                std::move(output),
                {std::move(x), std::move(y), std::move(z), std::move(bias)},
                textureName
            };
        }

        static FGShaderOperation writeTexture(const std::string &textureName,
                                              FGShaderValue color,
                                              FGShaderValue x,
                                              FGShaderValue y,
                                              FGShaderValue z,
                                              FGShaderValue bias) {
            return {
                TEXTURE_WRITE,
                {},
                {std::move(color), std::move(x), std::move(y), std::move(z), std::move(bias)},
                textureName
            };
        }

        static FGShaderOperation assign(FGShaderVariable target, FGShaderValue value) {
            return {ASSIGN, {}, {target, std::move(value)}};
        }

        static FGShaderOperation add(FGShaderValue a, FGShaderValue b, FGShaderVariable output) {
            return {ADD, std::move(output), {std::move(a), std::move(b)}};
        }

        static FGShaderOperation subtract(FGShaderValue a, FGShaderValue b, FGShaderVariable output) {
            return {SUBTRACT, std::move(output), {std::move(a), std::move(b)}};
        }

        static FGShaderOperation multiply(FGShaderValue a, FGShaderValue b, FGShaderVariable output) {
            return {MULTIPLY, std::move(output), {std::move(a), std::move(b)}};
        }

        static FGShaderOperation divide(FGShaderValue a, FGShaderValue b, FGShaderVariable output) {
            return {DIVIDE, std::move(output), {std::move(a), std::move(b)}};
        }

        static FGShaderOperation equal(FGShaderValue a, FGShaderValue b, FGShaderVariable output) {
            return {COMPARE_EQUAL, std::move(output), {std::move(a), std::move(b)}};
        }

        static FGShaderOperation notEqual(FGShaderValue a, FGShaderValue b, FGShaderVariable output) {
            return {COMPARE_NEQUAL, std::move(output), {std::move(a), std::move(b)}};
        }

        static FGShaderOperation more(FGShaderValue a, FGShaderValue b, FGShaderVariable output) {
            return {COMPARE_MORE, std::move(output), {std::move(a), std::move(b)}};
        }

        static FGShaderOperation less(FGShaderValue a, FGShaderValue b, FGShaderVariable output) {
            return {COMPARE_LESS, std::move(output), {std::move(a), std::move(b)}};
        }

        static FGShaderOperation moreEqual(FGShaderValue a, FGShaderValue b, FGShaderVariable output) {
            return {COMPARE_MORE_EQUAL, std::move(output), {std::move(a), std::move(b)}};
        }

        static FGShaderOperation lessEqual(FGShaderValue a, FGShaderValue b, FGShaderVariable output) {
            return {COMPARE_LESS_EQUAL, std::move(output), {std::move(a), std::move(b)}};
        }

        static FGShaderOperation logicalAnd(FGShaderValue a, FGShaderValue b, FGShaderVariable output) {
            return {LOGICAL_AND, std::move(output), {std::move(a), std::move(b)}};
        }

        static FGShaderOperation logicalOr(FGShaderValue a, FGShaderValue b, FGShaderVariable output) {
            return {LOGICAL_OR, std::move(output), {std::move(a), std::move(b)}};
        }

        static FGShaderOperation callFunction(const std::string &functionName,
                                              const std::vector<FGShaderValue> &arguments,
                                              FGShaderVariable output) {
            return {CALL, std::move(output), arguments, functionName};
        }

        static FGShaderOperation returnFunction(FGShaderValue output) {
            return {RETURN, std::move(output), {}};
        }

        static FGShaderOperation subscript(FGShaderValue value,
                                           FGShaderValue row,
                                           FGShaderValue column,
                                           FGShaderVariable output) {
            return {
                SUBSCRIPT,
                std::move(output),
                {std::move(value), std::move(row), std::move(column)}
            };
        }

        static FGShaderOperation conditional(const std::vector<FGShaderOperation> &condition,
                                             const std::vector<FGShaderOperation> &thenPath,
                                             const std::vector<FGShaderOperation> &elsePath) {
            return FGShaderOperation{
                CONDITIONAL,
                {},
                {},
                condition,
                thenPath,
                elsePath,
                {},
                {},
                {},
                {},
                {}
            };
        }

        static FGShaderOperation loopWhile(const std::vector<FGShaderOperation> &predicate,
                                           const std::vector<FGShaderOperation> &body) {
            return FGShaderOperation{
                LOOP,
                {},
                {},
                predicate,
                {},
                {},
                {},
                {},
                body,
                {},
                {}
            };
        }

        static FGShaderOperation loopFor(const std::vector<FGShaderOperation> &initializer,
                                         const std::vector<FGShaderOperation> &predicate,
                                         const std::vector<FGShaderOperation> &iterator,
                                         const std::vector<FGShaderOperation> &body) {
            return FGShaderOperation{
                LOOP,
                {},
                {},
                predicate,
                {},
                {},
                initializer,
                iterator,
                body,
                {},
                {}
            };
        }
    };
}

#endif //XENGINE_FGSHADEROPERATION_HPP
