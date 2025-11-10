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

#ifndef XENGINE_SHADERINSTRUCTION_HPP
#define XENGINE_SHADERINSTRUCTION_HPP

#include <memory>
#include <utility>
#include <variant>

#include "xng/rendergraph/shader/shaderdatatype.hpp"

namespace xng {
    struct ShaderOperand;

    struct ShaderInstruction;

    typedef std::variant<ShaderDataType,
        ShaderPrimitiveType,
        ShaderPrimitiveType::VectorComponent,
        std::vector<ShaderInstruction>,
        std::string> ShaderInstructionData;

    struct ShaderInstruction {
        enum OpCode {
            // Variable Declaration
            DeclareVariable = 0,

            // Data Assignment
            Assign,

            // Control Flow
            Branch,
            Loop,
            CallFunction,
            Return,

            // Geometry Shader
            EmitVertex,
            EndPrimitive,

            // Built-In Data Assignment
            SetFragmentDepth,
            SetLayer,
            SetVertexPosition,

            // Object Subscripting
            VectorSwizzle,
            ArraySubscript,
            MatrixSubscript,
            ObjectMember,

            // Constructors
            CreateArray,
            CreateMatrix,
            CreateVector,
            CreateStruct,

            // Texture / Buffer Access
            TextureSample,
            TextureSampleArray,

            TextureSampleCubeMap,
            TextureSampleCubeMapArray,

            TextureFetch,
            TextureFetchArray,

            TextureFetchMS,
            TextureFetchMSArray,

            TextureSize,
            BufferSize,

            // Arithmetic
            Add,
            Subtract,
            Multiply,
            Divide,
            LogicalAnd,
            LogicalOr,
            GreaterEqual,
            Greater,
            LessEqual,
            Less,
            Equal,
            NotEqual,

            // Built-In Functions
            Abs,
            Sin,
            Cos,
            Tan,
            Asin,
            Acos,
            Atan,
            Pow,
            Exp,
            Log,
            Sqrt,
            InverseSqrt,
            Floor,
            Ceil,
            Round,
            Fract,
            Mod,
            Min,
            Max,
            Clamp,
            Mix,
            Step,
            SmoothStep,
            Dot,
            Cross,
            Normalize,
            Length,
            Distance,
            Reflect,
            Refract,
            FaceForward,
            Transpose,
            Inverse,

            LHS_START = DeclareVariable,
            LHS_END = VectorSwizzle,
        } code = DeclareVariable;

        std::vector<ShaderOperand> operands{}; // Operand types can be dynamic depending on the instruction format.
        std::vector<ShaderInstructionData> data{}; // Data types are statically defined by the instruction format.
    };

    struct ShaderOperand {
        enum OperandType : int {
            None = 0,
            Instruction,
            Buffer,
            Texture,
            Parameter,
            InputAttribute,
            OutputAttribute,
            Argument,
            Variable,
            Literal,
        } type{};

        std::variant<ShaderInstruction, ShaderPrimitive, std::string> value{};

        ShaderOperand() = default;

        ShaderOperand(const OperandType type, std::variant<ShaderInstruction, ShaderPrimitive, std::string> val)
            : type(type), value(std::move(val)) {
        }

        static ShaderOperand instruction(ShaderInstruction instruction) {
            return {Instruction, std::move(instruction)};
        }

        static ShaderOperand buffer(std::string name) {
            return {Buffer, std::move(name)};
        }

        static ShaderOperand texture(std::string name) {
            return {Texture, std::move(name)};
        }

        static ShaderOperand parameter(std::string name) {
            return {Parameter, std::move(name)};
        }

        static ShaderOperand inputAttribute(std::string name) {
            return {InputAttribute, std::move(name)};
        }

        static ShaderOperand outputAttribute(std::string name) {
            return {OutputAttribute, std::move(name)};
        }

        static ShaderOperand argument(std::string name) {
            return {Argument, std::move(name)};
        }

        static ShaderOperand variable(std::string name) {
            return {Variable, std::move(name)};
        }

        static ShaderOperand literal(ShaderPrimitive value) {
            return {Literal, std::move(value)};
        }

        static ShaderOperand literal(ShaderPrimitive::Value value) {
            return {Literal, std::move(value)};
        }

        bool isAssigned() const {
            return type != None;
        }
    };
}

#endif //XENGINE_SHADERINSTRUCTION_HPP
