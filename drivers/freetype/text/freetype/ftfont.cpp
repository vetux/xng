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

#include "ftfont.hpp"

#include <string>

namespace xng {
    FTFont::FTFont(std::istream &stream) {
        bytes = std::vector<char>((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());

        FT_Init_FreeType(&library);

        auto r = FT_New_Memory_Face(library,
                                    reinterpret_cast<const FT_Byte *>(bytes.data()),
                                    bytes.size(),
                                    0,
                                    &face);

        if (r != 0) {
            throw std::runtime_error("Failed to create face from memory " + std::to_string(r));
        }

        FT_Set_Pixel_Sizes(face, 0, 25);
    }

    FTFont::~FTFont() {
        FT_Done_Face(face);
    }

    void FTFont::setPixelSize(Vec2i size) {
        FT_Set_Pixel_Sizes(face, size.x, size.y);
    }

    Character FTFont::renderAscii(char c) {
        auto r = FT_Load_Char(face, c, FT_LOAD_RENDER);
        if (r != 0) {
            throw std::runtime_error("Failed to rasterize character " + std::to_string(c) + " " + std::to_string(r));
        }

        Vec2i size(face->glyph->bitmap.width, face->glyph->bitmap.rows);
        Vec2i bearing(face->glyph->bitmap_left, face->glyph->bitmap_top);

        auto bitmap = face->glyph->bitmap;
        auto pitch = bitmap.pitch;

        ImageRGBA buffer(size);
        if (pitch > 0) {
            //Ascending
            auto rowLength = pitch;
            if (size.x != rowLength) {
                throw std::runtime_error("Invalid bitmap format");
            }
            for (int x = 0; x < size.x; x++) {
                for (int y = 0; y < size.y; y++) {
                    auto pixel = bitmap.buffer[size.x * y + x];
                    buffer.setPixel(x, y, {pixel, pixel, pixel, pixel});
                }
            }
        } else if (pitch < 0) {
            //Descending
            auto rowLength = pitch * -1;
            if (size.x != rowLength) {
                throw std::runtime_error("Invalid bitmap format");
            }
            for (int x = 0; x < size.x; x++) {
                for (int y = 0; y < size.y; y++) {
                    auto pixel = bitmap.buffer[size.x * y + x];
                    buffer.setPixel(x, size.y - y, {pixel, pixel, pixel, pixel});
                }
            }
        } else {
            //No pitch
            buffer = ImageRGBA();
        }

        int advanceX = numeric_cast<int>(face->glyph->advance.x) >> 6;

        return std::move(Character(c, std::move(buffer), bearing, advanceX));
    }

    std::map<char, Character> FTFont::renderAscii() {
        std::map<char, Character> ret;
        for (int i = 0; i <= 127; i++) {
            ret[numeric_cast<char>(i)] = std::move(renderAscii(numeric_cast<char>(i)));
        }
        return ret;
    }

    Character FTFont::renderUnicode(wchar_t c) {
        throw std::runtime_error("FreeType unicode rendering not implemented yet.");
    }
}