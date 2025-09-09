/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2023  Julian Zampiccoli
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

#ifndef XENGINE_FGATTRIBUTEELEMENT_HPP
#define XENGINE_FGATTRIBUTEELEMENT_HPP

#include <stdexcept>

namespace xng {
    struct FGShaderValue {
        enum Type {
            SCALAR,
            VECTOR2,
            VECTOR3,
            VECTOR4,
            MAT2,
            MAT3,
            MAT4
        };

        enum Component {
            BOOLEAN = 0, // 1 Byte boolean
            UNSIGNED_INT, // 4 Byte unsigned
            SIGNED_INT, // 4 Byte signed
            FLOAT, // 4 Byte float
            DOUBLE, // 8 Byte double
        };

        static int getBytes(Component type) {
            switch (type) {
                case BOOLEAN:
                    return 1;
                case UNSIGNED_INT:
                case SIGNED_INT:
                case FLOAT:
                    return 4;
                case DOUBLE:
                    return 8;
                default:
                    throw std::runtime_error("Invalid component");
            }
        }

        static int getCount(Type count) {
            switch (count) {
                case SCALAR:
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
                default:
                    throw std::runtime_error("Invalid type");
            }
        }

        size_t stride() const {
            return (getBytes(component) * getCount(type)) * count;
        }

        FGShaderValue() = default;

        FGShaderValue(const Type type, const Component component, const size_t count = 1)
            : type(type),
              component(component),
              count(count) {
        }

        bool operator==(const FGShaderValue &other) const {
            return type == other.type && component == other.component && count == other.count;
        }

        Type type{};
        Component component{};
        size_t count = 1; // If larger than 1, this element is a fixed size array
    };
}

#endif //XENGINE_FGATTRIBUTEELEMENT_HPP
