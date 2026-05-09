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

#ifndef XENGINE_TEXTURERESOLUTION_HPP
#define XENGINE_TEXTURERESOLUTION_HPP

namespace xng {
    enum TextureResolution : int {
        RESOLUTION_8x8 = 0,
        RESOLUTION_16x16,
        RESOLUTION_32x32,
        RESOLUTION_64x64,
        RESOLUTION_128x128,
        RESOLUTION_256x256,
        RESOLUTION_512x512,
        RESOLUTION_1024x1024,
        RESOLUTION_2048x2048,
        RESOLUTION_4096x4096,
        RESOLUTION_8192x8192,
        RESOLUTION_16384x16384,
        RESOLUTION_END,
        RESOLUTION_BEGIN = RESOLUTION_8x8
    };
}

#endif //XENGINE_TEXTURERESOLUTION_HPP
