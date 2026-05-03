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

#ifndef XENGINE_FREETYPE_HPP
#define XENGINE_FREETYPE_HPP

#include "xng/font/fontengine.hpp"

struct FT_LibraryRec_;

typedef struct FT_LibraryRec_ *FT_Library;

namespace xng::freetype {
    class XENGINE_EXPORT FontEngine final : public xng::FontEngine {
    public:
        FontEngine();

        ~FontEngine() override;

        std::unique_ptr<FontRenderer> createFontRenderer(const std::vector<uint8_t> &font) override;

    private:
        FT_Library library = nullptr;
    };
}

#endif //XENGINE_FREETYPE_HPP
