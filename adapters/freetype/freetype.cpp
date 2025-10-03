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

#include "xng/adapters/freetype/freetype.hpp"

#include "ftfontrenderer.hpp"

namespace xng::freetype {
    FontEngine::FontEngine() {
        auto r = FT_Init_FreeType(&library);
        if (r != 0) {
            throw std::runtime_error("Failed to initalize freetype: " + std::to_string(r));
        }
    }

    FontEngine::~FontEngine() {
        FT_Done_FreeType(library);
    }

    std::unique_ptr<FontRenderer> FontEngine::createFontRenderer(const Font &data) {
        return std::make_unique<FTFontRenderer>(data, library);
    }

    std::unique_ptr<FontRenderer> FontEngine::createFontRenderer(std::istream &stream) {
        return xng::FontEngine::createFontRenderer(stream);
    }
}
