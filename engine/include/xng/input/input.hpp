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

#ifndef XENGINE_INPUT_HPP
#define XENGINE_INPUT_HPP

#include "xng/graphics/image.hpp"

#include "xng/event/eventbus.hpp"

#include "xng/input/device/keyboard.hpp"
#include "xng/input/device/mouse.hpp"
#include "xng/input/device/gamepad.hpp"

namespace xng {
    class XENGINE_EXPORT Input {
    public:
        enum CursorMode {
            CURSOR_NORMAL,
            CURSOR_HIDDEN,
            CURSOR_DISABLED
        };

        virtual ~Input() = default;

        virtual void setClipboardText(std::string text) = 0;

        virtual std::string getClipboardText() = 0;

        virtual void setMouseCursorImage(const ImageRGBA &image) = 0;

        virtual void clearMouseCursorImage() = 0;

        virtual void setMouseCursorMode(CursorMode mode) = 0;

        /**
         * Set the event bus on which input events of devices are dispatched.
         *
         * @param bus
         */
        virtual void setEventBus(const EventBus &bus) = 0;

        virtual void clearEventBus() = 0;

        virtual const std::map<int, Keyboard> & getKeyboards() = 0;

        virtual const std::map<int, Mouse> & getMice() = 0;

        virtual const std::map<int, GamePad> &getGamePads() = 0;

        const Keyboard &getKeyboard(const int id = 0) {
            return getKeyboards().at(id);
        }

        const Mouse &getMouse(const int id = 0) {
            return getMice().at(id);
        }

        const GamePad &getGamePad(const int id = 0) {
            return getGamePads().at(id);
        }

        bool getKey(const KeyboardKey key) {
            return getKeyboard().getKey(key);
        }

        bool getKeyDown(const KeyboardKey key) {
            return getKeyboard().getKeyDown(key);
        }
    };
}

#endif //XENGINE_INPUT_HPP
