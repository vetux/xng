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

#ifndef XENGINE_TEXTRENDERER_HPP
#define XENGINE_TEXTRENDERER_HPP

#include "text/text.hpp"

#include "render/2d/renderer2d.hpp"

namespace xng {
    class XENGINE_EXPORT TextRenderer {
    public:
        TextRenderer(Font &font, RenderDevice &device);

        void setFontSize(Vec2i pixelSize);

        Vec2f getSize(const std::string &text,
                      int lineHeight,
                      int lineWidth = 0,
                      int lineSpacing = 10);

        /**
         * Render the given text to a texture and return it in a Text object.
         *
         * @param text
         * @param lineHeight The distance between the origins of each line and the distance between the top of the texture and the origin of the first line.
         * @param lineWidth If larger than zero the value in pixels at which a newline is rendered if the widths of characters in the current line exceed it.
         * @param lineSpacing Additional spacing between lines
         * @return
         */
        Text render(const std::string &text,
                    int lineHeight,
                    int lineWidth = 0,
                    int lineSpacing = 10);

    private:
        Vec2i fontSize{0, 50};
        std::map<char, Character> ascii;
        std::map<char, std::unique_ptr<TextureBuffer>> textures;

        Font *font;
        RenderDevice *device = nullptr;

        Renderer2D ren2d;

        std::unique_ptr<RenderTarget> target;
    };
}
#endif //XENGINE_TEXTRENDERER_HPP
