/**
 *   xEngine - C++ Game Engine Library
 *   Copyright (C) 2026 Julia Zampiccoli
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the Lesser General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef XENGINE_JOINTBOX2D_HPP
#define XENGINE_JOINTBOX2D_HPP

#include "box2dinclude.hpp"

#include "xng/physics/joint.hpp"

namespace xng {
    namespace box2d {
        class JointBox2D : public Joint {
        public:
            b2Joint *joint;

            ~JointBox2D() override = default;
        };
    }
}

#endif //XENGINE_JOINTBOX2D_HPP
