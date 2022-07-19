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

#ifndef XENGINE_CANVASCOMPONENT_HPP
#define XENGINE_CANVASCOMPONENT_HPP

#include "io/messageable.hpp"

namespace xng {
    struct XENGINE_EXPORT CanvasComponent : public Messageable {
        Vec2f cameraPosition;
        int layer; // The sorting layer of this canvas

        Messageable &operator<<(const Message &message) override {
            cameraPosition << message.value("cameraPosition");
            layer = message.value("layer", 0);
            return *this;
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            cameraPosition >> message["cameraPosition"];
            message["layer"] = layer;
            return message;
        }
    };
}

#endif //XENGINE_CANVASCOMPONENT_HPP
