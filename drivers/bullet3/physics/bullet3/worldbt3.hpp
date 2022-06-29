/**
 *  xEngine - C++ game engine library
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

#ifndef XENGINE_WORLDBT3_HPP
#define XENGINE_WORLDBT3_HPP


#include "physics/3d/world3d.hpp"

namespace xng {
    class WorldBt3 : public World3D {
    public:
        RigidBody3D *createRigidBody() override;

        void destroyRigidBody(RigidBody3D *ptr) override;

        std::set<RigidBody3D *> getRigidBodies() override;

        Joint3D *createJoint() override;

        void destroyJoint(Joint3D *ptr) override;

        std::set<Joint3D *> getJoints() override;

        void addContactListener(ContactListener *listener) override;

        void removeContactListener(ContactListener *listener) override;

        void setGravity(const Vec3f &gravity) override;

        void step(float deltaTime) override;

        std::type_index getType() override;
    };
}


#endif //XENGINE_WORLDBT3_HPP
