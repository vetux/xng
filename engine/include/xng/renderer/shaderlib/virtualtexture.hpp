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

#ifndef XENGINE_VIRTUALTEXTURE_HPP
#define XENGINE_VIRTUALTEXTURE_HPP

#include "xng/shaderscript/shaderscript.hpp"

namespace xng::shaderlib::virtualtexture {
    using namespace ShaderScript;

    XENGINE_EXPORT ivec3 getAtlasTexel(Param<UInt> textureID,
                                       Param<vec2> uv,
                                       Param<Int> mip,
                                       Param<Int> wrap,
                                       Param<ivec2> imageSize,
                                       Param<vec2> mipSize,
                                       Param<UInt> atlasSize,
                                       Param<UInt> tileSize,
                                       Param<UInt> tileBorder,
                                       DynamicBufferWrapper<UInt> &tileMapOffsets,
                                       DynamicBufferWrapper<UInt> &tileMap);

    XENGINE_EXPORT vec3 getAtlasUV(Param<UInt> textureID,
                                   Param<vec2> uv,
                                   Param<Int> mip,
                                   Param<Int> wrap,
                                   Param<ivec2> imageSize,
                                   Param<vec2> mipSize,
                                   Param<UInt> atlasSize,
                                   Param<UInt> tileSize,
                                   Param<UInt> tileBorder,
                                   DynamicBufferWrapper<UInt> &tileMapOffsets,
                                   DynamicBufferWrapper<UInt> &tileMap);

    /**
     * Sample the virtual texture using nearest-filtering.
     *
     * tileOffset = tileMapOffsets[textureID + mip]
     *
     * atlasTileSize = tileSize + 2*tileBorder
     *
     * The border of the tiles contains the neighboring tile data for correct hardware linear / anisotropic filtering.
     *
     * @param textureID The ID of the texture / Base Index into tile map offsets.
     * @param uv The uv in virtual texture space.
     * @param wrap The WrappingMethod to use.
     * @param imageSize The size of the virtual image.
     * @param atlasSize The texel count of one dimension in the rectangular atlas texture.
     * @param tileSize The texel count of one dimension of the rectangular tiles.
     * @param tileBorder The texel count of the border on one side.
     * @param tileMapOffsets The dynamic buffer containing the tile map offsets.
     * @param tileMap The dynamic buffer containing the tile maps.
     * @param sampler The array texture containing the tiles.
     * @return
     */
    XENGINE_EXPORT vec4 sample_nearest(Param<UInt> textureID,
                                       Param<vec2> uv,
                                       Param<Int> wrap,
                                       Param<ivec2> imageSize,
                                       Param<UInt> atlasSize,
                                       Param<UInt> tileSize,
                                       Param<UInt> tileBorder,
                                       DynamicBufferWrapper<UInt> &tileMapOffsets,
                                       DynamicBufferWrapper<UInt> &tileMap,
                                       ShaderObject &sampler);

    /**
     * Sample the virtual texture using hardware bilinear filtering.
     *
     * @param textureID
     * @param uv
     * @param wrap
     * @param imageSize
     * @param atlasSize
     * @param tileSize
     * @param tileBorder
     * @param tileMapOffsets
     * @param tileMap
     * @param sampler
     * @return
     */
    XENGINE_EXPORT vec4 sample_bilinear(Param<UInt> textureID,
                                        Param<vec2> uv,
                                        Param<Int> wrap,
                                        Param<ivec2> imageSize,
                                        Param<UInt> atlasSize,
                                        Param<UInt> tileSize,
                                        Param<UInt> tileBorder,
                                        DynamicBufferWrapper<UInt> &tileMapOffsets,
                                        DynamicBufferWrapper<UInt> &tileMap,
                                        ShaderObject &sampler);

    /**
     * Sample the virtual texture using bicubic filtering.
     *
     * 16-tap bicubic (Catmull-Rom) approximated with 4 hardware bilinear samples
     *
     * @param textureID
     * @param uv
     * @param wrap
     * @param imageSize
     * @param atlasSize
     * @param tileSize
     * @param tileBorder
     * @param tileMapOffsets
     * @param tileMap
     * @param sampler
     * @return
     */
    XENGINE_EXPORT vec4 sample_bicubic(Param<UInt> textureID,
                                       Param<vec2> uv,
                                       Param<Int> wrap,
                                       Param<ivec2> imageSize,
                                       Param<UInt> atlasSize,
                                       Param<UInt> tileSize,
                                       Param<UInt> tileBorder,
                                       DynamicBufferWrapper<UInt> &tileMapOffsets,
                                       DynamicBufferWrapper<UInt> &tileMap,
                                       ShaderObject &sampler);
}

#endif //XENGINE_VIRTUALTEXTURE_HPP
