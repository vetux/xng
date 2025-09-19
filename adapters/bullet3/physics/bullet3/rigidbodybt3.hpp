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

#ifndef XENGINE_RIGIDBODYBT3_HPP
#define XENGINE_RIGIDBODYBT3_HPP

#include "xng/physics/rigidbody.hpp"

#include "physics/bullet3/colliderbt3.hpp"
#include "physics/bullet3/bt3convert.hpp"
#include "btBulletDynamicsCommon.h"

namespace xng {
    class WorldBt3;

    class RigidBodyBt3 final : public RigidBody {
    public:
        btRigidBody *body = nullptr;
        btDynamicsWorld *world = nullptr;

        std::unique_ptr<ColliderBt3> collider = nullptr;

        RigidBodyType type{};

        WorldBt3 &worldBt3;

        static btCollisionShape *createShape(const ColliderShape &shape) {
            switch (shape.type) {
                default:
                    throw std::runtime_error("Unsupported collider shape type");
                case COLLIDER_SPHERE:
                    return new btSphereShape(shape.radius);
                case COLLIDER_BOX:
                    return new btBoxShape(convert(shape.halfExtent));
                case COLLIDER_CYLINDER:
                    return new btCylinderShape(convert(shape.halfExtent));
                case COLLIDER_CAPSULE:
                    return new btCapsuleShape(shape.radius, shape.height);
                case COLLIDER_CONE:
                    return new btConeShape(shape.radius, shape.height);
                case COLLIDER_CONVEX_HULL: {
                    std::vector<btScalar> data;
                    if (shape.indices.empty()) {
                        for (auto i = 0; i < shape.vertices.size(); i += 3) {
                            data.emplace_back(shape.vertices.at(i).x);
                            data.emplace_back(shape.vertices.at(i).y);
                            data.emplace_back(shape.vertices.at(i).z);
                            data.emplace_back(shape.vertices.at(i + 1).x);
                            data.emplace_back(shape.vertices.at(i + 1).y);
                            data.emplace_back(shape.vertices.at(i + 1).z);
                            data.emplace_back(shape.vertices.at(i + 2).x);
                            data.emplace_back(shape.vertices.at(i + 2).y);
                            data.emplace_back(shape.vertices.at(i + 2).z);
                        }
                    } else {
                        for (auto i = 0; i < shape.indices.size(); i += 3) {
                            data.emplace_back(shape.vertices.at(shape.indices.at(i)).x);
                            data.emplace_back(shape.vertices.at(shape.indices.at(i)).y);
                            data.emplace_back(shape.vertices.at(shape.indices.at(i)).z);
                            data.emplace_back(shape.vertices.at(shape.indices.at(i + 1)).x);
                            data.emplace_back(shape.vertices.at(shape.indices.at(i + 1)).y);
                            data.emplace_back(shape.vertices.at(shape.indices.at(i + 1)).z);
                            data.emplace_back(shape.vertices.at(shape.indices.at(i + 2)).x);
                            data.emplace_back(shape.vertices.at(shape.indices.at(i + 2)).y);
                            data.emplace_back(shape.vertices.at(shape.indices.at(i + 2)).z);
                        }
                    }
                    return new btConvexHullShape(data.data(),
                                                 static_cast<int>(data.size()) / 3,
                                                 sizeof(btScalar) * 3);
                }
                case COLLIDER_TRIANGLES: {
                    auto *mesh = new btTriangleMesh();
                    if (shape.indices.empty()) {
                        for (auto i = 0; i < shape.vertices.size(); i += 3) {
                            mesh->addTriangle(convert(shape.vertices.at(i)),
                                              convert(shape.vertices.at(i + 1)),
                                              convert(shape.vertices.at(i + 2)));
                        }
                    } else {
                        for (auto i = 0; i < shape.indices.size(); i += 3) {
                            mesh->addTriangle(convert(shape.vertices.at(shape.indices.at(i))),
                                              convert(shape.vertices.at(shape.indices.at(i + 1))),
                                              convert(shape.vertices.at(shape.indices.at(i + 2))));
                        }
                    }
                    return new btBvhTriangleMeshShape(mesh, false);
                }
            }
        }

        explicit RigidBodyBt3(btDynamicsWorld *world, WorldBt3 &worldBt3) : world(world), worldBt3(worldBt3) {
            btScalar mass(0);
            btVector3 localInertia(1, 1, 1);

            btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, nullptr, nullptr, localInertia);
            body = new btRigidBody(rbInfo);

            world->addRigidBody(body);
        }

        RigidBodyBt3(btDynamicsWorld *world,
                     WorldBt3 &worldBt3,
                     const ColliderDesc &desc,
                     RigidBodyType type) : world(world), worldBt3(worldBt3) {
            btCollisionShape *shape = createShape(desc.shape);

            btScalar mass(type == RigidBodyType::STATIC ? 0 : 1);
            btVector3 localInertia(1, 1, 1);

            shape->calculateLocalInertia(mass, localInertia);

            btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, nullptr, shape, localInertia);
            body = new btRigidBody(rbInfo);

            world->addRigidBody(body);

            body->setFriction(desc.properties.friction);
            body->setRestitution(desc.properties.restitution);

            switch (type) {
                case STATIC:
                    if (desc.properties.isSensor)
                        body->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT
                                                | btCollisionObject::CF_NO_CONTACT_RESPONSE);
                    else
                        body->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT);
                    break;
                case KINEMATIC:
                    if (desc.properties.isSensor)
                        body->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT
                                                | btCollisionObject::CF_NO_CONTACT_RESPONSE);
                    else
                        body->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT);
                    break;
                case DYNAMIC:
                    if (desc.properties.isSensor)
                        body->setCollisionFlags(btCollisionObject::CF_DYNAMIC_OBJECT
                                                | btCollisionObject::CF_NO_CONTACT_RESPONSE);
                    else
                        body->setCollisionFlags(btCollisionObject::CF_DYNAMIC_OBJECT);
                    break;
            }

            collider = std::make_unique<ColliderBt3>(*this);
        }

        ~RigidBodyBt3() override;

        void setRigidBodyType(RigidBodyType val) override {
            throw std::runtime_error("Set rigidbody type not supported on bullet3");
        }

        RigidBodyType getRigidBodyType() override {
            return type;
        }

        void setPosition(const Vec3f &position) override {
            auto v = convert(position);
            body->getWorldTransform().setOrigin(v);
        }

        Vec3f getPosition() override {
            return convert(body->getWorldTransform().getOrigin());
        }

        void setVelocity(const Vec3f &velocity) override {
            auto v = convert(velocity);
            body->setLinearVelocity(v);
        }

        Vec3f getVelocity() override {
            return convert(body->getLinearVelocity());
        }

        void setRotation(const Vec3f &rotation) override {
            auto v = convert(Quaternion(rotation));
            body->getWorldTransform().setRotation(v);
        }

        Vec3f getRotation() override {
            return convert(body->getWorldTransform().getRotation()).getEulerAngles();
        }

        void setAngularVelocity(const Vec3f &angularVelocity) override {
            auto v = convert(angularVelocity);
            body->setAngularVelocity(v);
        }

        Vec3f getAngularVelocity() override {
            return convert(body->getAngularVelocity());
        }

        void applyForce(const Vec3f &force, const Vec3f &point) override {
            auto fv = convert(force);
            auto pv = convert(point);
            body->applyForce(fv, pv);
            body->activate();
        }

        void applyTorque(const Vec3f &torque) override {
            auto v = convert(torque);
            body->applyTorque(v);
            body->activate();
        }

        void setAngularFactor(const Vec3f &ax) override {
            body->setAngularFactor({
                ax.x,
                ax.y,
                ax.z
            });
        }

        std::unique_ptr<Collider> createCollider(const ColliderDesc &desc) override {
            throw std::runtime_error("createCollider not suppoert in bullet3, please use world->createRigidbody");
        }

        Collider &getFixedCollider() override {
            if (collider == nullptr) {
                throw std::runtime_error("No collider attached");
            }
            return *collider;
        }

        bool hasFixedCollider() override {
            return collider != nullptr;
        }

        void applyLinearImpulse(const Vec3f &impulse, const Vec3f &point) override {
            auto iv = convert(impulse);
            auto pv = convert(point);
            body->applyImpulse(iv, pv);
            body->activate();
        }

        void applyAngularImpulse(const Vec3f &impulse) override {
            auto v = convert(impulse);
            body->applyTorqueImpulse(v);
            body->activate();
        }

        void setMass(float mass, const Vec3f &center, const Vec3f &localInertia) override {
            auto cv = convert(center);
            auto lv = convert(localInertia);

            btTransform t = btTransform();
            t.setIdentity();
            t.setOrigin(cv);
            body->setCenterOfMassTransform(t);

            body->setMassProps(mass, lv);
        }

        void setMass(float mass, const Vec3f &center) override {
            auto v = convert(center);

            btVector3 localInertia{1, 1, 1};
            body->getCollisionShape()->calculateLocalInertia(mass, localInertia);

            btTransform t = btTransform();
            t.setIdentity();
            t.setOrigin(v);
            body->setCenterOfMassTransform(t);

            body->setMassProps(mass, localInertia);
        }

        float getMass() override {
            return body->getMass();
        }

        void setGravityScale(float scale) override {
            if (scale != 1)
                body->setGravity({0, 0, 0});
        }
    };
}

#endif //XENGINE_RIGIDBODYBT3_HPP
