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

#ifndef XENGINE_SHADERSTRUCTDEF_HPP
#define XENGINE_SHADERSTRUCTDEF_HPP

#include "xng/rendergraph/shaderscript/shaderobject.hpp"
#include "xng/rendergraph/shader/shaderstruct.hpp"

namespace xng::ShaderScript {
    static ShaderStructElement _getElement(const std::string &typeName, const std::string &name) {
        if (typeName == "Bool") return {ShaderDataType::Bool(), name};
        if (typeName == "Int") return {ShaderDataType::Int(), name};
        if (typeName == "UInt") return {ShaderDataType::UInt(), name};
        if (typeName == "Float") return {ShaderDataType::Float(), name};
        if (typeName == "Double") return {ShaderDataType::Double(), name};
        if (typeName == "bvec2") return {ShaderDataType::bvec2(), name};
        if (typeName == "bvec3") return {ShaderDataType::bvec3(), name};
        if (typeName == "bvec4") return {ShaderDataType::bvec4(), name};
        if (typeName == "ivec2") return {ShaderDataType::ivec2(), name};
        if (typeName == "ivec3") return {ShaderDataType::ivec3(), name};
        if (typeName == "ivec4") return {ShaderDataType::ivec4(), name};
        if (typeName == "uvec2") return {ShaderDataType::uvec2(), name};
        if (typeName == "uvec3") return {ShaderDataType::uvec3(), name};
        if (typeName == "uvec4") return {ShaderDataType::uvec4(), name};
        if (typeName == "vec2") return {ShaderDataType::vec2(), name};
        if (typeName == "vec3") return {ShaderDataType::vec3(), name};
        if (typeName == "vec4") return {ShaderDataType::vec4(), name};
        if (typeName == "dvec2") return {ShaderDataType::dvec2(), name};
        if (typeName == "dvec3") return {ShaderDataType::dvec3(), name};
        if (typeName == "dvec4") return {ShaderDataType::dvec4(), name};
        if (typeName == "mat2") return {ShaderDataType::mat2(), name};
        if (typeName == "mat3") return {ShaderDataType::mat3(), name};
        if (typeName == "mat4") return {ShaderDataType::mat4(), name};
        if (typeName == "dmat2") return {ShaderDataType::dmat2(), name};
        if (typeName == "dmat3") return {ShaderDataType::dmat3(), name};
        if (typeName == "dmat4") return {ShaderDataType::dmat4(), name};
        auto it = typeName.rfind("Array");
        if (it == 0) {
            auto itStart = typeName.rfind('<');
            auto itEnd = typeName.rfind('>');
            if (itStart != std::string::npos && itEnd != std::string::npos && itStart + 1 < itEnd) {
                int count = std::stoi(typeName.substr(itStart + 1, itEnd - itStart - 1));
                std::string type = typeName.substr(5, itStart - 5);
                if (type == "Bool") return {ShaderDataType::array(ShaderDataType::Bool(), count), name};
                if (type == "Int") return {ShaderDataType::array(ShaderDataType::Int(), count), name};
                if (type == "UInt") return {ShaderDataType::array(ShaderDataType::UInt(), count), name};
                if (type == "Float") return {ShaderDataType::array(ShaderDataType::Float(), count), name};
                if (type == "Double") return {ShaderDataType::array(ShaderDataType::Double(), count), name};
                if (type == "BVec2") return {ShaderDataType::array(ShaderDataType::bvec2(), count), name};
                if (type == "BVec3") return {ShaderDataType::array(ShaderDataType::bvec3(), count), name};
                if (type == "BVec4") return {ShaderDataType::array(ShaderDataType::bvec4(), count), name};
                if (type == "IVec2") return {ShaderDataType::array(ShaderDataType::ivec2(), count), name};
                if (type == "IVec3") return {ShaderDataType::array(ShaderDataType::ivec3(), count), name};
                if (type == "IVec4") return {ShaderDataType::array(ShaderDataType::ivec4(), count), name};
                if (type == "UVec2") return {ShaderDataType::array(ShaderDataType::uvec2(), count), name};
                if (type == "UVec3") return {ShaderDataType::array(ShaderDataType::uvec3(), count), name};
                if (type == "UVec4") return {ShaderDataType::array(ShaderDataType::uvec4(), count), name};
                if (type == "Vec2") return {ShaderDataType::array(ShaderDataType::vec2(), count), name};
                if (type == "Vec3") return {ShaderDataType::array(ShaderDataType::vec3(), count), name};
                if (type == "Vec4") return {ShaderDataType::array(ShaderDataType::vec4(), count), name};
                if (type == "DVec2") return {ShaderDataType::array(ShaderDataType::dvec2(), count), name};
                if (type == "DVec3") return {ShaderDataType::array(ShaderDataType::dvec3(), count), name};
                if (type == "DVec4") return {ShaderDataType::array(ShaderDataType::dvec4(), count), name};
                if (type == "Mat2") return {ShaderDataType::array(ShaderDataType::mat2(), count), name};
                if (type == "Mat3") return {ShaderDataType::array(ShaderDataType::mat3(), count), name};
                if (type == "Mat4") return {ShaderDataType::array(ShaderDataType::mat4(), count), name};
                if (type == "DMat2") return {ShaderDataType::array(ShaderDataType::dmat2(), count), name};
                if (type == "DMat3") return {ShaderDataType::array(ShaderDataType::dmat3(), count), name};
                if (type == "DMat4") return {ShaderDataType::array(ShaderDataType::dmat4(), count), name};
            }
        }
        return {typeName, name};
    }
}

#define GetGenerator(_type0, _name0, \
    _type1, _name1, \
    _type2, _name2, \
    _type3, _name3, \
    _type4, _name4, \
    _type5, _name5, \
    _type6, _name6, \
    _type7, _name7, \
    _type8, _name8, \
    _type9, _name9, \
    _type10, _name10, \
    _type11, _name11, \
    _type12, _name12, \
    _type13, _name13, \
    _type14, _name14, \
    _type15, _name15, \
    _type16, _name16, \
    _type17, _name17, \
    _type18, _name18, \
    _type19, _name19, \
    _type20, _name20, \
    _type21, _name21, \
    _type22, _name22, \
    _type23, _name23, \
    _type24, _name24, \
    _type25, _name25, \
    _type26, _name26, \
    _type27, _name27, \
    _type28, _name28, \
    _type29, _name29, \
    _type30, _name30, \
    _type31, _name31, \
    _type32, _name32, \
    _type33, _name33, \
    _type34, _name34, \
    _type35, _name35, \
    _type36, _name36, \
    _type37, _name37, \
    _type38, _name38, \
    _type39, _name39, \
    _type40, _name40, \
    _type41, _name41, \
    _type42, _name42, \
    _type43, _name43, \
    _type44, _name44, \
    _type45, _name45, \
    _type46, _name46, \
    _type47, _name47, \
    _type48, _name48, \
    _type49, _name49, \
    NAME, ...) NAME

// In MSVC preprocessor __VA_ARGS__ is treated as a single token in macro argument lists.
// According to this: https://stackoverflow.com/a/32400131
// when wrapping __VA_ARGS__ usage in another macro invocation, it forces the preprocessor to not treat __VA_ARGS__ as a single token.
#define ExpandVAArgs(v) v

#define GenerateElementDeclaration1(type, name) type name;
#define GenerateElementDeclaration2(type, name, ...) GenerateElementDeclaration1(type, name) ExpandVAArgs(GenerateElementDeclaration1(__VA_ARGS__))
#define GenerateElementDeclaration3(type, name, ...) GenerateElementDeclaration1(type, name) ExpandVAArgs(GenerateElementDeclaration2(__VA_ARGS__))
#define GenerateElementDeclaration4(type, name, ...) GenerateElementDeclaration1(type, name) ExpandVAArgs(GenerateElementDeclaration3(__VA_ARGS__))
#define GenerateElementDeclaration5(type, name, ...) GenerateElementDeclaration1(type, name) ExpandVAArgs(GenerateElementDeclaration4(__VA_ARGS__))
#define GenerateElementDeclaration6(type, name, ...) GenerateElementDeclaration1(type, name) ExpandVAArgs(GenerateElementDeclaration5(__VA_ARGS__))
#define GenerateElementDeclaration7(type, name, ...) GenerateElementDeclaration1(type, name) ExpandVAArgs(GenerateElementDeclaration6(__VA_ARGS__))
#define GenerateElementDeclaration8(type, name, ...) GenerateElementDeclaration1(type, name) ExpandVAArgs(GenerateElementDeclaration7(__VA_ARGS__))
#define GenerateElementDeclaration9(type, name, ...) GenerateElementDeclaration1(type, name) ExpandVAArgs(GenerateElementDeclaration8(__VA_ARGS__))
#define GenerateElementDeclaration10(type, name, ...) GenerateElementDeclaration1(type, name) ExpandVAArgs(GenerateElementDeclaration9(__VA_ARGS__))
#define GenerateElementDeclaration11(type, name, ...) GenerateElementDeclaration1(type, name) ExpandVAArgs(GenerateElementDeclaration10(__VA_ARGS__))
#define GenerateElementDeclaration12(type, name, ...) GenerateElementDeclaration1(type, name) ExpandVAArgs(GenerateElementDeclaration11(__VA_ARGS__))
#define GenerateElementDeclaration13(type, name, ...) GenerateElementDeclaration1(type, name) ExpandVAArgs(GenerateElementDeclaration12(__VA_ARGS__))
#define GenerateElementDeclaration14(type, name, ...) GenerateElementDeclaration1(type, name) ExpandVAArgs(GenerateElementDeclaration13(__VA_ARGS__))
#define GenerateElementDeclaration15(type, name, ...) GenerateElementDeclaration1(type, name) ExpandVAArgs(GenerateElementDeclaration14(__VA_ARGS__))
#define GenerateElementDeclaration16(type, name, ...) GenerateElementDeclaration1(type, name) ExpandVAArgs(GenerateElementDeclaration15(__VA_ARGS__))
#define GenerateElementDeclaration17(type, name, ...) GenerateElementDeclaration1(type, name) ExpandVAArgs(GenerateElementDeclaration16(__VA_ARGS__))
#define GenerateElementDeclaration18(type, name, ...) GenerateElementDeclaration1(type, name) ExpandVAArgs(GenerateElementDeclaration17(__VA_ARGS__))
#define GenerateElementDeclaration19(type, name, ...) GenerateElementDeclaration1(type, name) ExpandVAArgs(GenerateElementDeclaration18(__VA_ARGS__))
#define GenerateElementDeclaration20(type, name, ...) GenerateElementDeclaration1(type, name) ExpandVAArgs(GenerateElementDeclaration19(__VA_ARGS__))
#define GenerateElementDeclaration21(type, name, ...) GenerateElementDeclaration1(type, name) ExpandVAArgs(GenerateElementDeclaration20(__VA_ARGS__))
#define GenerateElementDeclaration22(type, name, ...) GenerateElementDeclaration1(type, name) ExpandVAArgs(GenerateElementDeclaration21(__VA_ARGS__))
#define GenerateElementDeclaration23(type, name, ...) GenerateElementDeclaration1(type, name) ExpandVAArgs(GenerateElementDeclaration22(__VA_ARGS__))
#define GenerateElementDeclaration24(type, name, ...) GenerateElementDeclaration1(type, name) ExpandVAArgs(GenerateElementDeclaration23(__VA_ARGS__))
#define GenerateElementDeclaration25(type, name, ...) GenerateElementDeclaration1(type, name) ExpandVAArgs(GenerateElementDeclaration24(__VA_ARGS__))
#define GenerateElementDeclaration26(type, name, ...) GenerateElementDeclaration1(type, name) ExpandVAArgs(GenerateElementDeclaration25(__VA_ARGS__))
#define GenerateElementDeclaration27(type, name, ...) GenerateElementDeclaration1(type, name) ExpandVAArgs(GenerateElementDeclaration26(__VA_ARGS__))
#define GenerateElementDeclaration28(type, name, ...) GenerateElementDeclaration1(type, name) ExpandVAArgs(GenerateElementDeclaration27(__VA_ARGS__))
#define GenerateElementDeclaration29(type, name, ...) GenerateElementDeclaration1(type, name) ExpandVAArgs(GenerateElementDeclaration28(__VA_ARGS__))
#define GenerateElementDeclaration30(type, name, ...) GenerateElementDeclaration1(type, name) ExpandVAArgs(GenerateElementDeclaration29(__VA_ARGS__))
#define GenerateElementDeclaration31(type, name, ...) GenerateElementDeclaration1(type, name) ExpandVAArgs(GenerateElementDeclaration30(__VA_ARGS__))
#define GenerateElementDeclaration32(type, name, ...) GenerateElementDeclaration1(type, name) ExpandVAArgs(GenerateElementDeclaration31(__VA_ARGS__))
#define GenerateElementDeclaration33(type, name, ...) GenerateElementDeclaration1(type, name) ExpandVAArgs(GenerateElementDeclaration32(__VA_ARGS__))
#define GenerateElementDeclaration34(type, name, ...) GenerateElementDeclaration1(type, name) ExpandVAArgs(GenerateElementDeclaration33(__VA_ARGS__))
#define GenerateElementDeclaration35(type, name, ...) GenerateElementDeclaration1(type, name) ExpandVAArgs(GenerateElementDeclaration34(__VA_ARGS__))
#define GenerateElementDeclaration36(type, name, ...) GenerateElementDeclaration1(type, name) ExpandVAArgs(GenerateElementDeclaration35(__VA_ARGS__))
#define GenerateElementDeclaration37(type, name, ...) GenerateElementDeclaration1(type, name) ExpandVAArgs(GenerateElementDeclaration36(__VA_ARGS__))
#define GenerateElementDeclaration38(type, name, ...) GenerateElementDeclaration1(type, name) ExpandVAArgs(GenerateElementDeclaration37(__VA_ARGS__))
#define GenerateElementDeclaration39(type, name, ...) GenerateElementDeclaration1(type, name) ExpandVAArgs(GenerateElementDeclaration38(__VA_ARGS__))
#define GenerateElementDeclaration40(type, name, ...) GenerateElementDeclaration1(type, name) ExpandVAArgs(GenerateElementDeclaration39(__VA_ARGS__))
#define GenerateElementDeclaration41(type, name, ...) GenerateElementDeclaration1(type, name) ExpandVAArgs(GenerateElementDeclaration40(__VA_ARGS__))
#define GenerateElementDeclaration42(type, name, ...) GenerateElementDeclaration1(type, name) ExpandVAArgs(GenerateElementDeclaration41(__VA_ARGS__))
#define GenerateElementDeclaration43(type, name, ...) GenerateElementDeclaration1(type, name) ExpandVAArgs(GenerateElementDeclaration42(__VA_ARGS__))
#define GenerateElementDeclaration44(type, name, ...) GenerateElementDeclaration1(type, name) ExpandVAArgs(GenerateElementDeclaration43(__VA_ARGS__))
#define GenerateElementDeclaration45(type, name, ...) GenerateElementDeclaration1(type, name) ExpandVAArgs(GenerateElementDeclaration44(__VA_ARGS__))
#define GenerateElementDeclaration46(type, name, ...) GenerateElementDeclaration1(type, name) ExpandVAArgs(GenerateElementDeclaration45(__VA_ARGS__))
#define GenerateElementDeclaration47(type, name, ...) GenerateElementDeclaration1(type, name) ExpandVAArgs(GenerateElementDeclaration46(__VA_ARGS__))
#define GenerateElementDeclaration48(type, name, ...) GenerateElementDeclaration1(type, name) ExpandVAArgs(GenerateElementDeclaration47(__VA_ARGS__))
#define GenerateElementDeclaration49(type, name, ...) GenerateElementDeclaration1(type, name) ExpandVAArgs(GenerateElementDeclaration48(__VA_ARGS__))
#define GenerateElementDeclaration50(type, name, ...) GenerateElementDeclaration1(type, name) ExpandVAArgs(GenerateElementDeclaration49(__VA_ARGS__))

#define GenerateElementDeclaration(...) ExpandVAArgs(GetGenerator(__VA_ARGS__,\
    GenerateElementDeclaration50, GenerateElementDeclaration50,\
    GenerateElementDeclaration49, GenerateElementDeclaration49,\
    GenerateElementDeclaration48, GenerateElementDeclaration48,\
    GenerateElementDeclaration47, GenerateElementDeclaration47,\
    GenerateElementDeclaration46, GenerateElementDeclaration46,\
    GenerateElementDeclaration45, GenerateElementDeclaration45,\
    GenerateElementDeclaration44, GenerateElementDeclaration44,\
    GenerateElementDeclaration43, GenerateElementDeclaration43,\
    GenerateElementDeclaration42, GenerateElementDeclaration42,\
    GenerateElementDeclaration41, GenerateElementDeclaration41,\
    GenerateElementDeclaration40, GenerateElementDeclaration40,\
    GenerateElementDeclaration39, GenerateElementDeclaration39,\
    GenerateElementDeclaration38, GenerateElementDeclaration38,\
    GenerateElementDeclaration37, GenerateElementDeclaration37,\
    GenerateElementDeclaration36, GenerateElementDeclaration36,\
    GenerateElementDeclaration35, GenerateElementDeclaration35,\
    GenerateElementDeclaration34, GenerateElementDeclaration34,\
    GenerateElementDeclaration33, GenerateElementDeclaration33,\
    GenerateElementDeclaration32, GenerateElementDeclaration32,\
    GenerateElementDeclaration31, GenerateElementDeclaration31,\
    GenerateElementDeclaration30, GenerateElementDeclaration30,\
    GenerateElementDeclaration29, GenerateElementDeclaration29,\
    GenerateElementDeclaration28, GenerateElementDeclaration28,\
    GenerateElementDeclaration27, GenerateElementDeclaration27,\
    GenerateElementDeclaration26, GenerateElementDeclaration26,\
    GenerateElementDeclaration25, GenerateElementDeclaration25,\
    GenerateElementDeclaration24, GenerateElementDeclaration24,\
    GenerateElementDeclaration23, GenerateElementDeclaration23,\
    GenerateElementDeclaration22, GenerateElementDeclaration22,\
    GenerateElementDeclaration21, GenerateElementDeclaration21,\
    GenerateElementDeclaration20, GenerateElementDeclaration20,\
    GenerateElementDeclaration19, GenerateElementDeclaration19,\
    GenerateElementDeclaration18, GenerateElementDeclaration18,\
    GenerateElementDeclaration17, GenerateElementDeclaration17,\
    GenerateElementDeclaration16, GenerateElementDeclaration16,\
    GenerateElementDeclaration15, GenerateElementDeclaration15,\
    GenerateElementDeclaration14, GenerateElementDeclaration14,\
    GenerateElementDeclaration13, GenerateElementDeclaration13,\
    GenerateElementDeclaration12, GenerateElementDeclaration12,\
    GenerateElementDeclaration11, GenerateElementDeclaration11,\
    GenerateElementDeclaration10, GenerateElementDeclaration10,\
    GenerateElementDeclaration9, GenerateElementDeclaration9,\
    GenerateElementDeclaration8, GenerateElementDeclaration8,\
    GenerateElementDeclaration7, GenerateElementDeclaration7,\
    GenerateElementDeclaration6, GenerateElementDeclaration6,\
    GenerateElementDeclaration5, GenerateElementDeclaration5,\
    GenerateElementDeclaration4, GenerateElementDeclaration4,\
    GenerateElementDeclaration3, GenerateElementDeclaration3,\
    GenerateElementDeclaration2, GenerateElementDeclaration2,\
    GenerateElementDeclaration1, GenerateElementDeclaration1)(__VA_ARGS__))

#define GenerateConstructor1(type, name) name(type(ShaderInstructionFactory::objectMember(_object.operand, #name)))
#define GenerateConstructor2(type, name, ...) GenerateConstructor1(type, name), ExpandVAArgs(GenerateConstructor1(__VA_ARGS__))
#define GenerateConstructor3(type, name, ...) GenerateConstructor1(type, name), ExpandVAArgs(GenerateConstructor2(__VA_ARGS__))
#define GenerateConstructor4(type, name, ...) GenerateConstructor1(type, name), ExpandVAArgs(GenerateConstructor3(__VA_ARGS__))
#define GenerateConstructor5(type, name, ...) GenerateConstructor1(type, name), ExpandVAArgs(GenerateConstructor4(__VA_ARGS__))
#define GenerateConstructor6(type, name, ...) GenerateConstructor1(type, name), ExpandVAArgs(GenerateConstructor5(__VA_ARGS__))
#define GenerateConstructor7(type, name, ...) GenerateConstructor1(type, name), ExpandVAArgs(GenerateConstructor6(__VA_ARGS__))
#define GenerateConstructor8(type, name, ...) GenerateConstructor1(type, name), ExpandVAArgs(GenerateConstructor7(__VA_ARGS__))
#define GenerateConstructor9(type, name, ...) GenerateConstructor1(type, name), ExpandVAArgs(GenerateConstructor8(__VA_ARGS__))
#define GenerateConstructor10(type, name, ...) GenerateConstructor1(type, name), ExpandVAArgs(GenerateConstructor9(__VA_ARGS__))
#define GenerateConstructor11(type, name, ...) GenerateConstructor1(type, name), ExpandVAArgs(GenerateConstructor10(__VA_ARGS__))
#define GenerateConstructor12(type, name, ...) GenerateConstructor1(type, name), ExpandVAArgs(GenerateConstructor11(__VA_ARGS__))
#define GenerateConstructor13(type, name, ...) GenerateConstructor1(type, name), ExpandVAArgs(GenerateConstructor12(__VA_ARGS__))
#define GenerateConstructor14(type, name, ...) GenerateConstructor1(type, name), ExpandVAArgs(GenerateConstructor13(__VA_ARGS__))
#define GenerateConstructor15(type, name, ...) GenerateConstructor1(type, name), ExpandVAArgs(GenerateConstructor14(__VA_ARGS__))
#define GenerateConstructor16(type, name, ...) GenerateConstructor1(type, name), ExpandVAArgs(GenerateConstructor15(__VA_ARGS__))
#define GenerateConstructor17(type, name, ...) GenerateConstructor1(type, name), ExpandVAArgs(GenerateConstructor16(__VA_ARGS__))
#define GenerateConstructor18(type, name, ...) GenerateConstructor1(type, name), ExpandVAArgs(GenerateConstructor17(__VA_ARGS__))
#define GenerateConstructor19(type, name, ...) GenerateConstructor1(type, name), ExpandVAArgs(GenerateConstructor18(__VA_ARGS__))
#define GenerateConstructor20(type, name, ...) GenerateConstructor1(type, name), ExpandVAArgs(GenerateConstructor19(__VA_ARGS__))
#define GenerateConstructor21(type, name, ...) GenerateConstructor1(type, name), ExpandVAArgs(GenerateConstructor20(__VA_ARGS__))
#define GenerateConstructor22(type, name, ...) GenerateConstructor1(type, name), ExpandVAArgs(GenerateConstructor21(__VA_ARGS__))
#define GenerateConstructor23(type, name, ...) GenerateConstructor1(type, name), ExpandVAArgs(GenerateConstructor22(__VA_ARGS__))
#define GenerateConstructor24(type, name, ...) GenerateConstructor1(type, name), ExpandVAArgs(GenerateConstructor23(__VA_ARGS__))
#define GenerateConstructor25(type, name, ...) GenerateConstructor1(type, name), ExpandVAArgs(GenerateConstructor24(__VA_ARGS__))
#define GenerateConstructor26(type, name, ...) GenerateConstructor1(type, name), ExpandVAArgs(GenerateConstructor25(__VA_ARGS__))
#define GenerateConstructor27(type, name, ...) GenerateConstructor1(type, name), ExpandVAArgs(GenerateConstructor26(__VA_ARGS__))
#define GenerateConstructor28(type, name, ...) GenerateConstructor1(type, name), ExpandVAArgs(GenerateConstructor27(__VA_ARGS__))
#define GenerateConstructor29(type, name, ...) GenerateConstructor1(type, name), ExpandVAArgs(GenerateConstructor28(__VA_ARGS__))
#define GenerateConstructor30(type, name, ...) GenerateConstructor1(type, name), ExpandVAArgs(GenerateConstructor29(__VA_ARGS__))
#define GenerateConstructor31(type, name, ...) GenerateConstructor1(type, name), ExpandVAArgs(GenerateConstructor30(__VA_ARGS__))
#define GenerateConstructor32(type, name, ...) GenerateConstructor1(type, name), ExpandVAArgs(GenerateConstructor31(__VA_ARGS__))
#define GenerateConstructor33(type, name, ...) GenerateConstructor1(type, name), ExpandVAArgs(GenerateConstructor32(__VA_ARGS__))
#define GenerateConstructor34(type, name, ...) GenerateConstructor1(type, name), ExpandVAArgs(GenerateConstructor33(__VA_ARGS__))
#define GenerateConstructor35(type, name, ...) GenerateConstructor1(type, name), ExpandVAArgs(GenerateConstructor34(__VA_ARGS__))
#define GenerateConstructor36(type, name, ...) GenerateConstructor1(type, name), ExpandVAArgs(GenerateConstructor35(__VA_ARGS__))
#define GenerateConstructor37(type, name, ...) GenerateConstructor1(type, name), ExpandVAArgs(GenerateConstructor36(__VA_ARGS__))
#define GenerateConstructor38(type, name, ...) GenerateConstructor1(type, name), ExpandVAArgs(GenerateConstructor37(__VA_ARGS__))
#define GenerateConstructor39(type, name, ...) GenerateConstructor1(type, name), ExpandVAArgs(GenerateConstructor38(__VA_ARGS__))
#define GenerateConstructor40(type, name, ...) GenerateConstructor1(type, name), ExpandVAArgs(GenerateConstructor39(__VA_ARGS__))
#define GenerateConstructor41(type, name, ...) GenerateConstructor1(type, name), ExpandVAArgs(GenerateConstructor40(__VA_ARGS__))
#define GenerateConstructor42(type, name, ...) GenerateConstructor1(type, name), ExpandVAArgs(GenerateConstructor41(__VA_ARGS__))
#define GenerateConstructor43(type, name, ...) GenerateConstructor1(type, name), ExpandVAArgs(GenerateConstructor42(__VA_ARGS__))
#define GenerateConstructor44(type, name, ...) GenerateConstructor1(type, name), ExpandVAArgs(GenerateConstructor43(__VA_ARGS__))
#define GenerateConstructor45(type, name, ...) GenerateConstructor1(type, name), ExpandVAArgs(GenerateConstructor44(__VA_ARGS__))
#define GenerateConstructor46(type, name, ...) GenerateConstructor1(type, name), ExpandVAArgs(GenerateConstructor45(__VA_ARGS__))
#define GenerateConstructor47(type, name, ...) GenerateConstructor1(type, name), ExpandVAArgs(GenerateConstructor46(__VA_ARGS__))
#define GenerateConstructor48(type, name, ...) GenerateConstructor1(type, name), ExpandVAArgs(GenerateConstructor47(__VA_ARGS__))
#define GenerateConstructor49(type, name, ...) GenerateConstructor1(type, name), ExpandVAArgs(GenerateConstructor48(__VA_ARGS__))
#define GenerateConstructor50(type, name, ...) GenerateConstructor1(type, name), ExpandVAArgs(GenerateConstructor49(__VA_ARGS__))

#define GenerateConstructor(...) ExpandVAArgs(GetGenerator(__VA_ARGS__,\
    GenerateConstructor50, GenerateConstructor50,\
    GenerateConstructor49, GenerateConstructor49,\
    GenerateConstructor48, GenerateConstructor48,\
    GenerateConstructor47, GenerateConstructor47,\
    GenerateConstructor46, GenerateConstructor46,\
    GenerateConstructor45, GenerateConstructor45,\
    GenerateConstructor44, GenerateConstructor44,\
    GenerateConstructor43, GenerateConstructor43,\
    GenerateConstructor42, GenerateConstructor42,\
    GenerateConstructor41, GenerateConstructor41,\
    GenerateConstructor40, GenerateConstructor40,\
    GenerateConstructor39, GenerateConstructor39,\
    GenerateConstructor38, GenerateConstructor38,\
    GenerateConstructor37, GenerateConstructor37,\
    GenerateConstructor36, GenerateConstructor36,\
    GenerateConstructor35, GenerateConstructor35,\
    GenerateConstructor34, GenerateConstructor34,\
    GenerateConstructor33, GenerateConstructor33,\
    GenerateConstructor32, GenerateConstructor32,\
    GenerateConstructor31, GenerateConstructor31,\
    GenerateConstructor30, GenerateConstructor30,\
    GenerateConstructor29, GenerateConstructor29,\
    GenerateConstructor28, GenerateConstructor28,\
    GenerateConstructor27, GenerateConstructor27,\
    GenerateConstructor26, GenerateConstructor26,\
    GenerateConstructor25, GenerateConstructor25,\
    GenerateConstructor24, GenerateConstructor24,\
    GenerateConstructor23, GenerateConstructor23,\
    GenerateConstructor22, GenerateConstructor22,\
    GenerateConstructor21, GenerateConstructor21,\
    GenerateConstructor20, GenerateConstructor20,\
    GenerateConstructor19, GenerateConstructor19,\
    GenerateConstructor18, GenerateConstructor18,\
    GenerateConstructor17, GenerateConstructor17,\
    GenerateConstructor16, GenerateConstructor16,\
    GenerateConstructor15, GenerateConstructor15,\
    GenerateConstructor14, GenerateConstructor14,\
    GenerateConstructor13, GenerateConstructor13,\
    GenerateConstructor12, GenerateConstructor12,\
    GenerateConstructor11, GenerateConstructor11,\
    GenerateConstructor10, GenerateConstructor10,\
    GenerateConstructor9, GenerateConstructor9,\
    GenerateConstructor8, GenerateConstructor8,\
    GenerateConstructor7, GenerateConstructor7,\
    GenerateConstructor6, GenerateConstructor6,\
    GenerateConstructor5, GenerateConstructor5,\
    GenerateConstructor4, GenerateConstructor4,\
    GenerateConstructor3, GenerateConstructor3,\
    GenerateConstructor2, GenerateConstructor2,\
    GenerateConstructor1, GenerateConstructor1)(__VA_ARGS__))

#define GenerateStructElements1(type, name) xng::ShaderScript::_getElement(#type, #name)
#define GenerateStructElements2(type, name, ...) GenerateStructElements1(type, name), ExpandVAArgs(GenerateStructElements1(__VA_ARGS__))
#define GenerateStructElements3(type, name, ...) GenerateStructElements1(type, name), ExpandVAArgs(GenerateStructElements2(__VA_ARGS__))
#define GenerateStructElements4(type, name, ...) GenerateStructElements1(type, name), ExpandVAArgs(GenerateStructElements3(__VA_ARGS__))
#define GenerateStructElements5(type, name, ...) GenerateStructElements1(type, name), ExpandVAArgs(GenerateStructElements4(__VA_ARGS__))
#define GenerateStructElements6(type, name, ...) GenerateStructElements1(type, name), ExpandVAArgs(GenerateStructElements5(__VA_ARGS__))
#define GenerateStructElements7(type, name, ...) GenerateStructElements1(type, name), ExpandVAArgs(GenerateStructElements6(__VA_ARGS__))
#define GenerateStructElements8(type, name, ...) GenerateStructElements1(type, name), ExpandVAArgs(GenerateStructElements7(__VA_ARGS__))
#define GenerateStructElements9(type, name, ...) GenerateStructElements1(type, name), ExpandVAArgs(GenerateStructElements8(__VA_ARGS__))
#define GenerateStructElements10(type, name, ...) GenerateStructElements1(type, name), ExpandVAArgs(GenerateStructElements9(__VA_ARGS__))
#define GenerateStructElements11(type, name, ...) GenerateStructElements1(type, name), ExpandVAArgs(GenerateStructElements10(__VA_ARGS__))
#define GenerateStructElements12(type, name, ...) GenerateStructElements1(type, name), ExpandVAArgs(GenerateStructElements11(__VA_ARGS__))
#define GenerateStructElements13(type, name, ...) GenerateStructElements1(type, name), ExpandVAArgs(GenerateStructElements12(__VA_ARGS__))
#define GenerateStructElements14(type, name, ...) GenerateStructElements1(type, name), ExpandVAArgs(GenerateStructElements13(__VA_ARGS__))
#define GenerateStructElements15(type, name, ...) GenerateStructElements1(type, name), ExpandVAArgs(GenerateStructElements14(__VA_ARGS__))
#define GenerateStructElements16(type, name, ...) GenerateStructElements1(type, name), ExpandVAArgs(GenerateStructElements15(__VA_ARGS__))
#define GenerateStructElements17(type, name, ...) GenerateStructElements1(type, name), ExpandVAArgs(GenerateStructElements16(__VA_ARGS__))
#define GenerateStructElements18(type, name, ...) GenerateStructElements1(type, name), ExpandVAArgs(GenerateStructElements17(__VA_ARGS__))
#define GenerateStructElements19(type, name, ...) GenerateStructElements1(type, name), ExpandVAArgs(GenerateStructElements18(__VA_ARGS__))
#define GenerateStructElements20(type, name, ...) GenerateStructElements1(type, name), ExpandVAArgs(GenerateStructElements19(__VA_ARGS__))
#define GenerateStructElements21(type, name, ...) GenerateStructElements1(type, name), ExpandVAArgs(GenerateStructElements20(__VA_ARGS__))
#define GenerateStructElements22(type, name, ...) GenerateStructElements1(type, name), ExpandVAArgs(GenerateStructElements21(__VA_ARGS__))
#define GenerateStructElements23(type, name, ...) GenerateStructElements1(type, name), ExpandVAArgs(GenerateStructElements22(__VA_ARGS__))
#define GenerateStructElements24(type, name, ...) GenerateStructElements1(type, name), ExpandVAArgs(GenerateStructElements23(__VA_ARGS__))
#define GenerateStructElements25(type, name, ...) GenerateStructElements1(type, name), ExpandVAArgs(GenerateStructElements24(__VA_ARGS__))
#define GenerateStructElements26(type, name, ...) GenerateStructElements1(type, name), ExpandVAArgs(GenerateStructElements25(__VA_ARGS__))
#define GenerateStructElements27(type, name, ...) GenerateStructElements1(type, name), ExpandVAArgs(GenerateStructElements26(__VA_ARGS__))
#define GenerateStructElements28(type, name, ...) GenerateStructElements1(type, name), ExpandVAArgs(GenerateStructElements27(__VA_ARGS__))
#define GenerateStructElements29(type, name, ...) GenerateStructElements1(type, name), ExpandVAArgs(GenerateStructElements28(__VA_ARGS__))
#define GenerateStructElements30(type, name, ...) GenerateStructElements1(type, name), ExpandVAArgs(GenerateStructElements29(__VA_ARGS__))
#define GenerateStructElements31(type, name, ...) GenerateStructElements1(type, name), ExpandVAArgs(GenerateStructElements30(__VA_ARGS__))
#define GenerateStructElements32(type, name, ...) GenerateStructElements1(type, name), ExpandVAArgs(GenerateStructElements31(__VA_ARGS__))
#define GenerateStructElements33(type, name, ...) GenerateStructElements1(type, name), ExpandVAArgs(GenerateStructElements32(__VA_ARGS__))
#define GenerateStructElements34(type, name, ...) GenerateStructElements1(type, name), ExpandVAArgs(GenerateStructElements33(__VA_ARGS__))
#define GenerateStructElements35(type, name, ...) GenerateStructElements1(type, name), ExpandVAArgs(GenerateStructElements34(__VA_ARGS__))
#define GenerateStructElements36(type, name, ...) GenerateStructElements1(type, name), ExpandVAArgs(GenerateStructElements35(__VA_ARGS__))
#define GenerateStructElements37(type, name, ...) GenerateStructElements1(type, name), ExpandVAArgs(GenerateStructElements36(__VA_ARGS__))
#define GenerateStructElements38(type, name, ...) GenerateStructElements1(type, name), ExpandVAArgs(GenerateStructElements37(__VA_ARGS__))
#define GenerateStructElements39(type, name, ...) GenerateStructElements1(type, name), ExpandVAArgs(GenerateStructElements38(__VA_ARGS__))
#define GenerateStructElements40(type, name, ...) GenerateStructElements1(type, name), ExpandVAArgs(GenerateStructElements39(__VA_ARGS__))
#define GenerateStructElements41(type, name, ...) GenerateStructElements1(type, name), ExpandVAArgs(GenerateStructElements40(__VA_ARGS__))
#define GenerateStructElements42(type, name, ...) GenerateStructElements1(type, name), ExpandVAArgs(GenerateStructElements41(__VA_ARGS__))
#define GenerateStructElements43(type, name, ...) GenerateStructElements1(type, name), ExpandVAArgs(GenerateStructElements42(__VA_ARGS__))
#define GenerateStructElements44(type, name, ...) GenerateStructElements1(type, name), ExpandVAArgs(GenerateStructElements43(__VA_ARGS__))
#define GenerateStructElements45(type, name, ...) GenerateStructElements1(type, name), ExpandVAArgs(GenerateStructElements44(__VA_ARGS__))
#define GenerateStructElements46(type, name, ...) GenerateStructElements1(type, name), ExpandVAArgs(GenerateStructElements45(__VA_ARGS__))
#define GenerateStructElements47(type, name, ...) GenerateStructElements1(type, name), ExpandVAArgs(GenerateStructElements46(__VA_ARGS__))
#define GenerateStructElements48(type, name, ...) GenerateStructElements1(type, name), ExpandVAArgs(GenerateStructElements47(__VA_ARGS__))
#define GenerateStructElements49(type, name, ...) GenerateStructElements1(type, name), ExpandVAArgs(GenerateStructElements48(__VA_ARGS__))
#define GenerateStructElements50(type, name, ...) GenerateStructElements1(type, name), ExpandVAArgs(GenerateStructElements49(__VA_ARGS__))

#define GenerateStructElements(...) ExpandVAArgs(GetGenerator(__VA_ARGS__,\
    GenerateStructElements50, GenerateStructElements50, \
    GenerateStructElements49, GenerateStructElements49, \
    GenerateStructElements48, GenerateStructElements48, \
    GenerateStructElements47, GenerateStructElements47, \
    GenerateStructElements46, GenerateStructElements46, \
    GenerateStructElements45, GenerateStructElements45, \
    GenerateStructElements44, GenerateStructElements44, \
    GenerateStructElements43, GenerateStructElements43, \
    GenerateStructElements42, GenerateStructElements42, \
    GenerateStructElements41, GenerateStructElements41, \
    GenerateStructElements40, GenerateStructElements40, \
    GenerateStructElements39, GenerateStructElements39, \
    GenerateStructElements38, GenerateStructElements38, \
    GenerateStructElements37, GenerateStructElements37, \
    GenerateStructElements36, GenerateStructElements36, \
    GenerateStructElements35, GenerateStructElements35, \
    GenerateStructElements34, GenerateStructElements34, \
    GenerateStructElements33, GenerateStructElements33, \
    GenerateStructElements32, GenerateStructElements32, \
    GenerateStructElements31, GenerateStructElements31, \
    GenerateStructElements30, GenerateStructElements30, \
    GenerateStructElements29, GenerateStructElements29, \
    GenerateStructElements28, GenerateStructElements28, \
    GenerateStructElements27, GenerateStructElements27, \
    GenerateStructElements26, GenerateStructElements26, \
    GenerateStructElements25, GenerateStructElements25, \
    GenerateStructElements24, GenerateStructElements24, \
    GenerateStructElements23, GenerateStructElements23, \
    GenerateStructElements22, GenerateStructElements22, \
    GenerateStructElements21, GenerateStructElements21, \
    GenerateStructElements20, GenerateStructElements20, \
    GenerateStructElements19, GenerateStructElements19, \
    GenerateStructElements18, GenerateStructElements18, \
    GenerateStructElements17, GenerateStructElements17, \
    GenerateStructElements16, GenerateStructElements16, \
    GenerateStructElements15, GenerateStructElements15, \
    GenerateStructElements14, GenerateStructElements14, \
    GenerateStructElements13, GenerateStructElements13, \
    GenerateStructElements12, GenerateStructElements12, \
    GenerateStructElements11, GenerateStructElements11, \
    GenerateStructElements10, GenerateStructElements10, \
    GenerateStructElements9, GenerateStructElements9, \
    GenerateStructElements8, GenerateStructElements8, \
    GenerateStructElements7, GenerateStructElements7, \
    GenerateStructElements6, GenerateStructElements6, \
    GenerateStructElements5, GenerateStructElements5, \
    GenerateStructElements4, GenerateStructElements4, \
    GenerateStructElements3, GenerateStructElements3, \
    GenerateStructElements2, GenerateStructElements2, \
    GenerateStructElements1, GenerateStructElements1,)(__VA_ARGS__))

/**
 *  This macro creates a C++ struct containing the specified member type/name pairs in the var args.
 *  The struct acts as a wrapper around ShaderObject for accessing members of a ShaderStructObject instance through named c++ members.
 *
 *  Object nesting is supported.
 *  Up to 50 member variables are supported.
 *
 *  The getShaderStruct() public static function can be used to retrieve the ShaderStruct instance.
 *
 *  e.g.:
 *      DefineStruct(MvpBuffer, mat4, mvp)
 *
 *      void shader() {
 *          ...
 *          MvpBuffer myBuffer;
 *          myBuffer.mvp = mat4();
 *      }
 *
 * @param name The type name of the struct
 */
#define DefineStruct(name, ...)\
    namespace {static constexpr char _##name[] = #name;\
    struct name {\
        static ShaderStruct getShaderStruct() {\
            return ShaderStruct(#name, {\
                GenerateStructElements(__VA_ARGS__) \
            });\
        }\
        ShaderObject _object;\
        GenerateElementDeclaration(__VA_ARGS__)\
        name() : _object(ShaderStructObject<_##name>()), GenerateConstructor(__VA_ARGS__) {}\
        name(ShaderObject &&buffer, bool b = false) : _object(buffer), GenerateConstructor(__VA_ARGS__) {}\
        operator const ShaderObject&() const { return _object; }\
    };}

#endif //XENGINE_SHADERSTRUCTDEF_HPP
