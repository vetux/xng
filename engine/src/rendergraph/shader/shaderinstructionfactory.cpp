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

#include "xng/rendergraph/shader/shaderinstructionfactory.hpp"

namespace xng::ShaderInstructionFactory {
    ShaderInstruction declareVariable(std::string name,
                                      ShaderDataType type,
                                      ShaderOperand value) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::DeclareVariable;
        ret.operands.push_back(std::move(value));
        ret.data.emplace_back(std::move(type));
        ret.data.emplace_back(std::move(name));
        return ret;
    }

    ShaderInstruction assign(ShaderOperand target, ShaderOperand value) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::Assign;
        ret.operands.push_back(std::move(target));
        ret.operands.push_back(std::move(value));
        return ret;
    }

    ShaderInstruction branch(ShaderOperand condition,
                             std::vector<ShaderInstruction> trueBranch,
                             std::vector<ShaderInstruction> falseBranch) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::Branch;
        ret.operands.push_back(std::move(condition));
        ret.data.emplace_back(std::move(trueBranch));
        ret.data.emplace_back(std::move(falseBranch));
        return ret;
    }

    ShaderInstruction loop(ShaderOperand initializer,
                           ShaderOperand predicate,
                           ShaderOperand iterator,
                           std::vector<ShaderInstruction> body) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::Loop;
        ret.operands.push_back(std::move(initializer));
        ret.operands.push_back(std::move(predicate));
        ret.operands.push_back(std::move(iterator));
        ret.data.emplace_back(std::move(body));
        return ret;
    }

    ShaderInstruction call(std::string name, std::vector<ShaderOperand> arguments) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::CallFunction;
        ret.operands = std::move(arguments);
        ret.data.emplace_back(std::move(name));
        return ret;
    }

    ShaderInstruction ret(ShaderOperand returnValue) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::Return;
        ret.operands.push_back(std::move(returnValue));
        return ret;
    }

    ShaderInstruction emitVertex() {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::EmitVertex;
        return ret;
    }

    ShaderInstruction endPrimitive() {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::EndPrimitive;
        return ret;
    }

    ShaderInstruction setFragmentDepth(ShaderOperand depth) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::SetFragmentDepth;
        ret.operands.push_back(std::move(depth));
        return ret;
    }

    ShaderInstruction setLayer(ShaderOperand layer) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::SetLayer;
        ret.operands.push_back(std::move(layer));
        return ret;
    }

    ShaderInstruction setVertexPosition(ShaderOperand position) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::SetVertexPosition;
        ret.operands.push_back(std::move(position));
        return ret;
    }

    ShaderInstruction vectorSwizzle(ShaderOperand vector,
                                    const std::vector<ShaderPrimitiveType::VectorComponent> &components) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::VectorSwizzle;
        ret.operands.push_back(std::move(vector));
        for (const auto &component: components) {
            ret.data.emplace_back(component);
        }
        return ret;
    }

    ShaderInstruction arraySubscript(ShaderOperand array, ShaderOperand index) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::ArraySubscript;
        ret.operands.push_back(std::move(array));
        ret.operands.push_back(std::move(index));
        return ret;
    }

    ShaderInstruction matrixSubscript(ShaderOperand matrix, ShaderOperand column, ShaderOperand row) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::MatrixSubscript;
        ret.operands.push_back(std::move(matrix));
        ret.operands.push_back(std::move(column));
        ret.operands.push_back(std::move(row));
        return ret;
    }

    ShaderInstruction objectMember(ShaderOperand object, std::string member) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::ObjectMember;
        ret.operands.push_back(std::move(object));
        ret.data.emplace_back(std::move(member));
        return ret;
    }

    ShaderInstruction createArray(const ShaderDataType &arrayType, std::vector<ShaderOperand> elements) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::CreateArray;
        ret.operands.insert(ret.operands.end(), elements.begin(), elements.end());
        ret.data.emplace_back(arrayType);
        return ret;
    }

    ShaderInstruction createMatrix(ShaderPrimitiveType elementType,
                                   ShaderOperand x,
                                   ShaderOperand y,
                                   ShaderOperand z,
                                   ShaderOperand w) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::CreateMatrix;
        ret.operands.push_back(std::move(x));
        ret.operands.push_back(std::move(y));
        ret.operands.push_back(std::move(z));
        ret.operands.push_back(std::move(w));
        ret.data.emplace_back(elementType);
        return ret;
    }

    ShaderInstruction createVector(ShaderPrimitiveType elementType,
                                   ShaderOperand x,
                                   ShaderOperand y,
                                   ShaderOperand z,
                                   ShaderOperand w) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::CreateVector;
        ret.operands.push_back(std::move(x));
        ret.operands.push_back(std::move(y));
        ret.operands.push_back(std::move(z));
        ret.operands.push_back(std::move(w));
        ret.data.emplace_back(elementType);
        return ret;
    }

    ShaderInstruction createStruct(ShaderStructType typeName, std::vector<ShaderOperand> values) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::CreateStruct;
        ret.operands = std::move(values);
        ret.data.emplace_back(std::move(typeName));
        return ret;
    }

    ShaderInstruction textureFetch(ShaderOperand texture, ShaderOperand coordinate, ShaderOperand index) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::TextureFetch;
        ret.operands.push_back(std::move(texture));
        ret.operands.push_back(std::move(coordinate));
        ret.operands.push_back(std::move(index));
        return ret;
    }

    ShaderInstruction textureFetchArray(ShaderOperand texture, ShaderOperand coordinate, ShaderOperand index) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::TextureFetchArray;
        ret.operands.push_back(std::move(texture));
        ret.operands.push_back(std::move(coordinate));
        ret.operands.push_back(std::move(index));
        return ret;
    }

    ShaderInstruction textureFetchMS(ShaderOperand texture, ShaderOperand coordinate, ShaderOperand index) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::TextureFetchMS;
        ret.operands.push_back(std::move(texture));
        ret.operands.push_back(std::move(coordinate));
        ret.operands.push_back(std::move(index));
        return ret;
    }

    ShaderInstruction textureFetchMSArray(ShaderOperand texture, ShaderOperand coordinate, ShaderOperand index) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::TextureFetchMSArray;
        ret.operands.push_back(std::move(texture));
        ret.operands.push_back(std::move(coordinate));
        ret.operands.push_back(std::move(index));
        return ret;
    }

    ShaderInstruction textureSample(ShaderOperand texture, ShaderOperand coordinate, ShaderOperand bias) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::TextureSample;
        ret.operands.push_back(std::move(texture));
        ret.operands.push_back(std::move(coordinate));
        ret.operands.push_back(std::move(bias));
        return ret;
    }

    ShaderInstruction textureSampleArray(ShaderOperand texture, ShaderOperand coordinate, ShaderOperand bias) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::TextureSampleArray;
        ret.operands.push_back(std::move(texture));
        ret.operands.push_back(std::move(coordinate));
        ret.operands.push_back(std::move(bias));
        return ret;
    }

    ShaderInstruction textureSampleCubeMap(ShaderOperand texture, ShaderOperand coordinate, ShaderOperand bias) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::TextureSampleCubeMap;
        ret.operands.push_back(std::move(texture));
        ret.operands.push_back(std::move(coordinate));
        ret.operands.push_back(std::move(bias));
        return ret;
    }

    ShaderInstruction textureSampleCubeMapArray(ShaderOperand texture, ShaderOperand coordinate, ShaderOperand bias) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::TextureSampleCubeMapArray;
        ret.operands.push_back(std::move(texture));
        ret.operands.push_back(std::move(coordinate));
        ret.operands.push_back(std::move(bias));
        return ret;
    }

    ShaderInstruction textureSize(ShaderOperand texture, ShaderOperand lod) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::TextureSize;
        ret.operands.push_back(std::move(texture));
        ret.operands.push_back(std::move(lod));
        return ret;
    }

    ShaderInstruction bufferSize(ShaderOperand buffer) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::BufferSize;
        ret.operands.push_back(std::move(buffer));
        return ret;
    }

    ShaderInstruction add(ShaderOperand left, ShaderOperand right) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::Add;
        ret.operands.push_back(std::move(left));
        ret.operands.push_back(std::move(right));
        return ret;
    }

    ShaderInstruction subtract(ShaderOperand left, ShaderOperand right) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::Subtract;
        ret.operands.push_back(std::move(left));
        ret.operands.push_back(std::move(right));
        return ret;
    }

    ShaderInstruction multiply(ShaderOperand left, ShaderOperand right) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::Multiply;
        ret.operands.push_back(std::move(left));
        ret.operands.push_back(std::move(right));
        return ret;
    }

    ShaderInstruction divide(ShaderOperand left, ShaderOperand right) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::Divide;
        ret.operands.push_back(std::move(left));
        ret.operands.push_back(std::move(right));
        return ret;
    }

    ShaderInstruction logicalAnd(ShaderOperand left, ShaderOperand right) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::LogicalAnd;
        ret.operands.push_back(std::move(left));
        ret.operands.push_back(std::move(right));
        return ret;
    }

    ShaderInstruction logicalOr(ShaderOperand left, ShaderOperand right) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::LogicalOr;
        ret.operands.push_back(std::move(left));
        ret.operands.push_back(std::move(right));
        return ret;
    }

    ShaderInstruction compareGreaterEqual(ShaderOperand left, ShaderOperand right) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::GreaterEqual;
        ret.operands.push_back(std::move(left));
        ret.operands.push_back(std::move(right));
        return ret;
    }

    ShaderInstruction compareGreater(ShaderOperand left, ShaderOperand right) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::Greater;
        ret.operands.push_back(std::move(left));
        ret.operands.push_back(std::move(right));
        return ret;
    }

    ShaderInstruction compareLessEqual(ShaderOperand left, ShaderOperand right) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::LessEqual;
        ret.operands.push_back(std::move(left));
        ret.operands.push_back(std::move(right));
        return ret;
    }

    ShaderInstruction compareLess(ShaderOperand left, ShaderOperand right) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::Less;
        ret.operands.push_back(std::move(left));
        ret.operands.push_back(std::move(right));
        return ret;
    }

    ShaderInstruction compareEqual(ShaderOperand left, ShaderOperand right) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::Equal;
        ret.operands.push_back(std::move(left));
        ret.operands.push_back(std::move(right));
        return ret;
    }

    ShaderInstruction compareNotEqual(ShaderOperand left, ShaderOperand right) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::NotEqual;
        ret.operands.push_back(std::move(left));
        ret.operands.push_back(std::move(right));
        return ret;
    }

    ShaderInstruction abs(ShaderOperand value) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::Abs;
        ret.operands.push_back(std::move(value));
        return ret;
    }

    ShaderInstruction sin(ShaderOperand value) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::Sin;
        ret.operands.push_back(std::move(value));
        return ret;
    }

    ShaderInstruction cos(ShaderOperand value) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::Cos;
        ret.operands.push_back(std::move(value));
        return ret;
    }

    ShaderInstruction tan(ShaderOperand value) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::Tan;
        ret.operands.push_back(std::move(value));
        return ret;
    }

    ShaderInstruction asin(ShaderOperand value) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::Asin;
        ret.operands.push_back(std::move(value));
        return ret;
    }

    ShaderInstruction acos(ShaderOperand value) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::Acos;
        ret.operands.push_back(std::move(value));
        return ret;
    }

    ShaderInstruction atan(ShaderOperand value) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::Atan;
        ret.operands.push_back(std::move(value));
        return ret;
    }

    ShaderInstruction pow(ShaderOperand value, ShaderOperand exponent) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::Pow;
        ret.operands.push_back(std::move(value));
        ret.operands.push_back(std::move(exponent));
        return ret;
    }

    ShaderInstruction exp(ShaderOperand value) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::Exp;
        ret.operands.push_back(std::move(value));
        return ret;
    }

    ShaderInstruction log(ShaderOperand value) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::Log;
        ret.operands.push_back(std::move(value));
        return ret;
    }

    ShaderInstruction sqrt(ShaderOperand value) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::Sqrt;
        ret.operands.push_back(std::move(value));
        return ret;
    }

    ShaderInstruction inverseSqrt(ShaderOperand value) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::InverseSqrt;
        ret.operands.push_back(std::move(value));
        return ret;
    }

    ShaderInstruction floor(ShaderOperand value) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::Floor;
        ret.operands.push_back(std::move(value));
        return ret;
    }

    ShaderInstruction ceil(ShaderOperand value) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::Ceil;
        ret.operands.push_back(std::move(value));
        return ret;
    }

    ShaderInstruction round(ShaderOperand value) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::Round;
        ret.operands.push_back(std::move(value));
        return ret;
    }

    ShaderInstruction fract(ShaderOperand value) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::Fract;
        ret.operands.push_back(std::move(value));
        return ret;
    }

    ShaderInstruction mod(ShaderOperand value, ShaderOperand modulus) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::Mod;
        ret.operands.push_back(std::move(value));
        ret.operands.push_back(std::move(modulus));
        return ret;
    }

    ShaderInstruction min(ShaderOperand x, ShaderOperand y) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::Min;
        ret.operands.push_back(std::move(x));
        ret.operands.push_back(std::move(y));
        return ret;
    }

    ShaderInstruction max(ShaderOperand x, ShaderOperand y) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::Max;
        ret.operands.push_back(std::move(x));
        ret.operands.push_back(std::move(y));
        return ret;
    }

    ShaderInstruction clamp(ShaderOperand value, ShaderOperand min, ShaderOperand max) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::Clamp;
        ret.operands.push_back(std::move(value));
        ret.operands.push_back(std::move(min));
        ret.operands.push_back(std::move(max));
        return ret;
    }

    ShaderInstruction mix(ShaderOperand x, ShaderOperand y, ShaderOperand a) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::Mix;
        ret.operands.push_back(std::move(x));
        ret.operands.push_back(std::move(y));
        ret.operands.push_back(std::move(a));
        return ret;
    }

    ShaderInstruction step(ShaderOperand edge, ShaderOperand x) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::Step;
        ret.operands.push_back(std::move(edge));
        ret.operands.push_back(std::move(x));
        return ret;
    }

    ShaderInstruction smoothstep(ShaderOperand edge0, ShaderOperand edge1, ShaderOperand x) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::SmoothStep;
        ret.operands.push_back(std::move(edge0));
        ret.operands.push_back(std::move(edge1));
        ret.operands.push_back(std::move(x));
        return ret;
    }

    ShaderInstruction dot(ShaderOperand x, ShaderOperand y) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::Dot;
        ret.operands.push_back(std::move(x));
        ret.operands.push_back(std::move(y));
        return ret;
    }

    ShaderInstruction cross(ShaderOperand x, ShaderOperand y) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::Cross;
        ret.operands.push_back(std::move(x));
        ret.operands.push_back(std::move(y));
        return ret;
    }

    ShaderInstruction normalize(ShaderOperand x) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::Normalize;
        ret.operands.push_back(std::move(x));
        return ret;
    }

    ShaderInstruction length(ShaderOperand x) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::Length;
        ret.operands.push_back(std::move(x));
        return ret;
    }

    ShaderInstruction distance(ShaderOperand x, ShaderOperand y) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::Distance;
        ret.operands.push_back(std::move(x));
        ret.operands.push_back(std::move(y));
        return ret;
    }

    ShaderInstruction reflect(ShaderOperand I, ShaderOperand N) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::Reflect;
        ret.operands.push_back(std::move(I));
        ret.operands.push_back(std::move(N));
        return ret;
    }

    ShaderInstruction refract(ShaderOperand I, ShaderOperand N, ShaderOperand eta) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::Refract;
        ret.operands.push_back(std::move(I));
        ret.operands.push_back(std::move(N));
        ret.operands.push_back(std::move(eta));
        return ret;
    }

    ShaderInstruction faceForward(ShaderOperand N, ShaderOperand I, ShaderOperand Nref) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::FaceForward;
        ret.operands.push_back(std::move(N));
        ret.operands.push_back(std::move(I));
        ret.operands.push_back(std::move(Nref));
        return ret;
    }

    ShaderInstruction transpose(ShaderOperand x) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::Transpose;
        ret.operands.push_back(std::move(x));
        return ret;
    }

    ShaderInstruction inverse(ShaderOperand x) {
        ShaderInstruction ret;
        ret.code = ShaderInstruction::Inverse;
        ret.operands.push_back(std::move(x));
        return ret;
    }
}
