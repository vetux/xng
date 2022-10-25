/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2022  Julian Zampiccoli
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

#ifndef XENGINE_WINDOWEVENT_HPP
#define XENGINE_WINDOWEVENT_HPP

#include "event/event.hpp"
#include "math/vector2.hpp"

namespace xng {
    struct WindowEvent : public Event {
        std::type_index getEventType() const override {
            return typeid(WindowEvent);
        }

        enum Type {
            WINDOW_CLOSE,
            WINDOW_MOVE,
            WINDOW_RESIZE,
            WINDOW_FOCUS,
            WINDOW_FBRESIZE
        } type{};

        Vec2i value{};
        bool focus{};

        WindowEvent() = default;

        WindowEvent(Type type, Vec2i value, bool focus)
                : type(type), value(value), focus(focus) {

        }
    };
}

#endif //XENGINE_WINDOWEVENT_HPP
