/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2023  Julian Zampiccoli
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

#ifndef XENGINE_MOUSE_HPP
#define XENGINE_MOUSE_HPP

#include "xng/math/vector2.hpp"

#include "xng/input/buttonstate.hpp"
#include "xng/input/inputdevice.hpp"

#include "xng/input/device/mouseevent.hpp"

namespace xng {
    class XENGINE_EXPORT Mouse : public InputDevice {
    public:
        bool getButton(MouseButton button) const {
            auto it = buttons.find(button);
            if (it != buttons.end())
                return it->second != RELEASED;
            else
                return false;
        }

        bool getButtonDown(MouseButton button) const {
            auto it = buttons.find(button);
            if (it != buttons.end())
                return it->second == PRESSED;
            else
                return false;
        }

        Vec2d wheelDelta;
        Vec2d positionDelta;

        Vec2d position;
        std::map<MouseButton, ButtonState> buttons;
    };
}

#endif //XENGINE_MOUSE_HPP
