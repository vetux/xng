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

#ifndef XENGINE_TEXTRENDERER_HPP
#define XENGINE_TEXTRENDERER_HPP

#include "text.hpp"
#include "textlayout.hpp"

#include "xng/render/2d/renderer2d.hpp"
#include "xng/rendergraph/rendergraphruntime.hpp"

namespace xng {
    /**
     * Renders text to Texture2D textures.
     */
    class XENGINE_EXPORT TextRenderer {
    public:
        TextRenderer(FontRenderer& font,
                     std::shared_ptr<Renderer2D> ren2D,
                     const Vec2i &pixelSize);

        ~TextRenderer();

        Vec2i getSize(const std::string &text, const TextLayout &layout) const;

        Text render(const std::string &text, const TextLayout &layout);

    private:
        std::shared_ptr<Renderer2D> ren2d;

        Vec2i pixelSize{0, 50};
        std::map<char, Character> ascii;
        std::map<char, Texture2D> textures;
    };
}
#endif //XENGINE_TEXTRENDERER_HPP
