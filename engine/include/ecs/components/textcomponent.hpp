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

#ifndef XENGINE_TEXTCOMPONENT_HPP
#define XENGINE_TEXTCOMPONENT_HPP

#include "io/messageable.hpp"

namespace xng {
    struct XENGINE_EXPORT TextComponent : public Messageable {
        Vec2i pixelSize;
        int lineHeight;
        int lineWidth;
        int lineSpacing;
        std::string fontPath;
        std::string text;
        Vec4f textColor;
        int layer; // The render layer of the text

        Messageable &operator<<(const Message &message) override {
            pixelSize << message.value("pixelSize");
            lineHeight = message.value("lineHeight", 0);
            lineWidth = message.value("lineWidth", 0);
            lineSpacing = message.value("lineSpacing", 0);
            fontPath = message.value("fontPath", std::string());
            text = message.value("text", std::string());
            textColor << message.value("textColor");
            layer = message.value("layer", 0);
            return *this;
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            return message;
        }
    };
}

#endif //XENGINE_TEXTCOMPONENT_HPP
