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

#ifndef XENGINE_FTFONTRENDERER_HPP
#define XENGINE_FTFONTRENDERER_HPP

#include <ft2build.h>

#include "freetype/freetype.h"

#include "xng/font/fontrenderer.hpp"

namespace xng {
    class FTFontRenderer : public FontRenderer {
    public:
        FT_Library library{};
        FT_Face face{};
        std::vector<char> bytes; //Freetype requires the data to stay in memory when loading with FT_New_Memory_Face

        explicit FTFontRenderer(const Font &font, FT_Library library);

        ~FTFontRenderer() override;

        void setPixelSize(Vec2i size) override;

        Character renderAscii(char c) override;

        std::map<char, Character> renderAscii() override;

        Character renderUnicode(wchar_t c) override;

        int getAscender() override;

        int getDescender() override;

        int getHeight() override;
    };
}

#endif //XENGINE_FTFONTRENDERER_HPP
