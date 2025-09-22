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

#ifndef XENGINE_TEXTCOMPONENT_HPP
#define XENGINE_TEXTCOMPONENT_HPP

#include "xng/io/messageable.hpp"
#include "xng/ecs/component.hpp"
#include "xng/font/font.hpp"
#include "xng/graphics/text/textlayoutparameters.hpp"

namespace xng {
    struct XENGINE_EXPORT TextComponent final : Component {
        XNG_COMPONENT_TYPENAME(TextComponent)

        std::string text{};

        ResourceHandle<Font> font{};
        Vec2i pixelSize{};

        TextLayoutParameters layoutParameters{};

        ColorRGBA textColor{};

        bool operator==(const TextComponent &other) const {
            return text == other.text
                   && font == other.font
                   && pixelSize == other.pixelSize
                   && textColor == other.textColor;
        }

        bool operator!=(const TextComponent &other) const {
            return !(*this == other);
        }

        Messageable &operator<<(const Message &message) override {
            message.value("text", text);
            message.value("font", font);
            message.value("pixelSize", pixelSize);
            message.value("layoutParameters", layoutParameters);
            message.value("textColor", textColor);
            return Component::operator<<(message);
        }

        Message &operator>>(Message &message) const override {
            text >> message["text"];
            font >> message["font"];
            pixelSize >> message["pixelSize"];
            layoutParameters >> message["layoutParameters"];
            textColor >> message["textColor"];
            return Component::operator>>(message);
        }
    };
}

#endif //XENGINE_TEXTCOMPONENT_HPP
