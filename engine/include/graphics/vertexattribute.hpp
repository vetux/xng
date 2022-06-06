/**
 *  xEngine - C++ game engine library
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

#ifndef XENGINE_VERTEXATTRIBUTE_HPP
#define XENGINE_VERTEXATTRIBUTE_HPP

namespace xengine {
    struct VertexAttribute {
        enum Type {
            SINGLE,
            VECTOR2,
            VECTOR3,
            VECTOR4,
            MAT2,
            MAT3,
            MAT4
        };

        enum Component {
            UNSIGNED_BYTE, // 1 Byte unsigned
            SIGNED_BYTE, // 1 Byte signed
            UNSIGNED_INT, // 4 Byte unsigned
            SIGNED_INT, // 4 Byte signed
            FLOAT, // 4 Byte float
            DOUBLE // 8 Byte double
        };

        static int getBytes(Component type) {
            switch (type) {
                case UNSIGNED_BYTE:
                case SIGNED_BYTE:
                    return 1;
                case UNSIGNED_INT:
                case SIGNED_INT:
                case FLOAT:
                    return 4;
                case DOUBLE:
                    return 8;
            }
        }

        static int getCount(Type count) {
            switch (count) {
                case SINGLE:
                    return 1;
                case VECTOR2:
                    return 2;
                case VECTOR3:
                    return 3;
                case VECTOR4:
                case MAT2:
                    return 4;
                case MAT3:
                    return 9;
                case MAT4:
                    return 16;
            }
        }

        int offset() const {
            return getBytes(component) * getCount(type);
        }

        VertexAttribute() = default;

        VertexAttribute(Type type, Component component) : type(type), component(component) {}

        Type type;
        Component component;
    };
}
#endif //XENGINE_VERTEXATTRIBUTE_HPP
