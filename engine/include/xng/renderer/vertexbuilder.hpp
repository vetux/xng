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

#ifndef XENGINE_VERTEXBUILDER_HPP
#define XENGINE_VERTEXBUILDER_HPP

#include <vector>
#include <cstdint>

#include "xng/math/vector2.hpp"
#include "xng/math/vector3.hpp"
#include "xng/math/vector4.hpp"

namespace xng {
    class VertexBuilder {
    public:
        std::vector<uint8_t> build() {
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
                vertex.emplace_back(bytePointer[i]);
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
        std::vector<uint8_t> vertex;
    };
}

#endif //XENGINE_VERTEXBUILDER_HPP
