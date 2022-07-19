/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2021  Julian Zampiccoli
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

#ifndef XENGINE_BUTTONCOMPONENT_HPP
#define XENGINE_BUTTONCOMPONENT_HPP

#include "asset/sprite.hpp"
#include "io/messageable.hpp"

namespace xng {
    struct XENGINE_EXPORT ButtonComponent : public Messageable {
        std::string id;
        int layer; // The layer of this button used for resolving inputs on overlapping buttons

        Messageable &operator<<(const Message &message) override {
            id = message.value("id", std::string());
            layer = message.value("layer", 0);
            return *this;
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            message["id"] = id;
            message["layer"] = layer;
            return message;
        }
    };
}

#endif //XENGINE_BUTTONCOMPONENT_HPP
