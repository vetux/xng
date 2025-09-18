/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2023  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 3 of the License, or (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.

 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef XENGINE_FLEXLAYOUTCOMPONENT_HPP
#define XENGINE_FLEXLAYOUTCOMPONENT_HPP

#include <typeindex>

#include "xng/ecs/component.hpp"

#include "xng/flexbox/flexlayout.hpp"

namespace xng {
    /**
     * Represents a flexbox layout node for laying out entities inside a canvas.
     *
     * Each child entity should define their own layout component to control the layout flexbox style.
     */
    struct FlexLayoutComponent final : Component {
        XNG_COMPONENT_TYPENAME(FlexLayoutComponent)

        FlexLayout layout;

        Messageable &operator<<(const Message &message) override {
            message.value("layout", layout);
            return Component::operator<<(message);
        }

        Message &operator>>(Message &message) const override {
            layout >> message["layout"];
            return Component::operator>>(message);
        }
    };
}

#endif //XENGINE_FLEXLAYOUTCOMPONENT_HPP
