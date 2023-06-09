/**
 *  xEngine - C++ Game Engine Library
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

#ifndef XENGINE_TEXT_HPP
#define XENGINE_TEXT_HPP

#include <utility>

#include "xng/font/font.hpp"

#include "xng/gpu/renderdevice.hpp"

namespace xng {
    struct XENGINE_EXPORT Text {
        Text() = default;

        Text(std::string text, Vec2f origin, int lineWidth, ImageRGBA buffer)
                : text(std::move(text)),
                  origin(std::move(origin)),
                  lineWidth(lineWidth),
                  image(std::move(buffer)) {}

        ~Text() = default;

        Text(const Text &other) = default;

        Text &operator=(const Text& other) = default;

        Text(Text &&other) = default;

        Text &operator=(Text &&other) = default;

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
         * Get the image containing the rendered text with the grayscale in the r,g,b,a components.
         *
         * @return
         */
        const ImageRGBA &getImage() const { return image; }

    private:
        std::string text;
        Vec2f origin;
        int lineWidth{};
        ImageRGBA image;
    };
}
#endif //XENGINE_TEXT_HPP
