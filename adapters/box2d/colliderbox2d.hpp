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

#ifndef XENGINE_COLLIDERBOX2D_HPP
#define XENGINE_COLLIDERBOX2D_HPP

#include "xng/physics/collidershape.hpp"
#include "xng/physics/colliderdesc.hpp"

#include "box2dinclude.hpp"

#include "xng/physics/collider.hpp"

namespace xng {
    namespace box2d {
        class RigidBodyBox2D;

        class ColliderBox2D : public Collider {
        public:
            RigidBodyBox2D &body;
            b2Fixture *fixture;

            ColliderBox2D(RigidBodyBox2D &body, const ColliderDesc &desc);

            ~ColliderBox2D() override;

            RigidBody &getBody() override;
        };
    }
}

#endif //XENGINE_COLLIDERBOX2D_HPP
