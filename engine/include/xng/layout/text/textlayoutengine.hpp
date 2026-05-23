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
        static Vec2i getSize(const std::u32string &text,
                             const TextLayoutParameters &layoutParameters,
                             const FontMetrics &fontMetrics,
                             const std::unordered_map<char32_t, Glyph::Metrics> &glyphMetrics);

        static TextLayout getLayout(const std::u32string &text,
                                    const TextLayoutParameters &layoutParameters,
                                    const FontMetrics &fontMetrics,
                                    const std::unordered_map<char32_t, Glyph::Metrics> &glyphMetrics);
    };
}

#endif //XENGINE_TEXTRENDERER_HPP
