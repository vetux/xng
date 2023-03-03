/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2022  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef XENGINE_VERTEXBUILDER_HPP
#define XENGINE_VERTEXBUILDER_HPP

#include "xng/geometry/vertex.hpp"

#include "xng/math/vector2.hpp"
#include "xng/math/vector3.hpp"
#include "xng/math/vector4.hpp"

namespace xng {
    class VertexBuilder {
    public:
        VertexBuilder &beginVertex() {
            vertex = {};
            return *this;
        }

        Vertex build() {
            return vertex;
        }

        /**
         * Add a value of any type whose size matches sizeof(float)
         *
         * @tparam T
         * @param value
         * @return
         */
        template<typename T>
        VertexBuilder &addValue(const T& value){
            static_assert(sizeof(float) == sizeof(T));
            vertex.buffer.emplace_back(reinterpret_cast<const float&>(value));
            return *this;
        }

        template<typename T>
        VertexBuilder &addVec2(const Vector2<T> &value) {
            static_assert(sizeof(float) == sizeof(T));
            vertex.buffer.emplace_back(reinterpret_cast<const float&>(value.x));
            vertex.buffer.emplace_back(reinterpret_cast<const float&>(value.y));
            return *this;
        }

        template<typename T>
        VertexBuilder &addVec3(const Vector3<T> &value) {
            static_assert(sizeof(float) == sizeof(T));
            vertex.buffer.emplace_back(reinterpret_cast<const float&>(value.x));
            vertex.buffer.emplace_back(reinterpret_cast<const float&>(value.y));
            vertex.buffer.emplace_back(reinterpret_cast<const float&>(value.z));
            return *this;
        }

        template<typename T>
        VertexBuilder &addVec4(const Vector4<T> &value) {
            static_assert(sizeof(float) == sizeof(T));
            vertex.buffer.emplace_back(reinterpret_cast<const float&>(value.x));
            vertex.buffer.emplace_back(reinterpret_cast<const float&>(value.y));
            vertex.buffer.emplace_back(reinterpret_cast<const float&>(value.z));
            vertex.buffer.emplace_back(reinterpret_cast<const float&>(value.w));
            return *this;
        }

    private:
        Vertex vertex;
    };
}

#endif //XENGINE_VERTEXBUILDER_HPP
