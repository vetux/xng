/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
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

#include "xng/graphics/text/textlayout.hpp"
#include "xng/graphics/text/textlayoutparameters.hpp"
#include "xng/font/fontengine.hpp"
#include "xng/resource/resourcehandle.hpp"

namespace xng {
    class XENGINE_EXPORT TextLayoutEngine {
    public:
        explicit TextLayoutEngine(FontEngine &fontEngine,
                                  const ResourceHandle<Font> &font,
                                  const Vec2i &fontPixelSize);

        Vec2i getSize(const std::string &text, const TextLayoutParameters &layoutParameters) const;

        TextLayout getLayout(const std::string &text, const TextLayoutParameters &layoutParameters) const;

    private:
        Uri fontUri;
        Vec2i fontPixelSize{};

        int ascender = 0;
        int descender = 0;
        int lineHeight = 0;
        std::map<char, Character> ascii;
    };
}

#endif //XENGINE_TEXTRENDERER_HPP
