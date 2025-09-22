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

#ifndef XENGINE_CANVASCOMPONENT_HPP
#define XENGINE_CANVASCOMPONENT_HPP

#include "xng/io/messageable.hpp"
#include "xng/ecs/component.hpp"
#include "xng/math/vector2.hpp"

namespace xng {
    /**
     * A canvas specifies a rectangular area which can be drawn into using layout components.
     * The canvas entity must define a layout for child items by having a corresponding layout component.
     *
     * Each canvas is rendered to a texture using the Renderer2D and then presented in world space on a plane.
     */
    struct XENGINE_EXPORT CanvasComponent final : Component {
        XNG_COMPONENT_TYPENAME(CanvasComponent)

        bool worldSpace = false; // If false the canvas is rendered relative to the camera, transform still applies
        Vec2i size = Vec2i(0, 0); // Optional size specification. if unassigned, the back buffer size is used.

        Messageable &operator<<(const Message &message) override {
            message.value("worldSpace", worldSpace);
            message.value("size", size);
            return Component::operator<<(message);
        }

        Message &operator>>(Message &message) const override {
            worldSpace >> message["worldSpace"];
            size >> message["size"];
            return Component::operator>>(message);
        }
    };
}

#endif //XENGINE_CANVASCOMPONENT_HPP
