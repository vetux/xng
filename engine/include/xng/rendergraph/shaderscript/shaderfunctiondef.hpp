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

#ifndef XENGINE_SHADERFUNCTIONDEF_HPP
#define XENGINE_SHADERFUNCTIONDEF_HPP

#include <optional>

#include "xng/rendergraph/shader/shaderdatatype.hpp"
#include "xng/rendergraph/shader/shaderfunction.hpp"

#include "xng/rendergraph/shaderscript/shadercommondef.hpp"

namespace xng::ShaderScript {
    static std::optional<ShaderDataType> getFunctionReturnType(const std::string &typeName) {
        if (typeName == "void") {
            return {};
        }
        return ShaderDataType::fromString(typeName);
    }

    static ColorFormat getColorFormat(const std::string &name) {
        if (name == "R") return R;
        if (name == "RG") return RG;
        if (name == "RGB") return RGB;
        if (name == "RGBA") return RGBA;
        if (name == "DEPTH") return DEPTH;
        if (name == "STENCIL") return STENCIL;
        if (name == "DEPTH_STENCIL") return DEPTH_STENCIL;

        if (name == "DEPTH_32F") return DEPTH_32F;
        if (name == "DEPTH_24") return DEPTH_24;
        if (name == "DEPTH_16") return DEPTH_16;
        if (name == "STENCIL_32") return STENCIL_32;
        if (name == "STENCIL_16") return STENCIL_16;
        if (name == "STENCIL_8") return STENCIL_8;
        if (name == "DEPTH24_STENCIL8") return DEPTH24_STENCIL8;

        if (name == "R_COMPRESSED") return R_COMPRESSED;
        if (name == "RG_COMPRESSED") return RG_COMPRESSED;
        if (name == "RGB_COMPRESSED") return RGB_COMPRESSED;
        if (name == "RGBA_COMPRESSED") return RGBA_COMPRESSED;

        if (name == "R8") return R8;
        if (name == "RG8") return RG8;
        if (name == "RGB8") return RGB8;
        if (name == "RGBA8") return RGBA8;

        if (name == "R16") return R16;
        if (name == "RG16") return RG16;
        if (name == "RGB16") return RGB16;
        if (name == "RGBA16") return RGBA16;

        if (name == "RGB12") return RGB12;
        if (name == "RGBA12") return RGBA12;
        if (name == "RGB10") return RGB10;

        if (name == "R16F") return R16F;
        if (name == "RG16F") return RG16F;
        if (name == "RGB16F") return RGB16F;
        if (name == "RGBA16F") return RGBA16F;

        if (name == "R32F") return R32F;
        if (name == "RG32F") return RG32F;
        if (name == "RGB32F") return RGB32F;
        if (name == "RGBA32F") return RGBA32F;

        if (name == "R8I") return R8I;
        if (name == "RG8I") return RG8I;
        if (name == "RGB8I") return RGB8I;
        if (name == "RGBA8I") return RGBA8I;

        if (name == "R16I") return R16I;
        if (name == "RG16I") return RG16I;
        if (name == "RGB16I") return RGB16I;
        if (name == "RGBA16I") return RGBA16I;

        if (name == "R32I") return R32I;
        if (name == "RG32I") return RG32I;
        if (name == "RGB32I") return RGB32I;
        if (name == "RGBA32I") return RGBA32I;

        if (name == "R8UI") return R8UI;
        if (name == "RG8UI") return RG8UI;
        if (name == "RGB8UI") return RGB8UI;
        if (name == "RGBA8UI") return RGBA8UI;

        if (name == "R16UI") return R16UI;
        if (name == "RG16UI") return RG16UI;
        if (name == "RGB16UI") return RGB16UI;
        if (name == "RGBA16UI") return RGBA16UI;

        if (name == "R32UI") return R32UI;
        if (name == "RG32UI") return RG32UI;
        if (name == "RGB32UI") return RGB32UI;
        if (name == "RGBA32UI") return RGBA32UI;

        throw std::runtime_error("Invalid color format");
    }

    static ShaderFunction::Argument getFunctionArgument(const std::string &typeName, const std::string &name) {
        if (typeName.find("Texture2DMSArray") == 0) {
            std::string color = typeName.substr(typeName.find('<') + 1, typeName.rfind('>') - typeName.find('<') - 1);
            return ShaderFunction::Argument(ShaderTexture(TEXTURE_2D_MULTISAMPLE_ARRAY, getColorFormat(color)), name);
        }
        if (typeName.find("TextureCubeArray") == 0) {
            std::string color = typeName.substr(typeName.find('<') + 1, typeName.rfind('>') - typeName.find('<') - 1);
            return ShaderFunction::Argument(ShaderTexture(TEXTURE_CUBE_MAP_ARRAY, getColorFormat(color)), name);
        }
        if (typeName.find("TextureCube") == 0) {
            std::string color = typeName.substr(typeName.find('<') + 1, typeName.rfind('>') - typeName.find('<') - 1);
            return ShaderFunction::Argument(ShaderTexture(TEXTURE_CUBE_MAP, getColorFormat(color)), name);
        }
        if (typeName.find("Texture2DMS") == 0) {
            std::string color = typeName.substr(typeName.find('<') + 1, typeName.rfind('>') - typeName.find('<') - 1);
            return ShaderFunction::Argument(ShaderTexture(TEXTURE_2D_MULTISAMPLE, getColorFormat(color)), name);
        }
        if (typeName.find("Texture2DArray") == 0) {
            std::string color = typeName.substr(typeName.find('<') + 1, typeName.rfind('>') - typeName.find('<') - 1);
            return ShaderFunction::Argument(ShaderTexture(TEXTURE_2D_ARRAY, getColorFormat(color)), name);
        }
        if (typeName.find("Texture2D") == 0) {
            std::string color = typeName.substr(typeName.find('<') + 1, typeName.rfind('>') - typeName.find('<') - 1);
            return ShaderFunction::Argument(ShaderTexture(TEXTURE_2D, getColorFormat(color)), name);
        }
        return ShaderFunction::Argument(ShaderDataType::fromString(typeName), name);
    }
}

#define GenerateFunctionArgs1(type, name) getFunctionArgument(#type, #name)
#define GenerateFunctionArgs2(type, name, ...) GenerateFunctionArgs1(type, name), ExpandVAArgs(GenerateFunctionArgs1(__VA_ARGS__))
#define GenerateFunctionArgs3(type, name, ...) GenerateFunctionArgs1(type, name), ExpandVAArgs(GenerateFunctionArgs2(__VA_ARGS__))
#define GenerateFunctionArgs4(type, name, ...) GenerateFunctionArgs1(type, name), ExpandVAArgs(GenerateFunctionArgs3(__VA_ARGS__))
#define GenerateFunctionArgs5(type, name, ...) GenerateFunctionArgs1(type, name), ExpandVAArgs(GenerateFunctionArgs4(__VA_ARGS__))
#define GenerateFunctionArgs6(type, name, ...) GenerateFunctionArgs1(type, name), ExpandVAArgs(GenerateFunctionArgs5(__VA_ARGS__))
#define GenerateFunctionArgs7(type, name, ...) GenerateFunctionArgs1(type, name), ExpandVAArgs(GenerateFunctionArgs6(__VA_ARGS__))
#define GenerateFunctionArgs8(type, name, ...) GenerateFunctionArgs1(type, name), ExpandVAArgs(GenerateFunctionArgs7(__VA_ARGS__))
#define GenerateFunctionArgs9(type, name, ...) GenerateFunctionArgs1(type, name), ExpandVAArgs(GenerateFunctionArgs8(__VA_ARGS__))
#define GenerateFunctionArgs10(type, name, ...) GenerateFunctionArgs1(type, name), ExpandVAArgs(GenerateFunctionArgs9(__VA_ARGS__))
#define GenerateFunctionArgs11(type, name, ...) GenerateFunctionArgs1(type, name), ExpandVAArgs(GenerateFunctionArgs10(__VA_ARGS__))
#define GenerateFunctionArgs12(type, name, ...) GenerateFunctionArgs1(type, name), ExpandVAArgs(GenerateFunctionArgs11(__VA_ARGS__))
#define GenerateFunctionArgs13(type, name, ...) GenerateFunctionArgs1(type, name), ExpandVAArgs(GenerateFunctionArgs12(__VA_ARGS__))
#define GenerateFunctionArgs14(type, name, ...) GenerateFunctionArgs1(type, name), ExpandVAArgs(GenerateFunctionArgs13(__VA_ARGS__))
#define GenerateFunctionArgs15(type, name, ...) GenerateFunctionArgs1(type, name), ExpandVAArgs(GenerateFunctionArgs14(__VA_ARGS__))
#define GenerateFunctionArgs16(type, name, ...) GenerateFunctionArgs1(type, name), ExpandVAArgs(GenerateFunctionArgs15(__VA_ARGS__))
#define GenerateFunctionArgs17(type, name, ...) GenerateFunctionArgs1(type, name), ExpandVAArgs(GenerateFunctionArgs16(__VA_ARGS__))
#define GenerateFunctionArgs18(type, name, ...) GenerateFunctionArgs1(type, name), ExpandVAArgs(GenerateFunctionArgs17(__VA_ARGS__))
#define GenerateFunctionArgs19(type, name, ...) GenerateFunctionArgs1(type, name), ExpandVAArgs(GenerateFunctionArgs18(__VA_ARGS__))
#define GenerateFunctionArgs20(type, name, ...) GenerateFunctionArgs1(type, name), ExpandVAArgs(GenerateFunctionArgs19(__VA_ARGS__))
#define GenerateFunctionArgs21(type, name, ...) GenerateFunctionArgs1(type, name), ExpandVAArgs(GenerateFunctionArgs20(__VA_ARGS__))
#define GenerateFunctionArgs22(type, name, ...) GenerateFunctionArgs1(type, name), ExpandVAArgs(GenerateFunctionArgs21(__VA_ARGS__))
#define GenerateFunctionArgs23(type, name, ...) GenerateFunctionArgs1(type, name), ExpandVAArgs(GenerateFunctionArgs22(__VA_ARGS__))
#define GenerateFunctionArgs24(type, name, ...) GenerateFunctionArgs1(type, name), ExpandVAArgs(GenerateFunctionArgs23(__VA_ARGS__))
#define GenerateFunctionArgs25(type, name, ...) GenerateFunctionArgs1(type, name), ExpandVAArgs(GenerateFunctionArgs24(__VA_ARGS__))
#define GenerateFunctionArgs26(type, name, ...) GenerateFunctionArgs1(type, name), ExpandVAArgs(GenerateFunctionArgs25(__VA_ARGS__))
#define GenerateFunctionArgs27(type, name, ...) GenerateFunctionArgs1(type, name), ExpandVAArgs(GenerateFunctionArgs26(__VA_ARGS__))
#define GenerateFunctionArgs28(type, name, ...) GenerateFunctionArgs1(type, name), ExpandVAArgs(GenerateFunctionArgs27(__VA_ARGS__))
#define GenerateFunctionArgs29(type, name, ...) GenerateFunctionArgs1(type, name), ExpandVAArgs(GenerateFunctionArgs28(__VA_ARGS__))
#define GenerateFunctionArgs30(type, name, ...) GenerateFunctionArgs1(type, name), ExpandVAArgs(GenerateFunctionArgs29(__VA_ARGS__))
#define GenerateFunctionArgs31(type, name, ...) GenerateFunctionArgs1(type, name), ExpandVAArgs(GenerateFunctionArgs30(__VA_ARGS__))
#define GenerateFunctionArgs32(type, name, ...) GenerateFunctionArgs1(type, name), ExpandVAArgs(GenerateFunctionArgs31(__VA_ARGS__))
#define GenerateFunctionArgs33(type, name, ...) GenerateFunctionArgs1(type, name), ExpandVAArgs(GenerateFunctionArgs32(__VA_ARGS__))
#define GenerateFunctionArgs34(type, name, ...) GenerateFunctionArgs1(type, name), ExpandVAArgs(GenerateFunctionArgs33(__VA_ARGS__))
#define GenerateFunctionArgs35(type, name, ...) GenerateFunctionArgs1(type, name), ExpandVAArgs(GenerateFunctionArgs34(__VA_ARGS__))
#define GenerateFunctionArgs36(type, name, ...) GenerateFunctionArgs1(type, name), ExpandVAArgs(GenerateFunctionArgs35(__VA_ARGS__))
#define GenerateFunctionArgs37(type, name, ...) GenerateFunctionArgs1(type, name), ExpandVAArgs(GenerateFunctionArgs36(__VA_ARGS__))
#define GenerateFunctionArgs38(type, name, ...) GenerateFunctionArgs1(type, name), ExpandVAArgs(GenerateFunctionArgs37(__VA_ARGS__))
#define GenerateFunctionArgs39(type, name, ...) GenerateFunctionArgs1(type, name), ExpandVAArgs(GenerateFunctionArgs38(__VA_ARGS__))
#define GenerateFunctionArgs40(type, name, ...) GenerateFunctionArgs1(type, name), ExpandVAArgs(GenerateFunctionArgs39(__VA_ARGS__))
#define GenerateFunctionArgs41(type, name, ...) GenerateFunctionArgs1(type, name), ExpandVAArgs(GenerateFunctionArgs40(__VA_ARGS__))
#define GenerateFunctionArgs42(type, name, ...) GenerateFunctionArgs1(type, name), ExpandVAArgs(GenerateFunctionArgs41(__VA_ARGS__))
#define GenerateFunctionArgs43(type, name, ...) GenerateFunctionArgs1(type, name), ExpandVAArgs(GenerateFunctionArgs42(__VA_ARGS__))
#define GenerateFunctionArgs44(type, name, ...) GenerateFunctionArgs1(type, name), ExpandVAArgs(GenerateFunctionArgs43(__VA_ARGS__))
#define GenerateFunctionArgs45(type, name, ...) GenerateFunctionArgs1(type, name), ExpandVAArgs(GenerateFunctionArgs44(__VA_ARGS__))
#define GenerateFunctionArgs46(type, name, ...) GenerateFunctionArgs1(type, name), ExpandVAArgs(GenerateFunctionArgs45(__VA_ARGS__))
#define GenerateFunctionArgs47(type, name, ...) GenerateFunctionArgs1(type, name), ExpandVAArgs(GenerateFunctionArgs46(__VA_ARGS__))
#define GenerateFunctionArgs48(type, name, ...) GenerateFunctionArgs1(type, name), ExpandVAArgs(GenerateFunctionArgs47(__VA_ARGS__))
#define GenerateFunctionArgs49(type, name, ...) GenerateFunctionArgs1(type, name), ExpandVAArgs(GenerateFunctionArgs48(__VA_ARGS__))
#define GenerateFunctionArgs50(type, name, ...) GenerateFunctionArgs1(type, name), ExpandVAArgs(GenerateFunctionArgs49(__VA_ARGS__))

#define GenerateFunctionArgs(...) ExpandVAArgs(GetGenerator(__VA_ARGS__,\
    GenerateFunctionArgs50, GenerateFunctionArgs50,\
    GenerateFunctionArgs49, GenerateFunctionArgs49,\
    GenerateFunctionArgs48, GenerateFunctionArgs48,\
    GenerateFunctionArgs47, GenerateFunctionArgs47,\
    GenerateFunctionArgs46, GenerateFunctionArgs46,\
    GenerateFunctionArgs45, GenerateFunctionArgs45,\
    GenerateFunctionArgs44, GenerateFunctionArgs44,\
    GenerateFunctionArgs43, GenerateFunctionArgs43,\
    GenerateFunctionArgs42, GenerateFunctionArgs42,\
    GenerateFunctionArgs41, GenerateFunctionArgs41,\
    GenerateFunctionArgs40, GenerateFunctionArgs40,\
    GenerateFunctionArgs39, GenerateFunctionArgs39,\
    GenerateFunctionArgs38, GenerateFunctionArgs38,\
    GenerateFunctionArgs37, GenerateFunctionArgs37,\
    GenerateFunctionArgs36, GenerateFunctionArgs36,\
    GenerateFunctionArgs35, GenerateFunctionArgs35,\
    GenerateFunctionArgs34, GenerateFunctionArgs34,\
    GenerateFunctionArgs33, GenerateFunctionArgs33,\
    GenerateFunctionArgs32, GenerateFunctionArgs32,\
    GenerateFunctionArgs31, GenerateFunctionArgs31,\
    GenerateFunctionArgs30, GenerateFunctionArgs30,\
    GenerateFunctionArgs29, GenerateFunctionArgs29,\
    GenerateFunctionArgs28, GenerateFunctionArgs28,\
    GenerateFunctionArgs27, GenerateFunctionArgs27,\
    GenerateFunctionArgs26, GenerateFunctionArgs26,\
    GenerateFunctionArgs25, GenerateFunctionArgs25,\
    GenerateFunctionArgs24, GenerateFunctionArgs24,\
    GenerateFunctionArgs23, GenerateFunctionArgs23,\
    GenerateFunctionArgs22, GenerateFunctionArgs22,\
    GenerateFunctionArgs21, GenerateFunctionArgs21,\
    GenerateFunctionArgs20, GenerateFunctionArgs20,\
    GenerateFunctionArgs19, GenerateFunctionArgs19,\
    GenerateFunctionArgs18, GenerateFunctionArgs18,\
    GenerateFunctionArgs17, GenerateFunctionArgs17,\
    GenerateFunctionArgs16, GenerateFunctionArgs16,\
    GenerateFunctionArgs15, GenerateFunctionArgs15,\
    GenerateFunctionArgs14, GenerateFunctionArgs14,\
    GenerateFunctionArgs13, GenerateFunctionArgs13,\
    GenerateFunctionArgs12, GenerateFunctionArgs12,\
    GenerateFunctionArgs11, GenerateFunctionArgs11,\
    GenerateFunctionArgs10, GenerateFunctionArgs10,\
    GenerateFunctionArgs9, GenerateFunctionArgs9,\
    GenerateFunctionArgs8, GenerateFunctionArgs8,\
    GenerateFunctionArgs7, GenerateFunctionArgs7,\
    GenerateFunctionArgs6, GenerateFunctionArgs6,\
    GenerateFunctionArgs5, GenerateFunctionArgs5,\
    GenerateFunctionArgs4, GenerateFunctionArgs4,\
    GenerateFunctionArgs3, GenerateFunctionArgs3,\
    GenerateFunctionArgs2, GenerateFunctionArgs2,\
    GenerateFunctionArgs1, GenerateFunctionArgs1)(__VA_ARGS__))

#define GenerateArgumentAccessors1(type, name) type name(xng::ShaderScript::argument(#name));
#define GenerateArgumentAccessors2(type, name, ...) GenerateArgumentAccessors1(type, name) ExpandVAArgs(GenerateArgumentAccessors1(__VA_ARGS__))
#define GenerateArgumentAccessors3(type, name, ...) GenerateArgumentAccessors1(type, name) ExpandVAArgs(GenerateArgumentAccessors2(__VA_ARGS__))
#define GenerateArgumentAccessors4(type, name, ...) GenerateArgumentAccessors1(type, name) ExpandVAArgs(GenerateArgumentAccessors3(__VA_ARGS__))
#define GenerateArgumentAccessors5(type, name, ...) GenerateArgumentAccessors1(type, name) ExpandVAArgs(GenerateArgumentAccessors4(__VA_ARGS__))
#define GenerateArgumentAccessors6(type, name, ...) GenerateArgumentAccessors1(type, name) ExpandVAArgs(GenerateArgumentAccessors5(__VA_ARGS__))
#define GenerateArgumentAccessors7(type, name, ...) GenerateArgumentAccessors1(type, name) ExpandVAArgs(GenerateArgumentAccessors6(__VA_ARGS__))
#define GenerateArgumentAccessors8(type, name, ...) GenerateArgumentAccessors1(type, name) ExpandVAArgs(GenerateArgumentAccessors7(__VA_ARGS__))
#define GenerateArgumentAccessors9(type, name, ...) GenerateArgumentAccessors1(type, name) ExpandVAArgs(GenerateArgumentAccessors8(__VA_ARGS__))
#define GenerateArgumentAccessors10(type, name, ...) GenerateArgumentAccessors1(type, name) ExpandVAArgs(GenerateArgumentAccessors9(__VA_ARGS__))
#define GenerateArgumentAccessors11(type, name, ...) GenerateArgumentAccessors1(type, name) ExpandVAArgs(GenerateArgumentAccessors10(__VA_ARGS__))
#define GenerateArgumentAccessors12(type, name, ...) GenerateArgumentAccessors1(type, name) ExpandVAArgs(GenerateArgumentAccessors11(__VA_ARGS__))
#define GenerateArgumentAccessors13(type, name, ...) GenerateArgumentAccessors1(type, name) ExpandVAArgs(GenerateArgumentAccessors12(__VA_ARGS__))
#define GenerateArgumentAccessors14(type, name, ...) GenerateArgumentAccessors1(type, name) ExpandVAArgs(GenerateArgumentAccessors13(__VA_ARGS__))
#define GenerateArgumentAccessors15(type, name, ...) GenerateArgumentAccessors1(type, name) ExpandVAArgs(GenerateArgumentAccessors14(__VA_ARGS__))
#define GenerateArgumentAccessors16(type, name, ...) GenerateArgumentAccessors1(type, name) ExpandVAArgs(GenerateArgumentAccessors15(__VA_ARGS__))
#define GenerateArgumentAccessors17(type, name, ...) GenerateArgumentAccessors1(type, name) ExpandVAArgs(GenerateArgumentAccessors16(__VA_ARGS__))
#define GenerateArgumentAccessors18(type, name, ...) GenerateArgumentAccessors1(type, name) ExpandVAArgs(GenerateArgumentAccessors17(__VA_ARGS__))
#define GenerateArgumentAccessors19(type, name, ...) GenerateArgumentAccessors1(type, name) ExpandVAArgs(GenerateArgumentAccessors18(__VA_ARGS__))
#define GenerateArgumentAccessors20(type, name, ...) GenerateArgumentAccessors1(type, name) ExpandVAArgs(GenerateArgumentAccessors19(__VA_ARGS__))
#define GenerateArgumentAccessors21(type, name, ...) GenerateArgumentAccessors1(type, name) ExpandVAArgs(GenerateArgumentAccessors20(__VA_ARGS__))
#define GenerateArgumentAccessors22(type, name, ...) GenerateArgumentAccessors1(type, name) ExpandVAArgs(GenerateArgumentAccessors21(__VA_ARGS__))
#define GenerateArgumentAccessors23(type, name, ...) GenerateArgumentAccessors1(type, name) ExpandVAArgs(GenerateArgumentAccessors22(__VA_ARGS__))
#define GenerateArgumentAccessors24(type, name, ...) GenerateArgumentAccessors1(type, name) ExpandVAArgs(GenerateArgumentAccessors23(__VA_ARGS__))
#define GenerateArgumentAccessors25(type, name, ...) GenerateArgumentAccessors1(type, name) ExpandVAArgs(GenerateArgumentAccessors24(__VA_ARGS__))
#define GenerateArgumentAccessors26(type, name, ...) GenerateArgumentAccessors1(type, name) ExpandVAArgs(GenerateArgumentAccessors25(__VA_ARGS__))
#define GenerateArgumentAccessors27(type, name, ...) GenerateArgumentAccessors1(type, name) ExpandVAArgs(GenerateArgumentAccessors26(__VA_ARGS__))
#define GenerateArgumentAccessors28(type, name, ...) GenerateArgumentAccessors1(type, name) ExpandVAArgs(GenerateArgumentAccessors27(__VA_ARGS__))
#define GenerateArgumentAccessors29(type, name, ...) GenerateArgumentAccessors1(type, name) ExpandVAArgs(GenerateArgumentAccessors28(__VA_ARGS__))
#define GenerateArgumentAccessors30(type, name, ...) GenerateArgumentAccessors1(type, name) ExpandVAArgs(GenerateArgumentAccessors29(__VA_ARGS__))
#define GenerateArgumentAccessors31(type, name, ...) GenerateArgumentAccessors1(type, name) ExpandVAArgs(GenerateArgumentAccessors30(__VA_ARGS__))
#define GenerateArgumentAccessors32(type, name, ...) GenerateArgumentAccessors1(type, name) ExpandVAArgs(GenerateArgumentAccessors31(__VA_ARGS__))
#define GenerateArgumentAccessors33(type, name, ...) GenerateArgumentAccessors1(type, name) ExpandVAArgs(GenerateArgumentAccessors32(__VA_ARGS__))
#define GenerateArgumentAccessors34(type, name, ...) GenerateArgumentAccessors1(type, name) ExpandVAArgs(GenerateArgumentAccessors33(__VA_ARGS__))
#define GenerateArgumentAccessors35(type, name, ...) GenerateArgumentAccessors1(type, name) ExpandVAArgs(GenerateArgumentAccessors34(__VA_ARGS__))
#define GenerateArgumentAccessors36(type, name, ...) GenerateArgumentAccessors1(type, name) ExpandVAArgs(GenerateArgumentAccessors35(__VA_ARGS__))
#define GenerateArgumentAccessors37(type, name, ...) GenerateArgumentAccessors1(type, name) ExpandVAArgs(GenerateArgumentAccessors36(__VA_ARGS__))
#define GenerateArgumentAccessors38(type, name, ...) GenerateArgumentAccessors1(type, name) ExpandVAArgs(GenerateArgumentAccessors37(__VA_ARGS__))
#define GenerateArgumentAccessors39(type, name, ...) GenerateArgumentAccessors1(type, name) ExpandVAArgs(GenerateArgumentAccessors38(__VA_ARGS__))
#define GenerateArgumentAccessors40(type, name, ...) GenerateArgumentAccessors1(type, name) ExpandVAArgs(GenerateArgumentAccessors39(__VA_ARGS__))
#define GenerateArgumentAccessors41(type, name, ...) GenerateArgumentAccessors1(type, name) ExpandVAArgs(GenerateArgumentAccessors40(__VA_ARGS__))
#define GenerateArgumentAccessors42(type, name, ...) GenerateArgumentAccessors1(type, name) ExpandVAArgs(GenerateArgumentAccessors41(__VA_ARGS__))
#define GenerateArgumentAccessors43(type, name, ...) GenerateArgumentAccessors1(type, name) ExpandVAArgs(GenerateArgumentAccessors42(__VA_ARGS__))
#define GenerateArgumentAccessors44(type, name, ...) GenerateArgumentAccessors1(type, name) ExpandVAArgs(GenerateArgumentAccessors43(__VA_ARGS__))
#define GenerateArgumentAccessors45(type, name, ...) GenerateArgumentAccessors1(type, name) ExpandVAArgs(GenerateArgumentAccessors44(__VA_ARGS__))
#define GenerateArgumentAccessors46(type, name, ...) GenerateArgumentAccessors1(type, name) ExpandVAArgs(GenerateArgumentAccessors45(__VA_ARGS__))
#define GenerateArgumentAccessors47(type, name, ...) GenerateArgumentAccessors1(type, name) ExpandVAArgs(GenerateArgumentAccessors46(__VA_ARGS__))
#define GenerateArgumentAccessors48(type, name, ...) GenerateArgumentAccessors1(type, name) ExpandVAArgs(GenerateArgumentAccessors47(__VA_ARGS__))
#define GenerateArgumentAccessors49(type, name, ...) GenerateArgumentAccessors1(type, name) ExpandVAArgs(GenerateArgumentAccessors48(__VA_ARGS__))
#define GenerateArgumentAccessors50(type, name, ...) GenerateArgumentAccessors1(type, name) ExpandVAArgs(GenerateArgumentAccessors49(__VA_ARGS__))

#define GenerateArgumentAccessors(...) ExpandVAArgs(GetGenerator(__VA_ARGS__,\
    GenerateArgumentAccessors50, GenerateArgumentAccessors50,\
    GenerateArgumentAccessors49, GenerateArgumentAccessors49,\
    GenerateArgumentAccessors48, GenerateArgumentAccessors48,\
    GenerateArgumentAccessors47, GenerateArgumentAccessors47,\
    GenerateArgumentAccessors46, GenerateArgumentAccessors46,\
    GenerateArgumentAccessors45, GenerateArgumentAccessors45,\
    GenerateArgumentAccessors44, GenerateArgumentAccessors44,\
    GenerateArgumentAccessors43, GenerateArgumentAccessors43,\
    GenerateArgumentAccessors42, GenerateArgumentAccessors42,\
    GenerateArgumentAccessors41, GenerateArgumentAccessors41,\
    GenerateArgumentAccessors40, GenerateArgumentAccessors40,\
    GenerateArgumentAccessors39, GenerateArgumentAccessors39,\
    GenerateArgumentAccessors38, GenerateArgumentAccessors38,\
    GenerateArgumentAccessors37, GenerateArgumentAccessors37,\
    GenerateArgumentAccessors36, GenerateArgumentAccessors36,\
    GenerateArgumentAccessors35, GenerateArgumentAccessors35,\
    GenerateArgumentAccessors34, GenerateArgumentAccessors34,\
    GenerateArgumentAccessors33, GenerateArgumentAccessors33,\
    GenerateArgumentAccessors32, GenerateArgumentAccessors32,\
    GenerateArgumentAccessors31, GenerateArgumentAccessors31,\
    GenerateArgumentAccessors30, GenerateArgumentAccessors30,\
    GenerateArgumentAccessors29, GenerateArgumentAccessors29,\
    GenerateArgumentAccessors28, GenerateArgumentAccessors28,\
    GenerateArgumentAccessors27, GenerateArgumentAccessors27,\
    GenerateArgumentAccessors26, GenerateArgumentAccessors26,\
    GenerateArgumentAccessors25, GenerateArgumentAccessors25,\
    GenerateArgumentAccessors24, GenerateArgumentAccessors24,\
    GenerateArgumentAccessors23, GenerateArgumentAccessors23,\
    GenerateArgumentAccessors22, GenerateArgumentAccessors22,\
    GenerateArgumentAccessors21, GenerateArgumentAccessors21,\
    GenerateArgumentAccessors20, GenerateArgumentAccessors20,\
    GenerateArgumentAccessors19, GenerateArgumentAccessors19,\
    GenerateArgumentAccessors18, GenerateArgumentAccessors18,\
    GenerateArgumentAccessors17, GenerateArgumentAccessors17,\
    GenerateArgumentAccessors16, GenerateArgumentAccessors16,\
    GenerateArgumentAccessors15, GenerateArgumentAccessors15,\
    GenerateArgumentAccessors14, GenerateArgumentAccessors14,\
    GenerateArgumentAccessors13, GenerateArgumentAccessors13,\
    GenerateArgumentAccessors12, GenerateArgumentAccessors12,\
    GenerateArgumentAccessors11, GenerateArgumentAccessors11,\
    GenerateArgumentAccessors10, GenerateArgumentAccessors10,\
    GenerateArgumentAccessors9, GenerateArgumentAccessors9,\
    GenerateArgumentAccessors8, GenerateArgumentAccessors8,\
    GenerateArgumentAccessors7, GenerateArgumentAccessors7,\
    GenerateArgumentAccessors6, GenerateArgumentAccessors6,\
    GenerateArgumentAccessors5, GenerateArgumentAccessors5,\
    GenerateArgumentAccessors4, GenerateArgumentAccessors4,\
    GenerateArgumentAccessors3, GenerateArgumentAccessors3,\
    GenerateArgumentAccessors2, GenerateArgumentAccessors2,\
    GenerateArgumentAccessors1, GenerateArgumentAccessors1)(__VA_ARGS__))

#define Function(returnType, name, ...) ShaderBuilder::instance().BeginFunction(#name, { GenerateFunctionArgs(__VA_ARGS__) }, xng::ShaderScript::getFunctionReturnType(#returnType)); { GenerateArgumentAccessors(__VA_ARGS__)

#define End } ShaderBuilder::instance().EndFunction();

#endif //XENGINE_SHADERFUNCTIONDEF_HPP
