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

#ifndef XENGINE_TEXT_HPP
#define XENGINE_TEXT_HPP

#include <utility>

#include "xng/font/fontrenderer.hpp"
#include "xng/render/2d/texture2d.hpp"
#include "xng/render/2d/text/textlayout.hpp"

namespace xng {
    class Renderer2D;

    struct XENGINE_EXPORT Text {
        Text(std::string text,
             Vec2f origin,
             const TextLayout layout,
             Texture2D texture,
             Renderer2D &renderer)
            : text(std::move(text)),
              origin(std::move(origin)),
              layout(layout),
              texture(std::move(texture)),
              renderer(renderer) {
        }

        ~Text();

        Text(const Text &other) = delete;

        Text &operator=(const Text &other) = delete;

        Text(Text &&other) noexcept = delete;

        Text &operator=(Text &&other) noexcept = delete;

        /**
         * @return
         */
        const std::string &getText() const { return text; }

        const Vec2f &getOrigin() const { return origin; }

        /**
         * @return
         */
        TextLayout getLayout() const { return layout; }

        const Texture2D &getTexture() const { return texture; }

    private:
        std::string text;
        Vec2f origin;
        TextLayout layout{};
        Texture2D texture;
        Renderer2D &renderer;
    };
}
#endif //XENGINE_TEXT_HPP
