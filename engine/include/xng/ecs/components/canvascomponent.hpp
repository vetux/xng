/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2023  Julian Zampiccoli
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

#include "xng/io/messageable.hpp"
#include "xng/ecs/component.hpp"

namespace xng {
    /*
     * A canvas renders RectTransformComponents to the screen using Renderer2D
     */
    struct XENGINE_EXPORT CanvasComponent : public Component {
        Vec2f cameraPosition;

        Rectf projection; // The projection or zero to use screensize as projection size

        bool clear = true; // Wheter or not to clear the viewport when rendering this canvas
        ColorRGBA clearColor = ColorRGBA::black();
        int layer; // The sorting layer of this canvas relative to other canvases

        Messageable &operator<<(const Message &message) override {
            message.value("cameraPosition", cameraPosition);
            message.value("projection", projection);
            message.value("clear", clear);
            message.value("clearColor", clearColor);
            message.value("layer", layer);
            return Component::operator<<(message);
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            cameraPosition >> message["cameraPosition"];
            projection >> message["projection"];
            clear >> message["clear"];
            clearColor >> message["clearColor"];
            layer >> message["layer"];
            return Component::operator>>(message);
        }

        std::type_index getType() const override {
            return typeid(CanvasComponent);
        }
    };
}

#endif //XENGINE_CANVASCOMPONENT_HPP
