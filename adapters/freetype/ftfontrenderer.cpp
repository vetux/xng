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

#include "ftfontrenderer.hpp"

#include <cstring>
#include <string>

namespace xng {
    FTFontRenderer::FTFontRenderer(const std::vector<uint8_t> &font, FT_Library library)
        : library(library), bytes(font) {
        auto r = FT_New_Memory_Face(library,
                                    bytes.data(),
                                    static_cast<FT_Long>(bytes.size()),
                                    0,
                                    &face);

        if (r != 0) {
            throw std::runtime_error("Failed to create face from memory " + std::to_string(r));
        }

        FT_Set_Pixel_Sizes(face, 0, 25);
    }

    FTFontRenderer::~FTFontRenderer() {
        FT_Done_Face(face);
    }

    void FTFontRenderer::setPixelSize(Vec2i size) {
        FT_Set_Pixel_Sizes(face, size.x, size.y);
    }

    Character FTFontRenderer::renderAscii(const char c) {
        auto r = FT_Load_Char(face, c, FT_LOAD_RENDER);
        if (r != 0) {
            throw std::runtime_error("Failed to rasterize character " + std::to_string(c) + " " + std::to_string(r));
        }

        Character ret;
        ret.value = c;
        ret.bearing = Vec2i(face->glyph->bitmap_left, face->glyph->bitmap_top);
        ret.advance = static_cast<int>(face->glyph->advance.x) >> 6;
        ret.bitmapSize = Vec2i(static_cast<int>(face->glyph->bitmap.width), static_cast<int>(face->glyph->bitmap.rows));

        const auto bitmap = face->glyph->bitmap;
        const auto pitch = bitmap.pitch;

        if (pitch != 0) {
            const auto absPitch = std::abs(pitch);
            ret.bitmap.resize(ret.bitmapSize.x * ret.bitmapSize.y);
            for (auto y = 0; y < ret.bitmapSize.y; y++) {
                const size_t srcRow = pitch > 0 ? y : (ret.bitmapSize.y - y - 1);
                const size_t sourceOffset = srcRow * absPitch;
                const size_t dstOffset = y * ret.bitmapSize.x;
                std::memcpy(ret.bitmap.data() + dstOffset, bitmap.buffer + sourceOffset, ret.bitmapSize.x);
            }
        } else {
            throw std::runtime_error("Invalid font pitch");
        }

        return ret;
    }

    std::map<char, Character> FTFontRenderer::renderAscii() {
        std::map<char, Character> ret;
        for (int i = 0; i <= 127; i++) {
            ret[static_cast<char>(i)] = std::move(renderAscii(static_cast<char>(i)));
        }
        return ret;
    }

    Character FTFontRenderer::renderUnicode(wchar_t c) {
        throw std::runtime_error("FreeType unicode rendering not implemented yet.");
    }

    int FTFontRenderer::getAscender() {
        return static_cast<int>(face->ascender * (face->size->metrics.height >> 6)) / face->units_per_EM;
    }

    int FTFontRenderer::getDescender() {
        return static_cast<int>(face->descender * (face->size->metrics.height >> 6)) / face->units_per_EM;
    }

    int FTFontRenderer::getHeight() {
        return static_cast<int>(face->size->metrics.height >> 6);
    }
}
