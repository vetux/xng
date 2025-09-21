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

#ifndef XENGINE_TEXTLAYOUTPARAMETERS_HPP
#define XENGINE_TEXTLAYOUTPARAMETERS_HPP

#include "xng/graphics/text/textalignment.hpp"

namespace xng {
    struct TextLayoutParameters : Messageable {
        /**
         * If larger than zero, the value in pixels at which a newline is rendered if the widths of characters in the current line exceed it.
         * The character is then drawn on a new line
         */
        int maxLineWidth = 0;

        /**
         * Spacing between lines (Added to the font-defined line height.)
         */
        int lineSpacing = 0;

        /**
         * The horizontal alignment of lines within a rendered text e.g.,
         *
         * ALIGN_LEFT:
         * TestTestTest123
         * TestTest123
         * Test123
         *
         * ALIGN_CENTER:
         * TestTestTest123
         *   TestTest123
         *     Test123
         *
         * ALIGN_RIGHT:
         * TestTestTest123
         *     TestTest123
         *         Test123
         */
        TextAlignment alignment = TEXT_ALIGN_LEFT;

        TextLayoutParameters() = default;

        TextLayoutParameters(int line_width, int line_spacing, TextAlignment alignment)
            : maxLineWidth(line_width),
              lineSpacing(line_spacing),
              alignment(alignment){
        }

        Messageable &operator<<(const Message &message) override {
            message.value("lineWidth", maxLineWidth);
            message.value("lineSpacing", lineSpacing);
            message.value("alignment", reinterpret_cast<int &>(alignment));
            return *this;
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            maxLineWidth >> message["lineWidth"];
            lineSpacing >> message["lineSpacing"];
            alignment >> message["alignment"];
            return message;
        }
    };
}

#endif //XENGINE_TEXTLAYOUTPARAMETERS_HPP
