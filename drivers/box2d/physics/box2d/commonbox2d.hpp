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

#ifndef XENGINE_COMMONBOX2D_HPP
#define XENGINE_COMMONBOX2D_HPP

#include <stdexcept>

#include "physics/box2d/box2d.hpp"

#include "xng/physics/rigidbody.hpp"

namespace xng {
    Vec3f convert(const b2Vec2 &vec);

    b2Vec2 convert(const Vec3f &vec);

    RigidBody::RigidBodyType convert(b2BodyType type);

    b2BodyType convert(RigidBody::RigidBodyType type);

    b2PolygonShape convert(const std::vector<Vec3f> &points);
}

#endif //XENGINE_COMMONBOX2D_HPP
