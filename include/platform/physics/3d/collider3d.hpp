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

#ifndef XENGINE_COLLIDER3D_HPP
#define XENGINE_COLLIDER3D_HPP

#include <vector>

#include "math/vector3.hpp"

namespace xengine {
    class XENGINE_EXPORT Collider3D {
    public:
        virtual void setShape(const std::vector<Vec3f> &vertices) = 0;

        virtual void setShape(const std::vector<Vec3f> &vertices, const std::vector<std::size_t> &indices) = 0;

        virtual void setDensity(float density) = 0;

        virtual float getDensity() = 0;

    protected:
        virtual ~Collider3D() = default;
    };
}

#endif //XENGINE_COLLIDER3D_HPP
