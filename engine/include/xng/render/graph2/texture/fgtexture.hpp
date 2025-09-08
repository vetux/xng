/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2023  Julian Zampiccoli
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

#ifndef XENGINE_FGTEXTURE_HPP
#define XENGINE_FGTEXTURE_HPP

#include "xng/render/graph2/texture/fgtextureproperties.hpp"

#include "xng/render/scene/color.hpp"

using namespace xng::graph;

namespace xng {
    struct FGTexture {
        Vec2i size = {1, 1};
        FGTextureType textureType = TEXTURE_2D;
        FGColorFormat format = RGBA;

        FGTextureWrapping wrapping = CLAMP_TO_BORDER;
        FGTextureFiltering filterMin = NEAREST;
        FGTextureFiltering filterMag = NEAREST;

        int samples = 1; //Ignored if texture is not TEXTURE_2D_MULTISAMPLE
        int mipMapLevels = 1;
        FGMipMapFiltering mipMapFilter = NEAREST_MIPMAP_LINEAR;
        bool fixedSampleLocations = false;

        ColorRGBA borderColor = ColorRGBA(0);

        size_t arrayLayers = 1; // If > 1 this texture becomes an array texture
    };
}

#endif //XENGINE_FGTEXTURE_HPP