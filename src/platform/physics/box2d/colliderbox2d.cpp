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

#ifdef BUILD_ENGINE_PHYSICS_BOX2D

#include <stdexcept>

#include "colliderbox2d.hpp"
#include "commonbox2d.hpp"

namespace xengine {
    ColliderBox2D::ColliderBox2D(b2Body &body) {
        b2FixtureDef def;
        fixture = body.CreateFixture(&def);
    }

    ColliderBox2D::~ColliderBox2D() {
        fixture->GetBody()->DestroyFixture(fixture);
    }

    void ColliderBox2D::setShape(const std::vector<Vec2f> &vertices) {
        auto *body = fixture->GetBody();
        body->DestroyFixture(fixture);

        auto shape = convert(vertices);
        b2FixtureDef def;
        def.shape = &shape;
        fixture = body->CreateFixture(&def);
    }

    void ColliderBox2D::setShape(const std::vector<Vec2f> &vertices, const std::vector<std::size_t> &indices) {
        throw std::runtime_error("Indexed shapes not supported");
    }

    void ColliderBox2D::setDensity(float density) {
        fixture->SetDensity(density);
    }

    float ColliderBox2D::getDensity() {
        return fixture->GetDensity();
    }
}

#endif