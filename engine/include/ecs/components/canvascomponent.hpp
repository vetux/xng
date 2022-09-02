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
    /*
     * A canvas renders child canvas transform components to the screen using Renderer2D with the supplied cameraPosition and projection size
     */
    struct XENGINE_EXPORT CanvasComponent : public Messageable {
        bool enabled = true;
        Vec2f cameraPosition;
        Vec2f canvasProjectionSize; // If magnitude larger than 0 the size of the projection when rendering canvas elements
        int layer; // The sorting layer of this canvas

        Messageable &operator<<(const Message &message) override {
            enabled = message.value("enabled", true);
            cameraPosition << message.value("cameraPosition");
            canvasProjectionSize << message.value("canvasProjectionSize");
            layer = message.value("layer", 0);
            return *this;
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            message["enabled"] = enabled;
            cameraPosition >> message["cameraPosition"];
            canvasProjectionSize >> message["canvasProjectionSize"];
            message["layer"] = layer;
            return message;
        }
    };
}

#endif //XENGINE_CANVASCOMPONENT_HPP
