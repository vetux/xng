/**
 *  Mana - 3D Game Engine
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

#ifndef MANA_MATHSCHEMA_HPP
#define MANA_MATHSCHEMA_HPP

#include "io/message.hpp"

#include "math/grid.hpp"
#include "math/matrix.hpp"
#include "math/rectangle.hpp"
#include "math/transform.hpp"
#include "math/vector2.hpp"
#include "math/vector3.hpp"
#include "math/vector4.hpp"

namespace xengine {
    MANA_EXPORT Grid &operator<<(Grid &grid, const Message &message);

    MANA_EXPORT Message &operator<<(Message &message, const Grid &grid);

    MANA_EXPORT Mat4f &operator<<(Mat4f &matrix, const Message &message);

    MANA_EXPORT Message &operator<<(Message &message, const Mat4f &grid);

    MANA_EXPORT Rectf &operator<<(Rectf &mat, const Message &message);

    MANA_EXPORT Message &operator<<(Message &message, const Rectf &grid);

    MANA_EXPORT Transform &operator<<(Transform &grid, const Message &message);

    MANA_EXPORT Message &operator<<(Message &message, const Transform &grid);

    MANA_EXPORT Vec2f &operator<<(Vec2f &grid, const Message &message);

    MANA_EXPORT Message &operator<<(Message &message, const Vec2f &grid);

    MANA_EXPORT Vec2i &operator<<(Vec2i &grid, const Message &message);

    MANA_EXPORT Message &operator<<(Message &message, const Vec2i &grid);

    MANA_EXPORT Vec3f &operator<<(Vec3f &grid, const Message &message);

    MANA_EXPORT Message &operator<<(Message &message, const Vec3f &grid);

    MANA_EXPORT Vec4f &operator<<(Vec4f &grid, const Message &message);

    MANA_EXPORT Message &operator<<(Message &message, const Vec4f &grid);

    MANA_EXPORT Quaternion &operator<<(Quaternion &q, const Message &message);

    MANA_EXPORT Message &operator<<(Message &message, const Quaternion &q);
}

#endif //MANA_MATHSCHEMA_HPP
