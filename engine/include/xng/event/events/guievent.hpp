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

#ifndef XENGINE_GUIEVENT_HPP
#define XENGINE_GUIEVENT_HPP

#include "xng/event/event.hpp"

#include <utility>

namespace xng {
    struct GuiEvent : public Event {
        enum Type {
            BUTTON_HOVER_START,
            BUTTON_HOVER_STOP,
            BUTTON_CLICK
        } type;

        std::string id;

        GuiEvent(Type type, std::string id) : type(type), id(std::move(id)) {}

        std::type_index getEventType() const override {
            return typeid(GuiEvent);
        }
    };
}
#endif //XENGINE_GUIEVENT_HPP
