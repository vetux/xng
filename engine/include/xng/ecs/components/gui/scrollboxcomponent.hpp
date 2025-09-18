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

#ifndef XENGINE_SCROLLBOXCOMPONENT_HPP
#define XENGINE_SCROLLBOXCOMPONENT_HPP

#include "xng/ecs/component.hpp"
#include "xng/render/scene/image.hpp"

namespace xng {
    /**
     * For the scrollbox contents, a separate content node tree is built with the child entities layouts.
     *
     * If the resulting node layout overflows, scrollbars are displayed and allow scrolling the layout content.
     *
     * The child layouts are calculated relative to the scrollbox viewport.
     *
     * So either child layouts can define fixed sizes where they won't scale with screen size / scrollbox size
     * or as percentages which means they will grow relative to the screen / scrollbox size achieving "resolution scaling"
     */
    struct ScrollBoxComponent final : Component {
        float verticalScrollValue = 0;
        float horizontalScrollValue = 0;

        ColorRGBA barColor = ColorRGBA::white();
        ColorRGBA barBackgroundColor = ColorRGBA::black();
        int barSize = 10; // Width / height of the scrolling bars

        // TODO: Scroll bar / background textures

        Messageable &operator<<(const Message &message) override {
            message.value("verticalScrollValue", verticalScrollValue);
            message.value("horizontalScrollValue", horizontalScrollValue);
            message.value("barColor", barColor);
            message.value("barBackgroundColor", barBackgroundColor);
            message.value("barSize", barSize);
            return Component::operator<<(message);
        }

        Message &operator>>(Message &message) const override {
            verticalScrollValue >> message["verticalScrollValue"];
            horizontalScrollValue >> message["horizontalScrollValue"];
            barColor >> message["barColor"];
            barBackgroundColor >> message["barBackgroundColor"];
            barSize >> message["barSize"];
            return Component::operator>>(message);
        }

        std::type_index getType() const override { return typeid(ScrollBoxComponent); }
    };
}

#endif //XENGINE_SCROLLBOXCOMPONENT_HPP
