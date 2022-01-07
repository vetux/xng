/**
 *  XEngine - C++ game engine library
 *  Copyright (C) 2021  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef XENGINE_SHADERPROGRAM_HPP
#define XENGINE_SHADERPROGRAM_HPP

#include "math/vector2.hpp"
#include "math/vector3.hpp"
#include "math/matrix.hpp"

#include "renderobject.hpp"

namespace xengine {
    class XENGINE_EXPORT ShaderProgram : public RenderObject {
    public:
        ~ShaderProgram() override = default;

        /**
         * This has to be called before invoking any other method on the object.
         *
         * For static state based apis (Like OpenGL) moving this call to the user
         * prevents unnecessary activations.
         */
        virtual void activate() = 0;

        virtual bool setTexture(const std::string &name, int slot) = 0;

        virtual bool setBool(const std::string &name, bool value) = 0;

        virtual bool setInt(const std::string &name, int value) = 0;

        virtual bool setFloat(const std::string &name, float value) = 0;


        virtual bool setVec2(const std::string &name, const Vec2b &value) = 0;

        virtual bool setVec2(const std::string &name, const Vec2i &value) = 0;

        virtual bool setVec2(const std::string &name, const Vec2f &value) = 0;


        virtual bool setVec3(const std::string &name, const Vec3b &value) = 0;

        virtual bool setVec3(const std::string &name, const Vec3i &value) = 0;

        virtual bool setVec3(const std::string &name, const Vec3f &value) = 0;


        virtual bool setVec4(const std::string &name, const Vec4b &value) = 0;

        virtual bool setVec4(const std::string &name, const Vec4i &value) = 0;

        virtual bool setVec4(const std::string &name, const Vec4f &value) = 0;


        virtual bool setMat2(const std::string &name, const Mat2f &value) = 0;

        virtual bool setMat3(const std::string &name, const Mat3f &value) = 0;

        virtual bool setMat4(const std::string &name, const Mat4f &value) = 0;

        /**
         * Assign uniforms based on a location eg.
         *
         * register(...) in hlsl
         * layout(...) in glsl
         *
         * Lookup by the location directly is about twice as fast compared to looking up by name
         *
         * Note that when using the shader compiler to cross compile hlsl the
         * implementation uses shaderc which for no reason merges global variables (But not samplers) in a struct XENGINE_EXPORT
         * and thus looses all locations.
         */
        virtual bool setTexture(int location, int slot) = 0;

        virtual bool setBool(int location, bool value) = 0;

        virtual bool setInt(int location, int value) = 0;

        virtual bool setFloat(int location, float value) = 0;


        virtual bool setVec2(int location, const Vec2b &value) = 0;

        virtual bool setVec2(int location, const Vec2i &value) = 0;

        virtual bool setVec2(int location, const Vec2f &value) = 0;


        virtual bool setVec3(int location, const Vec3b &value) = 0;

        virtual bool setVec3(int location, const Vec3i &value) = 0;

        virtual bool setVec3(int location, const Vec3f &value) = 0;


        virtual bool setVec4(int location, const Vec4b &value) = 0;

        virtual bool setVec4(int location, const Vec4i &value) = 0;

        virtual bool setVec4(int location, const Vec4f &value) = 0;


        virtual bool setMat2(int location, const Mat2f &value) = 0;

        virtual bool setMat3(int location, const Mat3f &value) = 0;

        virtual bool setMat4(int location, const Mat4f &value) = 0;
    };
}

#endif //XENGINE_SHADERPROGRAM_HPP
