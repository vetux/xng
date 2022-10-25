/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2022  Julian Zampiccoli
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

#include "text/text.hpp"
#include "text/textrenderproperties.hpp"

#include "render/2d/renderer2d.hpp"

namespace xng {
    /**
     * A hardware text renderer.
     */
    class XENGINE_EXPORT TextRenderer {
    public:
        TextRenderer(Font &font, Renderer2D &renderer2D);

        void setFontSize(Vec2i pixelSize);

        Vec2f getSize(const std::string &text, const TextRenderProperties &properties);

        /**
         * @param text
         * @param properties
         * @return
         */
        Text render(const std::string &text, const TextRenderProperties &properties);

    private:
        Vec2i fontSize{0, 50};
        std::map<char, Character> ascii;
        std::map<char, std::unique_ptr<TextureBuffer>> textures;

        Font &font;
        Renderer2D &ren2d;

        std::unique_ptr<RenderTarget> target;
    };
}
#endif //XENGINE_TEXTRENDERER_HPP
