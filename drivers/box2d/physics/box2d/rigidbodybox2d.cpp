/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2022  Julian Zampiccoli
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

#include "rigidbodybox2d.hpp"
#include "commonbox2d.hpp"
#include "physics/box2d/worldbox2d.hpp"
#include "math/rotation.hpp"

namespace xng {
    RigidBodyBox2D::RigidBodyBox2D(WorldBox2D &world)
            : world(world) {
        b2BodyDef def = b2BodyDef();
        body = world.world.CreateBody(&def);
    }

    RigidBodyBox2D::~RigidBodyBox2D() {
        body->GetWorld()->DestroyBody(body);
    }

    void RigidBodyBox2D::setRigidBodyType(RigidBodyType type) {
        body->SetType(convert(type));
    }

    RigidBody::RigidBodyType RigidBodyBox2D::getRigidBodyType() {
        return convert(body->GetType());
    }

    void RigidBodyBox2D::setPosition(const Vec3f &position) {
        auto v = convert(position);
        if (std::isnan(v.x)
            || std::isnan(v.y)
            || std::isnan(position.x)
            || std::isnan(position.y)
            || std::isnan(position.z)) {
            throw std::runtime_error("Attempted to set NaN value.");
        }
        body->SetTransform(v, 0);
    }

    Vec3f RigidBodyBox2D::getPosition() {
        auto position = body->GetPosition();
        auto v = convert(position);
        if (std::isnan(v.x)
            || std::isnan(v.y)
            || std::isnan(position.x)
            || std::isnan(position.y)) {
            throw std::runtime_error("Box2D returned NaN value.");
        }
        return v;
    }

    void RigidBodyBox2D::setVelocity(const Vec3f &velocity) {
        auto v = convert(velocity);
        if (std::isnan(v.x)
            || std::isnan(v.y)
            || std::isnan(velocity.x)
            || std::isnan(velocity.y)
            || std::isnan(velocity.z)) {
            throw std::runtime_error("Attempted to set NaN value.");
        }
        body->SetLinearVelocity(v);
    }

    Vec3f RigidBodyBox2D::getVelocity() {
        auto value = body->GetLinearVelocity();
        auto v = convert(value);
        if (std::isnan(v.x)
            || std::isnan(v.y)
            || std::isnan(value.x)
            || std::isnan(value.y)) {
            throw std::runtime_error("Box2D returned NaN value.");
        }
        return v;
    }

    void RigidBodyBox2D::setRotation(const Vec3f &rotation) {
        auto v = convert(rotation);
        if (std::isnan(v.x)
            || std::isnan(v.y)
            || std::isnan(rotation.x)
            || std::isnan(rotation.y)
            || std::isnan(rotation.z)) {
            throw std::runtime_error("Attempted to set NaN value.");
        }
        body->SetTransform(body->GetPosition(), degreesToRadians(rotation.z));
    }

    Vec3f RigidBodyBox2D::getRotation() {
        auto angle = body->GetAngle();
        auto v = radiansToDegrees(angle);
        if (std::isnan(v)
            || std::isnan(angle)) {
            throw std::runtime_error("Box2D returned NaN value.");
        }
        return {0, 0, v};
    }

    void RigidBodyBox2D::setAngularVelocity(const Vec3f &angularVelocity) {
        float v = angularVelocity.z;
        if (std::isnan(v)) {
            throw std::runtime_error("Attempted to set NaN value.");
        }
        body->SetAngularVelocity(v);
    }

    Vec3f RigidBodyBox2D::getAngularVelocity() {
        auto v = body->GetAngularVelocity();
        if (std::isnan(v)) {
            throw std::runtime_error("Box2D returned NaN value.");
        }
        return {0, 0, v};
    }

    void RigidBodyBox2D::applyForce(const Vec3f &force, const Vec3f &point) {
        auto v = convert(force);
        if (std::isnan(v.x)
            || std::isnan(v.y)
            || std::isnan(force.x)
            || std::isnan(force.y)
            || std::isnan(force.z)) {
            throw std::runtime_error("Attempted to set NaN value.");
        }
        auto d = convert(point);
        if (std::isnan(d.x)
            || std::isnan(d.y)
            || std::isnan(point.x)
            || std::isnan(point.y)
            || std::isnan(point.z)) {
            throw std::runtime_error("Attempted to set NaN value.");
        }
        body->ApplyForce(v, d, true);
    }

    void RigidBodyBox2D::applyTorque(const Vec3f &torque) {
        auto v = torque.z;
        if (std::isnan(v)) {
            throw std::runtime_error("Attempted to set NaN value.");
        }
        body->ApplyTorque(v, true);
    }

    void RigidBodyBox2D::applyLinearImpulse(const Vec3f &impulse, const Vec3f &point) {
        auto v = convert(impulse);
        if (std::isnan(v.x)
            || std::isnan(v.y)
            || std::isnan(impulse.x)
            || std::isnan(impulse.y)
            || std::isnan(impulse.z)) {
            throw std::runtime_error("Attempted to set NaN value.");
        }
        auto d = convert(point);
        if (std::isnan(d.x)
            || std::isnan(d.y)
            || std::isnan(point.x)
            || std::isnan(point.y)
            || std::isnan(point.z)) {
            throw std::runtime_error("Attempted to set NaN value.");
        }
        body->ApplyLinearImpulse(v, d, true);
    }

    void RigidBodyBox2D::applyAngularImpulse(const Vec3f &impulse) {
        auto v = impulse.z;
        if (std::isnan(v)) {
            throw std::runtime_error("Attempted to set NaN value.");
        }
        body->ApplyAngularImpulse(v, true);
    }

    std::unique_ptr<Collider> RigidBodyBox2D::createCollider(const ColliderDesc &desc) {
        if (std::isnan(desc.density)
            || std::isnan(desc.friction)
            || std::isnan(desc.restitution)
            || std::isnan(desc.restitution_threshold)) {
            throw std::runtime_error("Attempted to set NaN value.");
        }

        for (auto &v: desc.shape.vertices) {
            if (std::isnan(v.x)
                || std::isnan(v.y)
                || std::isnan(v.z)) {
                throw std::runtime_error("Attempted to set NaN value.");
            }
        }

        return std::make_unique<ColliderBox2D>(*this, desc);
    }

    void RigidBodyBox2D::setLockedRotationAxes(const Vec3b &ax) {
        body->SetFixedRotation(ax.z);
    }

    float RigidBodyBox2D::getMass() {
        return body->GetMass();
    }

    void RigidBodyBox2D::setGravityScale(float scale) {
        if (std::isnan(scale)) {
            throw std::runtime_error("Attempted to set NaN value.");
        }
        body->SetGravityScale(scale);
    }

    void RigidBodyBox2D::setMass(float mass, const Vec3f &center, const Vec3f &rotationalInertia) {
        if (std::isnan(mass)
            || std::isnan(center.x)
            || std::isnan(center.y)
            || std::isnan(center.z)
            || std::isnan(rotationalInertia.x)
            || std::isnan(rotationalInertia.y)
            || std::isnan(rotationalInertia.z))
            throw std::runtime_error("Attempted to set NaN value.");
        b2MassData data;
        data.mass = mass;
        data.center = convert(center);
        data.I = rotationalInertia.z;
        body->SetMassData(&data);
    }
}