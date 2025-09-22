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

#ifndef XENGINE_FREETYPE_HPP
#define XENGINE_FREETYPE_HPP

#include "xng/font/fontengine.hpp"

struct FT_LibraryRec_;

typedef struct FT_LibraryRec_ *FT_Library;

namespace xng::freetype {
    class XENGINE_EXPORT FreeType final : public FontEngine {
    public:
        FreeType();

        ~FreeType() override;

        std::unique_ptr<FontRenderer> createFontRenderer(std::istream &stream) override;

        std::unique_ptr<FontRenderer> createFontRenderer(const Font &data) override;

    private:
        FT_Library library;
    };
}

#endif //XENGINE_FREETYPE_HPP
