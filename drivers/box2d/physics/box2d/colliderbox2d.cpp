/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2021  Julian Zampiccoli
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

#include <stdexcept>

#include "colliderbox2d.hpp"
#include "commonbox2d.hpp"
#include "physics/box2d/rigidbodybox2d.hpp"
#include "physics/box2d/worldbox2d.hpp"

namespace xng {
    b2PolygonShape convertShape(const ColliderShape &shape) {
        std::vector<b2Vec2> points;
        if (shape.indices.empty()) {
            for (auto &vert: shape.vertices) {
                points.emplace_back(convert(vert));
            }
        } else {
            for (auto &index: shape.indices) {
                auto &vert = shape.vertices.at(index);
                points.emplace_back(convert(vert));
            }
        }
        b2PolygonShape polyShape;
        polyShape.Set(points.data(), numeric_cast<int>(points.size()));
        return polyShape;
    }

    ColliderBox2D::ColliderBox2D(RigidBodyBox2D &body, const ColliderDesc &desc) : body(body) {
        b2PolygonShape polygonShape = convertShape(desc.shape);
        b2FixtureDef fixtureDef;
        fixtureDef.shape = &polygonShape;
        fixtureDef.friction = desc.friction;
        fixtureDef.restitution = desc.restitution;
        fixtureDef.density = desc.density;
        fixtureDef.isSensor = desc.isSensor;
        fixture = body.body->CreateFixture(&fixtureDef);

        body.world.fixtureColliderMapping[fixture] = this;
    }

    ColliderBox2D::~ColliderBox2D() {
        fixture->GetBody()->DestroyFixture(fixture);
        body.world.fixtureColliderMapping.erase(fixture);
    }

    RigidBody &ColliderBox2D::getBody() {
        return body;
    }
}