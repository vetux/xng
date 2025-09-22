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

#include <stdexcept>

#include "colliderbox2d.hpp"
#include "commonbox2d.hpp"
#include "physics/box2d/rigidbodybox2d.hpp"
#include "physics/box2d/worldbox2d.hpp"

namespace xng {
    namespace box2d {
        b2PolygonShape convertShape(const ColliderShape &shape) {
            if (shape.type != COLLIDER_2D) {
                throw std::runtime_error("Collider must be COLLIDER_2D for box2d physics driver.");
            }

            std::vector<b2Vec2> points;
            if (shape.indices.empty()) {
                for (auto &vert: shape.vertices) {
                    points.emplace_back(convert(vert));
                }
            } else {
                for (auto &index: shape.indices) {
                    const auto &vert = shape.vertices.at(index);
                    points.emplace_back(convert(vert));
                }
            }
            b2PolygonShape polyShape = b2PolygonShape();
            polyShape.Set(points.data(), static_cast<int>(points.size()));
            return polyShape;
        }

        ColliderBox2D::ColliderBox2D(RigidBodyBox2D &body, const ColliderDesc &desc) : body(body) {
            b2PolygonShape polygonShape = convertShape(desc.shape);
            b2FixtureDef fixtureDef;
            fixtureDef.shape = &polygonShape;
            fixtureDef.friction = desc.properties.friction;
            fixtureDef.restitution = desc.properties.restitution;
            fixtureDef.density = desc.properties.density;
            fixtureDef.isSensor = desc.properties.isSensor;

            fixture = body.body->CreateFixture(&fixtureDef);

            b2MassData mass;
            fixture->GetMassData(&mass);

            auto v = 1.0f / mass.mass;
            if (std::isnan(v) || std::isinf(v)) {
                throw std::runtime_error(
                        "Computed mass for fixture is invalid. Check your collider density / shape configuration.");
            }

            body.world.fixtureColliderMapping[fixture] = this;
        }

        ColliderBox2D::~ColliderBox2D() {
            b2Fixture *fix = fixture;
            fixture->GetBody()->DestroyFixture(fixture);
            body.world.fixtureColliderMapping.erase(fix);
        }

        RigidBody &ColliderBox2D::getBody() {
            return body;
        }
    }
}