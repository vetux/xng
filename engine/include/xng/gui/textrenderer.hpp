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

#ifndef XENGINE_TEXTRENDERER_HPP
#define XENGINE_TEXTRENDERER_HPP

#include "text.hpp"
#include "textlayout.hpp"

#include "xng/render/2d/renderer2d.hpp"

namespace xng {
    /**
     * Known Issues: Because the text renderer returns the rendered text as an Image there can be problems
     * handling the text in real time as the text image size increases because there are currently several cpu operations
     * required to move the text image between the cpu and gpu eg. swapping the columns on download or blitting
     * the text image into an aligned texture atlas image.
     */
    class XENGINE_EXPORT TextRenderer {
    public:
        TextRenderer() = default;

        TextRenderer(Font &font, Renderer2D &renderer2D, const Vec2i &pixelSize);

        ~TextRenderer();

        TextRenderer(const TextRenderer &other);

        TextRenderer &operator=(const TextRenderer &other);

        Vec2i getSize(const std::string &text, const TextLayout &layout);

        Text render(const std::string &text, const TextLayout &layout);

    private:
        Vec2i pixelSize{0, 50};
        std::map<char, Character> ascii;
        std::map<char, Texture2D> textures;

        Font *font = nullptr;
        Renderer2D *ren2d = nullptr;
    };
}
#endif //XENGINE_TEXTRENDERER_HPP
