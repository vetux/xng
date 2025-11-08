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
#include <optional>

#include "xng/rendergraph/shader/shaderdatatype.hpp"
#include "xng/rendergraph/shader/shaderliteral.hpp"
#include "xng/rendergraph/shader/shaderstruct.hpp"

namespace xng {
    struct ShaderOperand;

    struct ShaderInstruction {
        enum VectorComponent : int {
            COMPONENT_x = 0,
            COMPONENT_y,
            COMPONENT_z,
            COMPONENT_w
        };

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

        std::vector<ShaderOperand> operands{};

        std::string name{};
        std::variant<ShaderDataType, ShaderStructTypeName> type{};

        std::vector<ShaderInstruction> branchA{};
        std::vector<ShaderInstruction> branchB{};

        std::vector<VectorComponent> components{};
    };

    struct ShaderOperand {
        enum Type : int {
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

        ShaderInstruction instruction;
        std::string name;
        ShaderLiteral literal{};

        ShaderOperand() = default;

        explicit ShaderOperand(ShaderInstruction instruction)
            : type(Instruction), instruction(std::move(instruction)) {
        }

        ShaderOperand(const Type type, std::string name)
            : type(type), name(std::move(name)) {
        }

        explicit ShaderOperand(ShaderLiteral literal)
            : type(Literal),
              literal(std::move(literal)) {
        }

        bool isAssigned() const {
            return type != None;
        }
    };
}

#endif //XENGINE_SHADERINSTRUCTION_HPP
