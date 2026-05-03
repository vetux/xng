/**
 *   xEngine - C++ Game Engine Library
 *   Copyright (C) 2026 Julia Zampiccoli
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the Lesser General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef XENGINE_TEXTRENDERER_HPP
#define XENGINE_TEXTRENDERER_HPP

#include "xng/assets/font.hpp"
#include "xng/layout/text/textlayout.hpp"
#include "xng/layout/text/textlayoutparameters.hpp"
#include "xng/resource/resourcehandle.hpp"
#include "xng/font/fontengine.hpp"

namespace xng {
    class XENGINE_EXPORT TextLayoutEngine {
    public:
        explicit TextLayoutEngine(FontEngine &fontEngine,
                                  const ResourceHandle<Font> &font,
                                  const Vec2i &fontPixelSize);

        [[nodiscard]] Vec2i getSize(const std::string &text, const TextLayoutParameters &layoutParameters) const;

        [[nodiscard]] TextLayout getLayout(const std::string &text, const TextLayoutParameters &layoutParameters) const;

    private:
        Uri fontUri;
        Vec2i fontPixelSize{};

        int ascender = 0;
        int descender = 0;
        int lineHeight = 0;
        std::map<char, std::shared_ptr<Character>> ascii;
    };
}

#endif //XENGINE_TEXTRENDERER_HPP
