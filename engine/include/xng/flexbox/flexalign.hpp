/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 3 of the License, or (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.

 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef XENGINE_FLEXALIGN_HPP
#define XENGINE_FLEXALIGN_HPP

namespace xng {
    enum FlexAlign : int {
        FLEX_AUTO = 0,
        FLEX_ALIGN_START,
        FLEX_ALIGN_CENTER,
        FLEX_ALIGN_END,
        FLEX_STRETCH,
        FLEX_BASELINE,
        FLEX_ALIGN_SPACE_BETWEEN,
        FLEX_ALIGN_SPACE_AROUND,
    };
}
#endif //XENGINE_FLEXALIGN_HPP
