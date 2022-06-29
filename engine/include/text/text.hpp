/**
 *  xEngine - C++ game engine library
 *  Copyright (C) 2021  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef XENGINE_TEXT_HPP
#define XENGINE_TEXT_HPP

#include "text/font.hpp"
#include "cast/numeric_cast.hpp"

#include "graphics/renderdevice.hpp"

namespace xng {
    struct XENGINE_EXPORT Text {
        Text() = default;

        Text(std::string text, Vec2f origin, int lineWidth, std::unique_ptr<TextureBuffer> buffer)
                : text(std::move(text)),
                  origin(origin),
                  lineWidth(lineWidth),
                  buffer(std::move(buffer)) {}

        ~Text() = default;

        /**
         * @return
         */
        const std::string &getText() const { return text; }

        const Vec2f &getOrigin() const { return origin; }

        /**
         * @return
         */
        int getLineWidth() const { return lineWidth; }

        /**
         * Get the texture containing the rendered text with the grayscale in the x component.
         *
         * @return
         */
        TextureBuffer &getTexture() const { return *buffer; }

    private:
        std::string text;
        Vec2f origin;
        int lineWidth{};
        std::unique_ptr<TextureBuffer> buffer;
    };
}
#endif //XENGINE_TEXT_HPP
