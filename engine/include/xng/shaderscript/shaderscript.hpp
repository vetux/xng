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

#ifndef XENGINE_SHADERSCRIPT_HPP
#define XENGINE_SHADERSCRIPT_HPP

#include "xng/shaderscript/shaderobject.hpp"
#include "xng/shaderscript/shaderscope.hpp"
#include "xng/shaderscript/functionscope.hpp"
#include "xng/shaderscript/branchbuilder.hpp"
#include "xng/shaderscript/loopbuilder.hpp"

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
        return ShaderObject(lhs) && rhs;
    }

    template<rg::ShaderPrimitiveType::Type TYPE, rg::ShaderPrimitiveType::Component COMPONENT, int C>
    using Array = ShaderDataObject<TYPE, COMPONENT, C>;

    template<int C>
    using ArrayBool = Array<rg::ShaderPrimitiveType::SCALAR, rg::ShaderPrimitiveType::BOOLEAN, C>;

    template<int C>
    using ArrayInt = Array<rg::ShaderPrimitiveType::SCALAR, rg::ShaderPrimitiveType::SIGNED_INT, C>;

    template<int C>
    using ArrayUInt = Array<rg::ShaderPrimitiveType::SCALAR, rg::ShaderPrimitiveType::UNSIGNED_INT, C>;

    template<int C>
    using ArrayFloat = Array<rg::ShaderPrimitiveType::SCALAR, rg::ShaderPrimitiveType::FLOAT, C>;

    template<int C>
    using ArrayDouble = Array<rg::ShaderPrimitiveType::SCALAR, rg::ShaderPrimitiveType::DOUBLE, C>;

    template<int C>
    using ArrayBVec2 = Array<rg::ShaderPrimitiveType::VECTOR2, rg::ShaderPrimitiveType::BOOLEAN, C>;

    template<int C>
    using ArrayBVec3 = Array<rg::ShaderPrimitiveType::VECTOR3, rg::ShaderPrimitiveType::BOOLEAN, C>;

    template<int C>
    using ArrayBVec4 = Array<rg::ShaderPrimitiveType::VECTOR4, rg::ShaderPrimitiveType::BOOLEAN, C>;

    template<int C>
    using ArrayIVec2 = Array<rg::ShaderPrimitiveType::VECTOR2, rg::ShaderPrimitiveType::SIGNED_INT, C>;

    template<int C>
    using ArrayIVec3 = Array<rg::ShaderPrimitiveType::VECTOR3, rg::ShaderPrimitiveType::SIGNED_INT, C>;

    template<int C>
    using ArrayIVec4 = Array<rg::ShaderPrimitiveType::VECTOR4, rg::ShaderPrimitiveType::SIGNED_INT, C>;

    template<int C>
    using ArrayUVec2 = Array<rg::ShaderPrimitiveType::VECTOR2, rg::ShaderPrimitiveType::UNSIGNED_INT, C>;

    template<int C>
    using ArrayUVec3 = Array<rg::ShaderPrimitiveType::VECTOR3, rg::ShaderPrimitiveType::UNSIGNED_INT, C>;

    template<int C>
    using ArrayUVec4 = Array<rg::ShaderPrimitiveType::VECTOR4, rg::ShaderPrimitiveType::UNSIGNED_INT, C>;

    template<int C>
    using ArrayVec2 = Array<rg::ShaderPrimitiveType::VECTOR2, rg::ShaderPrimitiveType::FLOAT, C>;

    template<int C>
    using ArrayVec3 = Array<rg::ShaderPrimitiveType::VECTOR3, rg::ShaderPrimitiveType::FLOAT, C>;

    template<int C>
    using ArrayVec4 = Array<rg::ShaderPrimitiveType::VECTOR4, rg::ShaderPrimitiveType::FLOAT, C>;

    template<int C>
    using ArrayMat2 = Array<rg::ShaderPrimitiveType::MAT2, rg::ShaderPrimitiveType::FLOAT, C>;

    template<int C>
    using ArrayMat3 = Array<rg::ShaderPrimitiveType::MAT3, rg::ShaderPrimitiveType::FLOAT, C>;

    template<int C>
    using ArrayMat4 = Array<rg::ShaderPrimitiveType::MAT4, rg::ShaderPrimitiveType::FLOAT, C>;

    template<int C>
    using ArrayDVec2 = Array<rg::ShaderPrimitiveType::VECTOR2, rg::ShaderPrimitiveType::DOUBLE, C>;

    template<int C>
    using ArrayDVec3 = Array<rg::ShaderPrimitiveType::VECTOR3, rg::ShaderPrimitiveType::DOUBLE, C>;

    template<int C>
    using ArrayDVec4 = Array<rg::ShaderPrimitiveType::VECTOR4, rg::ShaderPrimitiveType::DOUBLE, C>;

    template<int C>
    using ArrayDMat2 = Array<rg::ShaderPrimitiveType::MAT2, rg::ShaderPrimitiveType::DOUBLE, C>;

    template<int C>
    using ArrayDMat3 = Array<rg::ShaderPrimitiveType::MAT3, rg::ShaderPrimitiveType::DOUBLE, C>;

    template<int C>
    using ArrayDMat4 = Array<rg::ShaderPrimitiveType::MAT4, rg::ShaderPrimitiveType::DOUBLE, C>;

    template<const char * typeName>
    using Object = ShaderStructObject<typeName, 1>;

    // Texture definitions only needed for function arguments.
    template<rg::ColorFormat C>
    using Texture2D = ShaderTextureObject<rg::TEXTURE_2D, C>;

    template<rg::ColorFormat C>
    using Texture2DMS = ShaderTextureObject<rg::TEXTURE_2D_MULTISAMPLE, C>;

    template<rg::ColorFormat C>
    using TextureCube = ShaderTextureObject<rg::TEXTURE_CUBE_MAP, C>;

    template<rg::ColorFormat C>
    using Texture2DArray = ShaderTextureObject<rg::TEXTURE_2D_ARRAY, C>;

    template<rg::ColorFormat C>
    using Texture2DMSArray = ShaderTextureObject<rg::TEXTURE_2D_MULTISAMPLE_ARRAY, C>;

    template<rg::ColorFormat C>
    using TextureCubeArray = ShaderTextureObject<rg::TEXTURE_CUBE_MAP_ARRAY, C>;

    typedef ShaderDataObject<rg::ShaderPrimitiveType::SCALAR, rg::ShaderPrimitiveType::BOOLEAN, 1> Bool;

    typedef ShaderDataObject<rg::ShaderPrimitiveType::SCALAR, rg::ShaderPrimitiveType::SIGNED_INT, 1> Int;

    typedef ShaderDataObject<rg::ShaderPrimitiveType::SCALAR, rg::ShaderPrimitiveType::UNSIGNED_INT, 1> UInt;

    typedef ShaderDataObject<rg::ShaderPrimitiveType::SCALAR, rg::ShaderPrimitiveType::FLOAT, 1> Float;

    typedef ShaderDataObject<rg::ShaderPrimitiveType::SCALAR, rg::ShaderPrimitiveType::DOUBLE, 1> Double;

    typedef ShaderDataObject<rg::ShaderPrimitiveType::VECTOR2, rg::ShaderPrimitiveType::FLOAT, 1> vec2;

    typedef ShaderDataObject<rg::ShaderPrimitiveType::VECTOR3, rg::ShaderPrimitiveType::FLOAT, 1> vec3;

    typedef ShaderDataObject<rg::ShaderPrimitiveType::VECTOR4, rg::ShaderPrimitiveType::FLOAT, 1> vec4;

    typedef ShaderDataObject<rg::ShaderPrimitiveType::MAT2, rg::ShaderPrimitiveType::FLOAT, 1> mat2;

    typedef ShaderDataObject<rg::ShaderPrimitiveType::MAT3, rg::ShaderPrimitiveType::FLOAT, 1> mat3;

    typedef ShaderDataObject<rg::ShaderPrimitiveType::MAT4, rg::ShaderPrimitiveType::FLOAT, 1> mat4;

    typedef ShaderDataObject<rg::ShaderPrimitiveType::VECTOR2, rg::ShaderPrimitiveType::DOUBLE, 1> dvec2;

    typedef ShaderDataObject<rg::ShaderPrimitiveType::VECTOR3, rg::ShaderPrimitiveType::DOUBLE, 1> dvec3;

    typedef ShaderDataObject<rg::ShaderPrimitiveType::VECTOR4, rg::ShaderPrimitiveType::DOUBLE, 1> dvec4;

    typedef ShaderDataObject<rg::ShaderPrimitiveType::MAT2, rg::ShaderPrimitiveType::DOUBLE, 1> dmat2;

    typedef ShaderDataObject<rg::ShaderPrimitiveType::MAT3, rg::ShaderPrimitiveType::DOUBLE, 1> dmat3;

    typedef ShaderDataObject<rg::ShaderPrimitiveType::MAT4, rg::ShaderPrimitiveType::DOUBLE, 1> dmat4;

    typedef ShaderDataObject<rg::ShaderPrimitiveType::VECTOR2, rg::ShaderPrimitiveType::BOOLEAN, 1> bvec2;

    typedef ShaderDataObject<rg::ShaderPrimitiveType::VECTOR3, rg::ShaderPrimitiveType::BOOLEAN, 1> bvec3;

    typedef ShaderDataObject<rg::ShaderPrimitiveType::VECTOR4, rg::ShaderPrimitiveType::BOOLEAN, 1> bvec4;

    typedef ShaderDataObject<rg::ShaderPrimitiveType::VECTOR2, rg::ShaderPrimitiveType::SIGNED_INT, 1> ivec2;

    typedef ShaderDataObject<rg::ShaderPrimitiveType::VECTOR3, rg::ShaderPrimitiveType::SIGNED_INT, 1> ivec3;

    typedef ShaderDataObject<rg::ShaderPrimitiveType::VECTOR4, rg::ShaderPrimitiveType::SIGNED_INT, 1> ivec4;

    typedef ShaderDataObject<rg::ShaderPrimitiveType::VECTOR2, rg::ShaderPrimitiveType::UNSIGNED_INT, 1> uvec2;

    typedef ShaderDataObject<rg::ShaderPrimitiveType::VECTOR3, rg::ShaderPrimitiveType::UNSIGNED_INT, 1> uvec3;

    typedef ShaderDataObject<rg::ShaderPrimitiveType::VECTOR4, rg::ShaderPrimitiveType::UNSIGNED_INT, 1> uvec4;

    inline ShaderObject inputAttribute(const std::string &attributeName) {
        return ShaderObject(rg::ShaderOperand(rg::ShaderOperand::InputAttribute, attributeName));
    }

    inline ShaderObject outputAttribute(const std::string &attributeName) {
        return ShaderObject(rg::ShaderOperand(rg::ShaderOperand::OutputAttribute, attributeName));
    }

    inline ShaderObject parameter(const std::string &name) {
        return ShaderObject(rg::ShaderOperand(rg::ShaderOperand::Parameter, name));
    }

    inline ShaderObject buffer(const std::string &name) {
        return ShaderObject(rg::ShaderOperand(rg::ShaderOperand::Buffer, name));
    }

    inline ShaderObject textureSampler(const std::string &name) {
        return ShaderObject(rg::ShaderOperand(rg::ShaderOperand::Texture, name));
    }

    inline ShaderObject argument(const std::string &name) {
        return ShaderObject(rg::ShaderOperand(rg::ShaderOperand::Argument, name));
    }

    inline ShaderObject getVertexID() {
        return ShaderObject(rg::ShaderOperand(rg::ShaderOperand::Instruction,
                                              rg::ShaderInstructionFactory::getVertexID()));
    }

    inline ShaderObject getInstanceID() {
        return ShaderObject(rg::ShaderOperand(rg::ShaderOperand::Instruction,
                                              rg::ShaderInstructionFactory::getInstanceID()));
    }

    inline ShaderObject getDrawID() {
        return ShaderObject(rg::ShaderOperand(rg::ShaderOperand::Instruction,
                                              rg::ShaderInstructionFactory::getDrawID()));
    }

    inline ShaderObject getBaseVertex() {
        return ShaderObject(rg::ShaderOperand(rg::ShaderOperand::Instruction,
                                              rg::ShaderInstructionFactory::getBaseVertex()));
    }

    inline ShaderObject getBaseInstance() {
        return ShaderObject(rg::ShaderOperand(rg::ShaderOperand::Instruction,
                                              rg::ShaderInstructionFactory::getBaseInstance()));
    }

    inline ShaderObject getNumberOfWorkGroups() {
        return ShaderObject(rg::ShaderOperand(rg::ShaderOperand::Instruction,
                                              rg::ShaderInstructionFactory::getNumberOfWorkGroups()));
    }

    inline ShaderObject getWorkGroupID() {
        return ShaderObject(rg::ShaderOperand(rg::ShaderOperand::Instruction,
                                              rg::ShaderInstructionFactory::getWorkGroupID()));
    }

    inline ShaderObject getLocalInvocationID() {
        return ShaderObject(rg::ShaderOperand(rg::ShaderOperand::Instruction,
                                              rg::ShaderInstructionFactory::getLocalInvocationID()));
    }

    inline ShaderObject getGlobalInvocationID() {
        return ShaderObject(rg::ShaderOperand(rg::ShaderOperand::Instruction,
                                              rg::ShaderInstructionFactory::getGlobalInvocationID()));
    }

    inline void setVertexPosition(const ShaderObject &value) {
        BlockScope::get().addInstruction(rg::ShaderInstructionFactory::setVertexPosition(value.operand));
    }

    inline void setFragmentDepth(const ShaderObject &value) {
        BlockScope::get().addInstruction((rg::ShaderInstructionFactory::setFragmentDepth(value.operand)));
    }

    inline void setLayer(const ShaderObject &value) {
        BlockScope::get().addInstruction((rg::ShaderInstructionFactory::setLayer(value.operand)));
    }

    inline void EmitVertex() {
        BlockScope::get().addInstruction(rg::ShaderInstructionFactory::emitVertex());
    }

    inline void EndPrimitive() {
        BlockScope::get().addInstruction(rg::ShaderInstructionFactory::endPrimitive());
    }

    inline ShaderObject Call(const std::string &functionName,
                             const std::vector<ShaderObject> &wArgs = {}) {
        std::vector<rg::ShaderOperand> args;
        for (auto &arg: wArgs) {
            args.push_back(arg.operand);
        }
        return ShaderObject(rg::ShaderInstructionFactory::call(functionName, args));
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
        BlockScope::get().addInstruction(rg::ShaderInstructionFactory::ret(value.operand));
    }

    inline void Return() {
        BlockScope::get().addInstruction(rg::ShaderInstructionFactory::ret());
    }

    inline ShaderObject textureSize(const ShaderObject &texture) {
        return ShaderObject(rg::ShaderInstructionFactory::textureSize(texture.operand));
    }

    inline ShaderObject textureSize(const ShaderObject &texture, const ShaderObject &lod) {
        return ShaderObject(rg::ShaderInstructionFactory::textureSize(texture.operand, lod.operand));
    }

    inline ShaderObject textureSample(const ShaderObject &texture, const ShaderObject &coords) {
        return ShaderObject(rg::ShaderInstructionFactory::textureSample(texture.operand, coords.operand));
    }

    inline ShaderObject textureSample(const ShaderObject &texture,
                                      const ShaderObject &coords,
                                      const ShaderObject &bias) {
        return ShaderObject(rg::ShaderInstructionFactory::textureSample(texture.operand,
                                                                        coords.operand,
                                                                        bias.operand));
    }

    inline ShaderObject textureSampleArray(const ShaderObject &texture, const ShaderObject &coords) {
        return ShaderObject(rg::ShaderInstructionFactory::textureSampleArray(texture.operand, coords.operand));
    }

    inline ShaderObject textureSampleArray(const ShaderObject &texture,
                                           const ShaderObject &coords,
                                           const ShaderObject &bias) {
        return ShaderObject(rg::ShaderInstructionFactory::textureSampleArray(texture.operand,
                                                                             coords.operand,
                                                                             bias.operand));
    }

    inline ShaderObject textureSampleLod(const ShaderObject &texture,
                                         const ShaderObject &coords,
                                         const ShaderObject &lod) {
        return ShaderObject(rg::ShaderInstructionFactory::textureSampleLod(texture.operand,
                                                                           coords.operand,
                                                                           lod.operand));
    }

    inline ShaderObject textureSampleArrayLod(const ShaderObject &texture,
                                              const ShaderObject &coords,
                                              const ShaderObject &lod) {
        return ShaderObject(rg::ShaderInstructionFactory::textureSampleArrayLod(texture.operand,
            coords.operand,
            lod.operand));
    }

    inline ShaderObject texelFetch(const ShaderObject &texture,
                                   const ShaderObject &coords,
                                   const ShaderObject &lod) {
        return ShaderObject(rg::ShaderInstructionFactory::textureFetch(texture.operand,
                                                                       coords.operand,
                                                                       lod.operand));
    }

    inline ShaderObject texelFetchArray(const ShaderObject &texture,
                                        const ShaderObject &coords,
                                        const ShaderObject &lod) {
        return ShaderObject(rg::ShaderInstructionFactory::textureFetchArray(texture.operand,
                                                                            coords.operand,
                                                                            lod.operand));
    }

    inline ShaderObject texelFetchMS(const ShaderObject &texture,
                                     const ShaderObject &coords,
                                     const ShaderObject &index) {
        return ShaderObject(rg::ShaderInstructionFactory::textureFetchMS(texture.operand,
                                                                         coords.operand,
                                                                         index.operand));
    }

    inline ShaderObject texelFetchMSArray(const ShaderObject &texture,
                                          const ShaderObject &coords,
                                          const ShaderObject &index) {
        return ShaderObject(rg::ShaderInstructionFactory::textureFetchMSArray(texture.operand,
                                                                              coords.operand,
                                                                              index.operand));
    }

    inline ShaderObject textureSampleCube(const ShaderObject &texture,
                                          const ShaderObject &coords) {
        return ShaderObject(rg::ShaderInstructionFactory::textureSampleCubeMap(texture.operand, coords.operand));
    }

    inline ShaderObject textureSampleCube(const ShaderObject &texture,
                                          const ShaderObject &coords,
                                          const ShaderObject &bias) {
        return ShaderObject(rg::ShaderInstructionFactory::textureSampleCubeMap(texture.operand,
                                                                               coords.operand,
                                                                               bias.operand));
    }

    inline ShaderObject textureSampleCubeArray(const ShaderObject &texture,
                                               const ShaderObject &coords) {
        return ShaderObject(rg::ShaderInstructionFactory::textureSampleCubeMapArray(texture.operand, coords.operand));
    }

    inline ShaderObject textureSampleCubeArray(const ShaderObject &texture,
                                               const ShaderObject &coords,
                                               const ShaderObject &bias) {
        return ShaderObject(rg::ShaderInstructionFactory::textureSampleCubeMapArray(texture.operand,
            coords.operand,
            bias.operand));
    }

    inline ShaderObject textureGrad(const ShaderObject &texture,
                                    const ShaderObject &coords,
                                    const ShaderObject &dPdx,
                                    const ShaderObject &dPdy) {
        return ShaderObject(rg::ShaderInstructionFactory::textureGrad(texture.operand,
                                                                      coords.operand,
                                                                      dPdx.operand,
                                                                      dPdy.operand));
    }

    inline ShaderObject textureGradArray(const ShaderObject &texture,
                                         const ShaderObject &coords,
                                         const ShaderObject &dPdx,
                                         const ShaderObject &dPdy) {
        return ShaderObject(rg::ShaderInstructionFactory::textureGradArray(texture.operand,
                                                                           coords.operand,
                                                                           dPdx.operand,
                                                                           dPdy.operand));
    }

    inline ShaderObject abs(const ShaderObject &value) {
        return ShaderObject(rg::ShaderInstructionFactory::abs(value.operand));
    }

    inline ShaderObject sin(const ShaderObject &value) {
        return ShaderObject(rg::ShaderInstructionFactory::sin(value.operand));
    }

    inline ShaderObject cos(const ShaderObject &value) {
        return ShaderObject(rg::ShaderInstructionFactory::cos(value.operand));
    }

    inline ShaderObject tan(const ShaderObject &value) {
        return ShaderObject(rg::ShaderInstructionFactory::tan(value.operand));
    }

    inline ShaderObject asin(const ShaderObject &value) {
        return ShaderObject(rg::ShaderInstructionFactory::asin(value.operand));
    }

    inline ShaderObject acos(const ShaderObject &value) {
        return ShaderObject(rg::ShaderInstructionFactory::acos(value.operand));
    }

    inline ShaderObject atan(const ShaderObject &value) {
        return ShaderObject(rg::ShaderInstructionFactory::atan(value.operand));
    }

    inline ShaderObject atan2(const ShaderObject &y, const ShaderObject &x) {
        return ShaderObject(rg::ShaderInstructionFactory::atan2(y.operand, x.operand));
    }

    inline ShaderObject pow(const ShaderObject &value, const ShaderObject &exponent) {
        return ShaderObject(rg::ShaderInstructionFactory::pow(value.operand, exponent.operand));
    }

    inline ShaderObject exp(const ShaderObject &value) {
        return ShaderObject(rg::ShaderInstructionFactory::exp(value.operand));
    }

    inline ShaderObject exp2(const ShaderObject &value) {
        return ShaderObject(rg::ShaderInstructionFactory::exp2(value.operand));
    }

    inline ShaderObject log(const ShaderObject &value) {
        return ShaderObject(rg::ShaderInstructionFactory::log(value.operand));
    }

    inline ShaderObject log2(const ShaderObject &value) {
        return ShaderObject(rg::ShaderInstructionFactory::log2(value.operand));
    }

    inline ShaderObject sqrt(const ShaderObject &value) {
        return ShaderObject(rg::ShaderInstructionFactory::sqrt(value.operand));
    }

    inline ShaderObject inverseSqrt(const ShaderObject &value) {
        return ShaderObject(rg::ShaderInstructionFactory::inverseSqrt(value.operand));
    }

    inline ShaderObject floor(const ShaderObject &value) {
        return ShaderObject(rg::ShaderInstructionFactory::floor(value.operand));
    }

    inline ShaderObject ceil(const ShaderObject &value) {
        return ShaderObject(rg::ShaderInstructionFactory::ceil(value.operand));
    }

    inline ShaderObject round(const ShaderObject &value) {
        return ShaderObject(rg::ShaderInstructionFactory::round(value.operand));
    }

    inline ShaderObject fract(const ShaderObject &value) {
        return ShaderObject(rg::ShaderInstructionFactory::fract(value.operand));
    }

    inline ShaderObject mod(const ShaderObject &value, const ShaderObject &modulus) {
        return ShaderObject(rg::ShaderInstructionFactory::mod(value.operand, modulus.operand));
    }

    inline ShaderObject min(const ShaderObject &x, const ShaderObject &y) {
        return ShaderObject(rg::ShaderInstructionFactory::min(x.operand, y.operand));
    }

    inline ShaderObject max(const ShaderObject &x, const ShaderObject &y) {
        return ShaderObject(rg::ShaderInstructionFactory::max(x.operand, y.operand));
    }

    inline ShaderObject clamp(const ShaderObject &value,
                              const ShaderObject &min,
                              const ShaderObject &max) {
        return ShaderObject(rg::ShaderInstructionFactory::clamp(value.operand, min.operand, max.operand));
    }

    inline ShaderObject mix(const ShaderObject &x,
                            const ShaderObject &y,
                            const ShaderObject &a) {
        return ShaderObject(rg::ShaderInstructionFactory::mix(x.operand, y.operand, a.operand));
    }

    inline ShaderObject step(const ShaderObject &edge, const ShaderObject &x) {
        return ShaderObject(rg::ShaderInstructionFactory::step(edge.operand, x.operand));
    }

    inline ShaderObject smoothstep(const ShaderObject &edge0,
                                   const ShaderObject &edge1,
                                   const ShaderObject &x) {
        return ShaderObject(rg::ShaderInstructionFactory::smoothstep(edge0.operand, edge1.operand, x.operand));
    }

    inline ShaderObject dot(const ShaderObject &x, const ShaderObject &y) {
        return ShaderObject(rg::ShaderInstructionFactory::dot(x.operand, y.operand));
    }

    inline ShaderObject cross(const ShaderObject &x, const ShaderObject &y) {
        return ShaderObject(rg::ShaderInstructionFactory::cross(x.operand, y.operand));
    }

    inline ShaderObject normalize(const ShaderObject &x) {
        return ShaderObject(rg::ShaderInstructionFactory::normalize(x.operand));
    }

    inline ShaderObject length(const ShaderObject &x) {
        return ShaderObject(rg::ShaderInstructionFactory::length(x.operand));
    }

    inline ShaderObject distance(const ShaderObject &x, const ShaderObject &y) {
        return ShaderObject(rg::ShaderInstructionFactory::distance(x.operand, y.operand));
    }

    inline ShaderObject reflect(const ShaderObject &I, const ShaderObject &N) {
        return ShaderObject(rg::ShaderInstructionFactory::reflect(I.operand, N.operand));
    }

    inline ShaderObject refract(const ShaderObject &I,
                                const ShaderObject &N,
                                const ShaderObject &eta) {
        return ShaderObject(rg::ShaderInstructionFactory::refract(I.operand, N.operand, eta.operand));
    }

    inline ShaderObject faceforward(const ShaderObject &N,
                                    const ShaderObject &I,
                                    const ShaderObject &Nref) {
        return ShaderObject(rg::ShaderInstructionFactory::faceForward(N.operand, I.operand, Nref.operand));
    }

    inline ShaderObject transpose(const ShaderObject &v) {
        return ShaderObject(rg::ShaderInstructionFactory::transpose(v.operand));
    }

    inline ShaderObject inverse(const ShaderObject &v) {
        return ShaderObject(rg::ShaderInstructionFactory::inverse(v.operand));
    }

    inline ShaderObject partialDerivativeX(const ShaderObject &value) {
        return ShaderObject(rg::ShaderInstructionFactory::partialDerivativeX(value.operand));
    }

    inline ShaderObject partialDerivativeY(const ShaderObject &value) {
        return ShaderObject(rg::ShaderInstructionFactory::partialDerivativeY(value.operand));
    }

    inline ShaderObject atomicAdd(const ShaderObject &mem, const ShaderObject &data) {
        return ShaderObject(rg::ShaderInstructionFactory::atomicAdd(mem.operand, data.operand));
    }

    inline ShaderObject atomicMin(const ShaderObject &mem, const ShaderObject &data) {
        return ShaderObject(rg::ShaderInstructionFactory::atomicMin(mem.operand, data.operand));
    }

    inline ShaderObject atomicMax(const ShaderObject &mem, const ShaderObject &data) {
        return ShaderObject(rg::ShaderInstructionFactory::atomicMax(mem.operand, data.operand));
    }

    inline ShaderObject atomicAnd(const ShaderObject &mem, const ShaderObject &data) {
        return ShaderObject(rg::ShaderInstructionFactory::atomicAnd(mem.operand, data.operand));
    }

    inline ShaderObject atomicOr(const ShaderObject &mem, const ShaderObject &data) {
        return ShaderObject(rg::ShaderInstructionFactory::atomicOr(mem.operand, data.operand));
    }

    inline ShaderObject atomicXor(const ShaderObject &mem, const ShaderObject &data) {
        return ShaderObject(rg::ShaderInstructionFactory::atomicXor(mem.operand, data.operand));
    }

    inline ShaderObject atomicExchange(const ShaderObject &mem, const ShaderObject &data) {
        return ShaderObject(rg::ShaderInstructionFactory::atomicExchange(mem.operand, data.operand));
    }

    inline ShaderObject atomicCompareSwap(const ShaderObject &mem,
                                          const ShaderObject &compare,
                                          const ShaderObject &data) {
        return ShaderObject(rg::ShaderInstructionFactory::atomicCompareSwap(mem.operand,
                                                                            compare.operand,
                                                                            data.operand));
    }

    template<typename T>
    struct DynamicBufferWrapper;

    template<typename T>
    struct DynamicBufferWrapper {
        ShaderObject buffer;

        explicit DynamicBufferWrapper(ShaderObject &&buffer) : buffer(buffer) {
            // By instantiating the struct type here, all type definitions are registered with the shader scope recursively.
            T def_type;
        }

        T operator[](const Int &index) {
            return buffer[index];
        }

        T operator[](const int index) {
            return buffer[Int(index)];
        }

        ShaderObject length() {
            return buffer.length();
        }
    };

    /**
     * Template specialization for ShaderDataObject.
     *
     * @tparam VALUE_TYPE
     * @tparam VALUE_COMPONENT
     * @tparam VALUE_COUNT
     */
    template<auto VALUE_TYPE, auto VALUE_COMPONENT, auto VALUE_COUNT>
    struct DynamicBufferWrapper<ShaderDataObject<VALUE_TYPE, VALUE_COMPONENT, VALUE_COUNT> > {
        ShaderObject buffer;

        explicit DynamicBufferWrapper(ShaderObject &&buffer) : buffer(buffer) {
        }

        ShaderObject operator[](const ShaderObject &index) {
            return buffer[index];
        }

        ShaderObject operator[](const unsigned int index) {
            return buffer[UInt(index)];
        }

        ShaderObject operator[](const int index) {
            return buffer[Int(index)];
        }

        ShaderObject length() {
            return buffer.length();
        }
    };
}

#endif //XENGINE_SHADERSCRIPT_HPP
