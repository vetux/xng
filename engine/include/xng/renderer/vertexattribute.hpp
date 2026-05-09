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

#ifndef XENGINE_VERTEXATTRIBUTE_HPP
#define XENGINE_VERTEXATTRIBUTE_HPP

namespace xng {
    enum VertexAttribute : int {
        POSITION = 0,
        NORMAL,
        TANGENT,
        BITANGENT,
        UV,
        BONE_INDEX,
        BONE_WEIGHT,
        ATTRIBUTE_BEGIN = POSITION,
        ATTRIBUTE_END = BONE_WEIGHT
    };

    inline size_t getVertexAttributeSize(const VertexAttribute attribute) {
        switch (attribute) {
            default:
                return 4 * 3;
            case UV:
                return 4 * 2;
            case BONE_INDEX:
            case BONE_WEIGHT:
                return 4 * 4;
        }
    }
}

#endif //XENGINE_VERTEXATTRIBUTE_HPP
