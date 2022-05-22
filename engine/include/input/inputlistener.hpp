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

#ifndef XENGINE_INPUTLISTENER_HPP
#define XENGINE_INPUTLISTENER_HPP

#include <string>

#include "input/device/gamepad.hpp"
#include "input/device/keyboard.hpp"
#include "input/device/mouse.hpp"

namespace xengine {
    class XENGINE_EXPORT XENGINE_EXPORT InputListener {
    public:
        virtual void onKeyDown(KeyboardKey key) {};

        virtual void onKeyUp(KeyboardKey key) {};

        /**
         * @param xPos The mouse x position in pixels relative to the window.
         * @param yPos The mouse y position in pixels relative to the window.
         */
        virtual void onMouseMove(double xPos, double yPos) {};

        /**
         * @param amount The scroll delta
         */
        virtual void onMouseWheelScroll(double amount) {};

        virtual void onMouseKeyDown(MouseButton key) {};

        virtual void onMouseKeyUp(MouseButton key) {};

        virtual void onTextInput(const std::string &text) {};

        virtual void onGamepadConnected(int id) {};

        virtual void onGamepadDisconnected(int id) {};

        virtual void onGamepadAxis(int id, GamePadAxis axis, double amount) {};

        virtual void onGamepadButtonDown(int id, GamePadButton button) {};

        virtual void onGamepadButtonUp(int id, GamePadButton button) {};
    };
}

#endif //XENGINE_INPUTLISTENER_HPP
