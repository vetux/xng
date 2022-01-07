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

#ifndef XENGINE_COLLIDERBOX2D_HPP
#define XENGINE_COLLIDERBOX2D_HPP

#include "box2d.hpp"

#include "platform/physics/2d/collider2d.hpp"

namespace xengine {
    class ColliderBox2D : public Collider2D {
    public:
        b2Fixture *fixture;

        explicit ColliderBox2D(b2Body &body);

        ~ColliderBox2D() override;

        void setShape(const std::vector<Vec2f> &vertices) override;

        void setShape(const std::vector<Vec2f> &vertices, const std::vector<std::size_t> &indices) override;

        void setDensity(float density) override;

        float getDensity() override;
    };
}

#endif //XENGINE_COLLIDERBOX2D_HPP
