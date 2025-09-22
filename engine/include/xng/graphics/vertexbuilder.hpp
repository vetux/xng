/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
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

#include "vertex.hpp"

#include "xng/math/vector2.hpp"
#include "xng/math/vector3.hpp"
#include "xng/math/vector4.hpp"

namespace xng {
    class VertexBuilder {
    public:
        Vertex build() {
            return vertex;
        }

        /**
         * Add a value to the vertex
         *
         * @tparam T
         * @param value
         * @return
         */
        template<typename T>
        VertexBuilder &addValue(const T& value){
            auto bytes = sizeof(T);

            auto * bytePointer = reinterpret_cast<const uint8_t *>(&value);
            for (auto i = 0; i < bytes; i++){
                vertex.buffer.emplace_back(bytePointer[i]);
            }

            return *this;
        }

        template<typename T>
        VertexBuilder &addVec2(const Vector2<T> &value) {
            addValue(value.x);
            addValue(value.y);
            return *this;
        }

        template<typename T>
        VertexBuilder &addVec3(const Vector3<T> &value) {
            addValue(value.x);
            addValue(value.y);
            addValue(value.z);
            return *this;
        }

        template<typename T>
        VertexBuilder &addVec4(const Vector4<T> &value) {
            addValue(value.x);
            addValue(value.y);
            addValue(value.z);
            addValue(value.w);
            return *this;
        }

    private:
        Vertex vertex;
    };
}

#endif //XENGINE_VERTEXBUILDER_HPP
