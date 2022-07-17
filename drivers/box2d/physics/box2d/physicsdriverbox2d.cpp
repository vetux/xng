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

#include "physics/box2d/physicsdriverbox2d.hpp"
#include "physics/box2d/worldbox2d.hpp"

#include "driver/registerdriver.hpp"

#warning "Not Implemented"

namespace xng {
    static bool driverRegistered = REGISTER_DRIVER("box2d", PhysicsDriverBox2D);

    std::type_index xng::PhysicsDriverBox2D::getType() {
        return {typeid(PhysicsDriverBox2D)};
    }

    std::unique_ptr<World> xng::PhysicsDriverBox2D::createWorld() {
        return std::make_unique<WorldBox2D>();
    }
}