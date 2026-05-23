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

    void FTFontRenderer::setPixelSize(const Vec2i &size) {
        FT_Set_Pixel_Sizes(face, size.x, size.y);
    }

    Glyph FTFontRenderer::render(const char32_t c) {
        auto r = FT_Load_Char(face, c, FT_LOAD_RENDER);
        if (r != 0) {
            throw std::runtime_error("Failed to rasterize Glyph " + std::to_string(c) + " " + std::to_string(r));
        }

        Glyph ret;
        ret.character = c;
        ret.metrics.bearing = Vec2i(face->glyph->bitmap_left, face->glyph->bitmap_top);
        ret.metrics.advance = static_cast<int>(face->glyph->advance.x) >> 6;
        ret.metrics.bitmapSize = Vec2i(static_cast<int>(face->glyph->bitmap.width),
                                       static_cast<int>(face->glyph->bitmap.rows));

        const auto bitmap = face->glyph->bitmap;
        const auto pitch = bitmap.pitch;

        if (pitch != 0) {
            const auto absPitch = std::abs(pitch);
            ret.bitmap.resize(ret.metrics.bitmapSize.x * ret.metrics.bitmapSize.y);
            for (auto y = 0; y < ret.metrics.bitmapSize.y; y++) {
                const size_t srcRow = pitch > 0 ? y : (ret.metrics.bitmapSize.y - y - 1);
                const size_t sourceOffset = srcRow * absPitch;
                const size_t dstOffset = y * ret.metrics.bitmapSize.x;
                std::memcpy(ret.bitmap.data() + dstOffset, bitmap.buffer + sourceOffset, ret.metrics.bitmapSize.x);
            }
        } else {
            throw std::runtime_error("Invalid font pitch");
        }

        return ret;
    }

    FontMetrics FTFontRenderer::getFontMetrics() {
        return {
            static_cast<int>(face->ascender * (face->size->metrics.height >> 6)) / face->units_per_EM,
            static_cast<int>(face->descender * (face->size->metrics.height >> 6)) / face->units_per_EM,
            static_cast<int>(face->size->metrics.height >> 6)
        };
    }
}
