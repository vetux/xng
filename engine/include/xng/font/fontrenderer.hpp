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

#ifndef XENGINE_FONTRENDERER_HPP
#define XENGINE_FONTRENDERER_HPP

#include "xng/math/vector2.hpp"
#include "xng/font/glyph.hpp"
#include "xng/font/fontmetrics.hpp"

namespace xng {
    /**
     * A software font rendering interface.
     */
    class XENGINE_EXPORT FontRenderer {
    public:
        virtual ~FontRenderer() = default;

        /**
         * Set the requested pixel size.
         *
         * @param size
         */
        virtual void setPixelSize(const Vec2i &size) = 0;

        /**
         * @param c The code point to check.
         * @return True if the font renderer supports rendering the given character.
         */
        virtual bool check(char32_t c) = 0;

        /**
         * Rasterize the given Unicode character.
         *
         * @param c
         * @return
         */
        virtual Glyph render(char32_t c) = 0;

        /**
         * @return The font metrics for the current pixel size.
         */
        virtual FontMetrics getFontMetrics() = 0;
    };
}

#endif //XENGINE_FONTRENDERER_HPP
