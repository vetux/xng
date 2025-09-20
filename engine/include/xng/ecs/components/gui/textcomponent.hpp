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

#ifndef XENGINE_TEXTCOMPONENT_HPP
#define XENGINE_TEXTCOMPONENT_HPP

#include "xng/io/messageable.hpp"
#include "xng/ecs/component.hpp"
#include "xng/font/font.hpp"
#include "xng/graphics/2d/text/textalignment.hpp"

namespace xng {
    struct XENGINE_EXPORT TextComponent final : Component {
        XNG_COMPONENT_TYPENAME(TextComponent)

        std::string text{};

        Vec2i pixelSize{};

        int lineHeight{};
        int lineWidth{};
        int lineSpacing{};
        TextAlignment alignment{};

        ResourceHandle<Font> font{};

        ColorRGBA textColor{};

        bool operator==(const TextComponent &other) const {
            return text == other.text
                   && pixelSize == other.pixelSize
                   && lineHeight == other.lineHeight
                   && lineWidth == other.lineWidth
                   && lineSpacing == other.lineSpacing
                   && alignment == other.alignment
                   && font == other.font
                   && textColor == other.textColor;
        }

        bool operator!=(const TextComponent &other) const {
            return !(*this == other);
        }

        Messageable &operator<<(const Message &message) override {
            message.value("pixelSize", pixelSize);
            message.value("lineHeight", lineHeight);
            message.value("lineWidth", lineWidth);
            message.value("lineSpacing", lineSpacing);
            message.value("font", font);
            message.value("alignment", reinterpret_cast<int &>(alignment), static_cast<int>(TEXT_ALIGN_LEFT));
            message.value("text", text);
            message.value("textColor", textColor);
            return Component::operator<<(message);
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            pixelSize >> message["pixelSize"];
            lineHeight >> message["lineHeight"];
            lineWidth >> message["lineWidth"];
            lineSpacing >> message["lineSpacing"];
            font >> message["font"];
            alignment >> message["alignment"];
            text >> message["text"];
            textColor >> message["textColor"];
            return Component::operator>>(message);
        }
    };
}

#endif //XENGINE_TEXTCOMPONENT_HPP
