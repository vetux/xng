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

#ifndef XENGINE_TEXTCOMPONENT_HPP
#define XENGINE_TEXTCOMPONENT_HPP

#include "xng/io/messageable.hpp"
#include "xng/resource/rawresource.hpp"
#include "xng/gui/textalignment.hpp"
#include "xng/ecs/component.hpp"

namespace xng {
    /**
     * A text component renders the given text.
     * If the resulting rendered text size is larger than the rect dimensions the text is cropped
     */
    struct XENGINE_EXPORT TextComponent : public Component {
        std::string text{};

        Vec2i pixelSize{};

        int lineHeight{};
        int lineWidth{};
        int lineSpacing{};
        TextAlignment alignment{};

        ResourceHandle<RawResource> font{};

        Vec2f textScroll{};

        ColorRGBA textColor{};

        TextureFiltering filter = NEAREST;

        bool operator==(const TextComponent &other) const = default;

        bool operator!=(const TextComponent &other) const = default;

        Messageable &operator<<(const Message &message) override {
            message.value("pixelSize", pixelSize);
            message.value("lineHeight", lineHeight);
            message.value("lineWidth", lineWidth);
            message.value("lineSpacing", lineSpacing);
            message.value("font", font);
            message.value("alignment", (int &) alignment, (int) TEXT_ALIGN_LEFT);
            message.value("textScroll", textScroll);
            message.value("text", text);
            message.value("textColor", textColor);
            message.value("filter", (int &)filter, (int) NEAREST);
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
            textScroll >> message["textScroll"];
            textColor >> message["textColor"];
            filter >> message["filter"];
            return Component::operator>>(message);
        }

        std::type_index getType() const override {
            return typeid(TextComponent);
        }
    };
}

#endif //XENGINE_TEXTCOMPONENT_HPP
