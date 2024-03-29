/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2024  Julian Zampiccoli
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

#ifndef XENGINE_MOUSEEVENT_HPP
#define XENGINE_MOUSEEVENT_HPP

#include "xng/event/event.hpp"

namespace xng {
    enum MouseButton {
        MOUSE_BUTTON_LEFT,
        MOUSE_BUTTON_MIDDLE,
        MOUSE_BUTTON_RIGHT,
        MOUSE_BUTTON_OPTIONAL_1,
        MOUSE_BUTTON_OPTIONAL_2,
        MOUSE_BUTTON_OPTIONAL_3,
        MOUSE_BUTTON_OPTIONAL_4,
        MOUSE_BUTTON_OPTIONAL_5,
        MOUSE_BUTTON_UNDEFINED
    };

    struct MouseEvent : public Event {
        enum Type {
            MOUSE_CONNECT,
            MOUSE_DISCONNECT,
            MOUSE_MOVE,
            MOUSE_WHEEL_SCROLL,
            MOUSE_KEY_DOWN,
            MOUSE_KEY_UP,
        } type{};

        int id{};

        double xPos{};
        double yPos{};

        double xAmount{};
        double yAmount{};

        MouseButton key{};

        std::type_index getEventType() const override {
            return typeid(MouseEvent);
        }
    };
}
#endif //XENGINE_MOUSEEVENT_HPP
