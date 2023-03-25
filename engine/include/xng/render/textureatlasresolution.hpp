/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2023  Julian Zampiccoli
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

#ifndef XENGINE_TEXTUREATLASRESOLUTION_HPP
#define XENGINE_TEXTUREATLASRESOLUTION_HPP

namespace xng {
    enum TextureAtlasResolution : int {
        TEXTURE_ATLAS_8x8,
        TEXTURE_ATLAS_16x16,
        TEXTURE_ATLAS_32x32,
        TEXTURE_ATLAS_64x64,
        TEXTURE_ATLAS_128x128,
        TEXTURE_ATLAS_256x256,
        TEXTURE_ATLAS_512x512,
        TEXTURE_ATLAS_1024x1024,
        TEXTURE_ATLAS_2048x2048,
        TEXTURE_ATLAS_4096x4096,
        TEXTURE_ATLAS_8192x8192,
        TEXTURE_ATLAS_16384x16384,
        TEXTURE_ATLAS_END,
        TEXTURE_ATLAS_BEGIN = TEXTURE_ATLAS_8x8
    };
}
#endif //XENGINE_TEXTUREATLASRESOLUTION_HPP
