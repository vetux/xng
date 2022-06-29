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

#ifndef XENGINE_GAMEPAD_HPP
#define XENGINE_GAMEPAD_HPP

#include <map>

#include "math/vector2.hpp"
#include "input/device/buttonstate.hpp"

namespace xng {
    enum GamePadAxis {
        LEFT_X,
        LEFT_Y,
        RIGHT_X,
        RIGHT_Y,
        TRIGGER_LEFT,
        TRIGGER_RIGHT,
        AXIS_UNDEFINED
    };

    enum GamePadButton {
        A,
        B,
        X,
        Y,
        BUMPER_LEFT,
        BUMPER_RIGHT,
        BACK,
        START,
        GUIDE,
        LEFT_STICK,
        RIGHT_STICK,
        DPAD_UP,
        DPAD_RIGHT,
        DPADP_LEFT,
        GAMEPAD_BUTTON_UNDEFINED,
        TRIANGLE = Y,
        RECTANGLE = X,
        CROSS = A,
        CIRCLE = B
    };

    class XENGINE_EXPORT GamePad {
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
