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

#include "physics/2d/world2d.hpp"

#include "physics/box2d/worldbox2d.hpp"

#include <stdexcept>

std::unique_ptr<xengine::World2D> xengine::World2D::create(xengine::World2D::Backend backend) {
    switch (backend) {
        case BOX2D:
            return std::make_unique<WorldBox2D>();
    }
    throw std::runtime_error("Invalid 2d physics backend " + std::to_string(backend));
}
