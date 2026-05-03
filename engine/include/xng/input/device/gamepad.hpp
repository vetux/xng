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

#ifndef XENGINE_GAMEPAD_HPP
#define XENGINE_GAMEPAD_HPP

#include <map>

#include "xng/input/buttonstate.hpp"

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

    class XENGINE_EXPORT GamePad final {
    public:
        double getGamepadAxis(const GamePadAxis axis) const {
            return axies.at(axis);
        }

        bool getGamepadButton(const GamePadButton button) const {
            auto it = buttons.find(button);
            if (it != buttons.end())
                return it->second != RELEASED;
            return false;
        }

        bool getGamepadButtonDown(const GamePadButton button) const {
            auto it = buttons.find(button);
            if (it != buttons.end())
                return it->second == PRESSED;
            return false;
        }

        std::string vendor; //Vendor if available eg sony
        std::string model; // Model if available eg ds4

        std::map<GamePadAxis, double> axies;
        std::map<GamePadButton, ButtonState> buttons;
    };
}

#endif //XENGINE_GAMEPAD_HPP
