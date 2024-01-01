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

#include "xng/event/events/contactevent.hpp"

#include "xng/ecs/systems/physicssystem.hpp"
#include "xng/ecs/components.hpp"
#include "xng/util/time.hpp"

namespace xng {
    static ColliderDesc applyScale(const ColliderDesc &desc, float scale) {
        ColliderDesc ret = desc;
        ret.shape = {};
        if (desc.shape.indices.empty()) {
            for (auto &vert: desc.shape.vertices) {
                ret.shape.vertices.emplace_back(vert / scale);
            }
        } else {
            for (auto &index: desc.shape.indices) {
                auto &vert = desc.shape.vertices.at(index);
                ret.shape.vertices.emplace_back(vert / scale);
            }
        }
        return ret;
    }

    static ColliderShape getShape(const Mesh &mesh, const ColliderShape &s) {
        if (mesh.vertexLayout != Mesh::getDefaultVertexLayout()) {
            throw std::runtime_error("Invalid mesh vertex layout");
        }
        ColliderShape shape = s;
        for (const auto &v: mesh.vertices) {
            assert(v.buffer.size() >= sizeof(float) * 2);
            float posx = *reinterpret_cast<const float *>(v.buffer.data());
            float posy = *reinterpret_cast<const float *>(v.buffer.data() + sizeof(float));
            float posz = *reinterpret_cast<const float *>(v.buffer.data() + sizeof(float) * 2);
            shape.vertices.emplace_back(posx,
                                        posy,
                                        posz);
        }
        for (auto &i: mesh.indices) {
            shape.indices.emplace_back(i);
        }
        return shape;
    }

    PhysicsSystem::PhysicsSystem(World &world, float scale, float timeStep)
            : world(world), scale(scale), timeStep(timeStep) {}

    PhysicsSystem::PhysicsSystem(World &world, float scale, int maxSteps)
            : world(world), scale(scale), maxSteps(maxSteps) {}


    void PhysicsSystem::start(EntityScene &scene, EventBus &eventBus) {
        for (auto &pair: scene.getPool<RigidBodyComponent>()) {
            if (rigidbodies.find(pair.first) == rigidbodies.end()) {
                onComponentCreate(pair.first, pair.second);
            }
        }
        bus = &eventBus;
        scene.addListener(*this);
        world.addContactListener(*this);
    }

    void PhysicsSystem::stop(EntityScene &scene, EventBus &eventBus) {
        world.removeContactListener(*this);
        scene.removeListener(*this);
        bus = nullptr;
    }

    void PhysicsSystem::update(DeltaTime deltaTime, EntityScene &scene, EventBus &eventBus) {
        for (auto &pair: scene.getPool<RigidBodyComponent>()) {
            if (rigidbodies.find(pair.first) == rigidbodies.end()) {
                if (scene.checkComponent<Collider3DComponent>(pair.first)) {
                    auto comp = scene.getComponent<Collider3DComponent>(pair.first);
                    ColliderDesc desc{};
                    desc.properties = comp.properties;
                    if (comp.mesh.assigned()) {
                        desc.shape = getShape(comp.mesh.get(), comp.shape);
                    } else {
                        desc.shape = comp.shape;
                    }
                    auto body = world.createBody(desc, pair.second.type);
                    body->setAngularFactor(pair.second.angularFactor);
                    body->setGravityScale(pair.second.gravityScale);

                    rigidbodiesReverse[body.get()] = pair.first;
                    rigidbodies[pair.first] = std::move(body);
                } else if (scene.checkComponent<Collider2DComponent>(pair.first)) {
                    auto comp = scene.getComponent<Collider2DComponent>(pair.first);

                    auto body = world.createBody();
                    body->setRigidBodyType(pair.second.type);
                    body->setAngularFactor(pair.second.angularFactor);
                    body->setGravityScale(pair.second.gravityScale);

                    rigidbodiesReverse[body.get()] = pair.first;
                    rigidbodies[pair.first] = std::move(body);

                    for (auto i = 0; i < comp.colliders.size(); i++) {
                        auto collider = rigidbodies.at(pair.first)->createCollider(applyScale(
                                comp.colliders.at(i), scale));
                        colliderIndices[collider.get()] = i;
                        colliders[pair.first].emplace_back(std::move(collider));
                    }

                } else {
                    auto body = world.createBody();
                    body->setRigidBodyType(pair.second.type);
                    body->setAngularFactor(pair.second.angularFactor);
                    body->setGravityScale(pair.second.gravityScale);

                    rigidbodiesReverse[body.get()] = pair.first;
                    rigidbodies[pair.first] = std::move(body);
                }
            }

            auto &rb = *rigidbodies.at(pair.first).get();
            auto tcomp = scene.getComponent<TransformComponent>(pair.first);

            if (pair.second.rotationalInertia.x < 0
                || pair.second.rotationalInertia.y < 0
                || pair.second.rotationalInertia.z < 0)
                rb.setMass(pair.second.type == RigidBody::STATIC ? 0 : pair.second.mass,
                           pair.second.massCenter);
            else
                rb.setMass(pair.second.type == RigidBody::STATIC ? 0 : pair.second.mass,
                           pair.second.massCenter,
                           pair.second.rotationalInertia);

            rb.setVelocity(pair.second.velocity);
            rb.setAngularVelocity(pair.second.angularVelocity);

            rb.setPosition(tcomp.transform.getPosition() / scale);
            rb.setRotation(tcomp.transform.getRotation().getEulerAngles());

            rb.applyForce(pair.second.force, pair.second.forcePoint / scale);
            rb.applyTorque(pair.second.torque);
            rb.applyLinearImpulse(pair.second.impulse, pair.second.impulsePoint);
            rb.applyAngularImpulse(pair.second.angularImpulse);
        }

        if (timeStep == 0) {
            world.step(deltaTime, maxSteps);
        } else {
            deltaAccumulator += static_cast<float>(deltaTime);
            int steps = static_cast<int>(deltaAccumulator / timeStep);
            deltaAccumulator -= timeStep * static_cast<float>(steps);

            for (int i = 0; i < steps && i < maxSteps; i++) {
                for (auto &pair: scene.getPool<RigidBodyComponent>()) {
                    auto &rb = *rigidbodies.at(pair.first).get();
                    rb.applyForce(pair.second.force, pair.second.forcePoint / scale);
                    rb.applyTorque(pair.second.torque);
                    rb.applyLinearImpulse(pair.second.impulse, pair.second.impulsePoint);
                    rb.applyAngularImpulse(pair.second.angularImpulse);
                }

                world.step(DeltaTime(timeStep));
            }
        }
        for (auto &pair: scene.getPool<RigidBodyComponent>()) {
            auto &rb = *rigidbodies.at(pair.first).get();
            auto tcomp = scene.getComponent<TransformComponent>(pair.first);

            tcomp.transform.setPosition(rb.getPosition() * scale);
            tcomp.transform.setRotation(Quaternion(rb.getRotation()));

            RigidBodyComponent comp = pair.second;
            comp.force = Vec3f();
            comp.torque = Vec3f();
            comp.impulse = Vec3f();
            comp.angularImpulse = Vec3f();
            comp.velocity = rb.getVelocity();
            comp.angularVelocity = rb.getAngularVelocity();
            comp.mass = rb.getMass();

            scene.updateComponent(pair.first, comp);
            scene.updateComponent(pair.first, tcomp);
        }
    }

    void PhysicsSystem::onComponentCreate(const EntityHandle &entity, const Component &component) {
    }

    void PhysicsSystem::onComponentDestroy(const EntityHandle &entity, const Component &component) {
        if (component.getType() == typeid(RigidBodyComponent)) {
            if (colliders.find(entity) != colliders.end()) {
                for (auto &col: colliders.at(entity)) {
                    colliderIndices.erase(col.get());
                }
                colliders.erase(entity);
            }
            if (rigidbodies.find(entity) != rigidbodies.end()) {
                rigidbodiesReverse.erase(rigidbodies.at(entity).get());
                rigidbodies.erase(entity);
            }
        }
    }

    void PhysicsSystem::onComponentUpdate(const EntityHandle &entity,
                                          const Component &oldComponent,
                                          const Component &newComponent) {
        if (oldComponent.getType() == typeid(RigidBodyComponent)) {
            auto &oComp = dynamic_cast<const RigidBodyComponent &>(oldComponent);
            auto &nComp = dynamic_cast<const RigidBodyComponent &>(newComponent);

            if (rigidbodies.find(entity) != rigidbodies.end()) {
                rigidbodies.at(entity)->setRigidBodyType(nComp.type);
                rigidbodies.at(entity)->setAngularFactor(nComp.angularFactor);
                rigidbodies.at(entity)->setGravityScale(nComp.gravityScale);
            }
        } else if (oldComponent.getType() == typeid(Collider2DComponent)) {
            auto &oComp = dynamic_cast<const Collider2DComponent &>(oldComponent);
            auto &nComp = dynamic_cast<const Collider2DComponent &>(newComponent);

            if (oComp.colliders != nComp.colliders
                && colliders.find(entity) != colliders.end()) {
                for (auto &col: colliders.at(entity)) {
                    colliderIndices.erase(col.get());
                }
                colliders.erase(entity);

                for (auto i = 0; i < nComp.colliders.size(); i++) {
                    auto collider = rigidbodies.at(entity)->createCollider(applyScale(
                            nComp.colliders.at(i), scale));
                    colliderIndices[collider.get()] = i;
                    colliders[entity].emplace_back(std::move(collider));
                }
            }
        } else if (oldComponent.getType() == typeid(Collider3DComponent)) {
            // Recreate rigidbody
            if (rigidbodies.find(entity) != rigidbodies.end()) {
                rigidbodiesReverse.erase(rigidbodies.at(entity).get());
                rigidbodies.erase(entity);
            }
        }
    }

    void PhysicsSystem::beginContact(const World::Contact &contact) {
        auto entA = rigidbodiesReverse.at(&contact.colliderA.get().getBody());
        auto entB = rigidbodiesReverse.at(&contact.colliderB.get().getBody());
        auto indexA = colliderIndices.at(&contact.colliderA.get());
        auto indexB = colliderIndices.at(&contact.colliderB.get());

        if (bus != nullptr)
            bus->invoke(ContactEvent(ContactEvent::BEGIN_CONTACT,
                                     entA,
                                     entB,
                                     indexA,
                                     indexB));
    }

    void PhysicsSystem::endContact(const World::Contact &contact) {
        auto entA = rigidbodiesReverse.at(&contact.colliderA.get().getBody());
        auto entB = rigidbodiesReverse.at(&contact.colliderB.get().getBody());
        auto indexA = colliderIndices.at(&contact.colliderA.get());
        auto indexB = colliderIndices.at(&contact.colliderB.get());

        if (bus != nullptr)
            bus->invoke(ContactEvent(ContactEvent::END_CONTACT,
                                     entA,
                                     entB,
                                     indexA,
                                     indexB));
    }

    void PhysicsSystem::onEntityDestroy(const EntityHandle &entity) {
        auto itc = colliders.find(entity);
        if (itc != colliders.end()) {
            std::set<Collider *> ptrs;
            for (auto &c: itc->second)
                ptrs.insert(c.get());
            colliders.erase(entity);
            for (auto &c: ptrs)
                colliderIndices.erase(c);
        }
        auto it = rigidbodies.find(entity);
        if (it != rigidbodies.end()) {
            auto ptr = it->second.get();
            rigidbodies.erase(entity);
            rigidbodiesReverse.erase(ptr);
        }
    }
}