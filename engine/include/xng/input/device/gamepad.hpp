/**
 *  This file is part of xEngine, a C++ game engine library.
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

#ifndef XENGINE_GAMEPAD_HPP
#define XENGINE_GAMEPAD_HPP

#include <map>

#include "xng/math/vector2.hpp"
#include "xng/input/buttonstate.hpp"
#include "xng/input/inputdevice.hpp"

#include "xng/input/device/gamepadevent.hpp"

namespace xng {
    class XENGINE_EXPORT GamePad : public InputDevice {
    public:
        double getGamepadAxis(GamePadAxis axis) const {
            return axies.at(axis);
        }

        bool getGamepadButton(GamePadButton button) const {
            auto it = buttons.find(button);
            if (it != buttons.end())
                return it->second != RELEASED;
            else
                return false;
        }

        bool getGamepadButtonDown(GamePadButton button) const {
            auto it = buttons.find(button);
            if (it != buttons.end())
                return it->second == PRESSED;
            else
                return false;
        }

        std::string vendor; //Vendor if available eg sony
        std::string model; // Model if available eg ds4

        std::map<GamePadAxis, double> axies;
        std::map<GamePadButton, ButtonState> buttons;
    };
}

#endif //XENGINE_GAMEPAD_HPP
