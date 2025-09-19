/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2024  Julian Zampiccoli
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

#include "worldbox2d.hpp"
#include "rigidbodybox2d.hpp"
#include "commonbox2d.hpp"

namespace xng {
    namespace box2d {
        WorldBox2D::WorldBox2D()
                : world(b2Vec2(0.0f, -1.0f)) {
            world.SetContactListener(this);
        }

        WorldBox2D::~WorldBox2D() {
            world.SetContactListener(nullptr);
        }

        std::unique_ptr<RigidBody> WorldBox2D::createBody() {
            return std::make_unique<RigidBodyBox2D>(*this);
        }

        std::unique_ptr<Joint> WorldBox2D::createJoint() {
            throw std::runtime_error("Not Implemented");
        }

        void WorldBox2D::addContactListener(World::ContactListener &listener) {
            contactListeners.insert(&listener);
        }

        void WorldBox2D::removeContactListener(World::ContactListener &listener) {
            contactListeners.erase(&listener);
        }

        void WorldBox2D::setGravity(const Vec3f &gravity) {
            world.SetGravity(convert(gravity));
        }

        void WorldBox2D::step(DeltaTime deltaTime) {
            world.Step(deltaTime, 10, 10);
        }

        void WorldBox2D::step(DeltaTime deltaTime, int maxSteps) {
            deltaAccumulator += static_cast<float>(deltaTime);
            int steps = static_cast<int>(deltaAccumulator / timeStep);
            deltaAccumulator -= timeStep * static_cast<float>(steps);
            for (int i = 0; i < steps && i < maxSteps; i++) {
                world.Step(deltaTime, 10, 10);
            }
        }

        void WorldBox2D::BeginContact(b2Contact *c) {
            Contact contact = Contact(*fixtureColliderMapping.at(c->GetFixtureA()),
                                      *fixtureColliderMapping.at(c->GetFixtureB()));
            for (auto &listener: contactListeners) {
                listener->beginContact(contact);
            }
        }

        void WorldBox2D::EndContact(b2Contact *c) {
            Contact contact = Contact(*fixtureColliderMapping.at(c->GetFixtureA()),
                                      *fixtureColliderMapping.at(c->GetFixtureB()));
            for (auto &listener: contactListeners) {
                listener->endContact(contact);
            }
        }

        std::vector<RayHit> WorldBox2D::rayTestAll(const Vec3f &from, const Vec3f &to) {
            b2RayCastInput input;
            input.p1 = convert(from);
            input.p2 = convert(to);
            input.maxFraction = 1;

            std::vector<RayHit> ret;
            for (b2Body *b = world.GetBodyList(); b; b = b->GetNext()) {
                for (b2Fixture *f = b->GetFixtureList(); f; f = f->GetNext()) {
                    b2RayCastOutput output;
                    if (!f->RayCast(&output, input, 0))
                        continue;
                    ret.emplace_back(RayHit{output.fraction,
                                            from + convert(output.fraction * convert(to - from)),
                                            convert(output.normal),
                                            *fixtureColliderMapping.at(f)});
                }
            }
            return ret;
        }

        RayHit WorldBox2D::rayTestClosest(const Vec3f &from, const Vec3f &to) {
            b2RayCastInput input;
            input.p1 = convert(from);
            input.p2 = convert(to);
            input.maxFraction = 1;

            float closestFraction = 1;
            b2Vec2 intersectionNormal(0, 0);
            b2Fixture *closestFixture;
            for (b2Body *b = world.GetBodyList(); b; b = b->GetNext()) {
                for (b2Fixture *f = b->GetFixtureList(); f; f = f->GetNext()) {
                    b2RayCastOutput output;
                    if (!f->RayCast(&output, input, 0))
                        continue;
                    if (output.fraction < closestFraction) {
                        closestFraction = output.fraction;
                        intersectionNormal = output.normal;
                        closestFixture = f;
                    }
                }
            }
            return RayHit{closestFraction,
                          from + convert(closestFraction * convert(to - from)),
                          convert(intersectionNormal),
                          *fixtureColliderMapping.at(closestFixture)};
        }

        std::unique_ptr<RigidBody>
        WorldBox2D::createBody(const ColliderDesc &colliderDesc, RigidBody::RigidBodyType type) {
            return std::make_unique<RigidBodyBox2D>(*this, colliderDesc);
        }
    }
}