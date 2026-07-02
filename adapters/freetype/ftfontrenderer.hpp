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

#ifndef XENGINE_FTFONTRENDERER_HPP
#define XENGINE_FTFONTRENDERER_HPP

#include <ft2build.h>

#include "freetype/freetype.h"

#include "xng/font/fontrenderer.hpp"

namespace xng {
    class FTFontRenderer final : public FontRenderer {
    public:
        FT_Library library{};
        FT_Face face{};
        std::vector<uint8_t> bytes; //Freetype requires the data to stay in memory when loading with FT_New_Memory_Face

        explicit FTFontRenderer(const std::vector<uint8_t> &font, FT_Library library);

        ~FTFontRenderer() override;

        void setPixelSize(const Vec2i &size) override;

        bool check(char32_t c) override;

        Glyph render(char32_t c) override;

        FontMetrics getFontMetrics() override;
    };
}

#endif //XENGINE_FTFONTRENDERER_HPP
