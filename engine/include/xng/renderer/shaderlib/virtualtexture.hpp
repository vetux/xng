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
    //TODO: Design solution for buffer / texture arguments

    // When passing the shader objects directly in the function signature the buffer / texture names are inlined in
    // the first call to the C++ side function and not regenerated in each call site. This means only the first invocation
    // of a function with the passed arguments is created all other invocations trying to pass different buffers / textures
    // would use the inlined buffer / texture from the first invocation.

    // When textures have non power of 2 tile count in any dimension the residency / readback still works out.

    // For residency, we store at the finest level the resident mips per tile, and higher mips are always at least half coverage
    // of the finest level, and any tile in the finest mip level is already the finest granularity possible.

    // For readback we store the mip tile index directly so the cpu can always reconstruct the covering mip 0 tiles
    // and write per mip0 tile the highest resident mip. Each higher mip tile can cover multiple mip0 tiles.

    using namespace ShaderScript;

    /**
     * Sample the virtual texture.
     *
     * The tile map contains the atlas tile indices for each mip map with the rows laid out linearly like so:
     * [x = 0, y = 0][x = 1,  y = 0]...
     *
     * The atlas tile indices point to a tile across layers by using the same linear row interpretation.
     *
     * The atlas tile is selected by simply dividing by the number of tiles in a single atlas layer and using the remainder
     * to index into the tiles of the layer to get the target tile.
     *
     * The full equation becomes:
     *
     * tileX = mipTexelX / tileSize
     * tileY = mipTexelY / tileSize
     * tilesPerRow = ceil(imageMipWidth / tileSize)
     * imageTileIndex = tileY * tilesPerRow + tileX
     *
     * tileOffset = tileMapOffsets[textureID + mip]
     * atlasTileIndex = tileMap[tileOffset + imageTileIndex]
     *
     * atlasTileSize = tileSize + 2*tileBorder
     * tilesPerLayer = (atlasSize / atlasTileSize)^2.
     *
     * layer = atlasTileIndex / tilesPerLayer
     * localAtlasTileIndex = atlasTileIndex - layer * tilesPerLayer
     *
     * -- Residency Map --
     * The residency map contains the lowest mip level currently resident per tile per mip 0 of each texture.
     * This means the streamer must guarantee that the full mip chain from maxMip to residentMip is kept allocated per tile.
     *
     * It uses the same linear row interpretation as the tile map.
     *
     * It is indexed via residencyMapOffsets like so:
     *
     * tileX = mip0TexelX / tileSize
     * tileY = mip0TexelY / tileSize
     * tilesPerRow = ceil(mip0Width / tileSize)
     * mip0TileIndex = tileY * tilesPerRow + tileX
     *
     * tileOffset = residencyMapOffsets[textureID]
     * lowestMipResident = residencyMap[tileOffset + mip0TileIndex]
     *
     * -- Tile Borders --
     * Each tile in the atlas texture has an additional border which contains the neighboring virtual tile
     * data to allow hardware linear / anisotropic filtering to sample across the edges.
     *
     * The borders must be sized to at least: ceil(maxAnisotropy / 2) + 1.
     *
     * For WRAP_REPEAT the borders of the tiles at image boundaries contain the repeated image texels
     * For WRAP_CLAMP_EDGE the borders of the tiles at image boundaries contain copies of the edge texels.
     *
     * @param textureID The ID of the texture / Base Index into tile map offsets.
     * @param uv The uv in virtual texture space.
     * @param wrap The WrappingMethod to use. (The atlas tiles must already contain borders matching this method)
     * @param minFilter The minification filter to use.
     * @param magFilter The magnification filter to use.
     * @param mipFilter The mip filter to use.
     * @param imageSize The size of the virtual image.
     * @param imageMaxMip The index of the maximum mip allocated for this image.
     * @param atlasSize The texel count of one dimension in the rectangular atlas texture.
     * @param tileSize The texel count of one dimension of the rectangular tiles.
     * @param tileBorder The texel count of the border on one side.
     * @param maxAnisotropy The maximum anisotropy level set on the hardware sampler.
     * @param tileMapOffsets The dynamic buffer containing the tile map offsets.
     * @param tileMap The dynamic buffer containing the tile maps.
     * @param residencyMapOffsets The offsets into residencyMap indexed by textureID
     * @param residencyMap The residency map
     * @param sampler The array texture containing the tiles.
     * @return
     */
    XENGINE_EXPORT vec4 sample_virtual(Param<UInt> textureID,
                                       Param<vec2> uv,
                                       Param<Int> wrap,
                                       Param<Int> minFilter,
                                       Param<Int> magFilter,
                                       Param<Int> mipFilter,
                                       Param<ivec2> imageSize,
                                       Param<UInt> imageMaxMip,
                                       Param<UInt> atlasSize,
                                       Param<UInt> tileSize,
                                       Param<UInt> tileBorder,
                                       Param<Float> maxAnisotropy,
                                       DynamicBufferWrapper<UInt> &tileMapOffsets,
                                       DynamicBufferWrapper<UInt> &tileMap,
                                       DynamicBufferWrapper<UInt> &residencyMapOffsets,
                                       DynamicBufferWrapper<UInt> &residencyMap,
                                       ShaderObject &sampler);

    XENGINE_EXPORT vec4 sample_nearest(Param<UInt> textureID,
                                       Param<vec2> uv,
                                       Param<Int> wrap,
                                       Param<ivec2> imageSize,
                                       Param<UInt> imageMaxMip,
                                       Param<UInt> atlasSize,
                                       Param<UInt> tileSize,
                                       Param<UInt> tileBorder,
                                       Param<Float> maxAnisotropy,
                                       Param<vec2> dx,
                                       Param<vec2> dy,
                                       DynamicBufferWrapper<UInt> &tileMapOffsets,
                                       DynamicBufferWrapper<UInt> &tileMap,
                                       DynamicBufferWrapper<UInt> &residencyMapOffsets,
                                       DynamicBufferWrapper<UInt> &residencyMap,
                                       ShaderObject &sampler);

    XENGINE_EXPORT vec4 sample_nearest_linear(Param<UInt> textureID,
                                              Param<vec2> uv,
                                              Param<Int> wrap,
                                              Param<ivec2> imageSize,
                                              Param<UInt> imageMaxMip,
                                              Param<UInt> atlasSize,
                                              Param<UInt> tileSize,
                                              Param<UInt> tileBorder,
                                              Param<Float> maxAnisotropy,
                                              Param<vec2> dx,
                                              Param<vec2> dy,
                                              DynamicBufferWrapper<UInt> &tileMapOffsets,
                                              DynamicBufferWrapper<UInt> &tileMap,
                                              DynamicBufferWrapper<UInt> &residencyMapOffsets,
                                              DynamicBufferWrapper<UInt> &residencyMap,
                                              ShaderObject &sampler);

    XENGINE_EXPORT vec4 sample_bilinear(Param<UInt> textureID,
                                        Param<vec2> uv,
                                        Param<Int> wrap,
                                        Param<ivec2> imageSize,
                                        Param<UInt> imageMaxMip,
                                        Param<UInt> atlasSize,
                                        Param<UInt> tileSize,
                                        Param<UInt> tileBorder,
                                        Param<Float> maxAnisotropy,
                                        Param<vec2> dx,
                                        Param<vec2> dy,
                                        DynamicBufferWrapper<UInt> &tileMapOffsets,
                                        DynamicBufferWrapper<UInt> &tileMap,
                                        DynamicBufferWrapper<UInt> &residencyMapOffsets,
                                        DynamicBufferWrapper<UInt> &residencyMap,
                                        ShaderObject &sampler);

    XENGINE_EXPORT vec4 sample_trilinear(Param<UInt> textureID,
                                         Param<vec2> uv,
                                         Param<Int> wrap,
                                         Param<ivec2> imageSize,
                                         Param<UInt> imageMaxMip,
                                         Param<UInt> atlasSize,
                                         Param<UInt> tileSize,
                                         Param<UInt> tileBorder,
                                         Param<Float> maxAnisotropy,
                                         Param<vec2> dx,
                                         Param<vec2> dy,
                                         DynamicBufferWrapper<UInt> &tileMapOffsets,
                                         DynamicBufferWrapper<UInt> &tileMap,
                                         DynamicBufferWrapper<UInt> &residencyMapOffsets,
                                         DynamicBufferWrapper<UInt> &residencyMap,
                                         ShaderObject &sampler);

    XENGINE_EXPORT vec4 sample_bicubic(Param<UInt> textureID,
                                       Param<vec2> uv,
                                       Param<Int> wrap,
                                       Param<ivec2> imageSize,
                                       Param<UInt> imageMaxMip,
                                       Param<UInt> atlasSize,
                                       Param<UInt> tileSize,
                                       Param<UInt> tileBorder,
                                       Param<Float> maxAnisotropy,
                                       Param<vec2> dx,
                                       Param<vec2> dy,
                                       DynamicBufferWrapper<UInt> &tileMapOffsets,
                                       DynamicBufferWrapper<UInt> &tileMap,
                                       DynamicBufferWrapper<UInt> &residencyMapOffsets,
                                       DynamicBufferWrapper<UInt> &residencyMap,
                                       ShaderObject &sampler);

    XENGINE_EXPORT vec4 sample_bicubic_trilinear(Param<UInt> textureID,
                                                 Param<vec2> uv,
                                                 Param<Int> wrap,
                                                 Param<ivec2> imageSize,
                                                 Param<UInt> imageMaxMip,
                                                 Param<UInt> atlasSize,
                                                 Param<UInt> tileSize,
                                                 Param<UInt> tileBorder,
                                                 Param<Float> maxAnisotropy,
                                                 Param<vec2> dx,
                                                 Param<vec2> dy,
                                                 DynamicBufferWrapper<UInt> &tileMapOffsets,
                                                 DynamicBufferWrapper<UInt> &tileMap,
                                                 DynamicBufferWrapper<UInt> &residencyMapOffsets,
                                                 DynamicBufferWrapper<UInt> &residencyMap,
                                                 ShaderObject &sampler);

    /**
     * Perform readback of the sampled tiles.
     *
     * The returned value holds the number of tapped tiles.
     *
     * The readback buffer is in the same format and size as the tileMap buffer,
     * but it stores the number of taps for each virtual tile instead of the atlas tile index.
     *
     * @param textureID
     * @param uv
     * @param wrap
     * @param minFilter
     * @param magFilter
     * @param mipFilter
     * @param imageSize
     * @param imageMaxMip
     * @param tileSize
     * @param maxAnisotropy
     * @param readbackOffsets
     * @param readback
     * @return The number of taps
     */
    XENGINE_EXPORT UInt readback_sample(Param<UInt> textureID,
                                        Param<vec2> uv,
                                        Param<Int> wrap,
                                        Param<Int> minFilter,
                                        Param<Int> magFilter,
                                        Param<Int> mipFilter,
                                        Param<ivec2> imageSize,
                                        Param<UInt> imageMaxMip,
                                        Param<UInt> tileSize,
                                        Param<Float> maxAnisotropy,
                                        DynamicBufferWrapper<UInt> &readbackOffsets,
                                        DynamicBufferWrapper<UInt> &readback);

    XENGINE_EXPORT UInt readback_sample_nearest(Param<UInt> textureID,
                                                Param<vec2> uv,
                                                Param<Int> wrap,
                                                Param<ivec2> imageSize,
                                                Param<UInt> imageMaxMip,
                                                Param<UInt> tileSize,
                                                Param<Float> maxAnisotropy,
                                                Param<vec2> dx,
                                                Param<vec2> dy,
                                                DynamicBufferWrapper<UInt> &readbackOffsets,
                                                DynamicBufferWrapper<UInt> &readback);

    XENGINE_EXPORT UInt readback_sample_nearest_linear(Param<UInt> textureID,
                                                       Param<vec2> uv,
                                                       Param<Int> wrap,
                                                       Param<ivec2> imageSize,
                                                       Param<UInt> imageMaxMip,
                                                       Param<UInt> tileSize,
                                                       Param<Float> maxAnisotropy,
                                                       Param<vec2> dx,
                                                       Param<vec2> dy,
                                                       DynamicBufferWrapper<UInt> &readbackOffsets,
                                                       DynamicBufferWrapper<UInt> &readback);

    XENGINE_EXPORT UInt readback_sample_bilinear(Param<UInt> textureID,
                                                 Param<vec2> uv,
                                                 Param<Int> wrap,
                                                 Param<ivec2> imageSize,
                                                 Param<UInt> imageMaxMip,
                                                 Param<UInt> tileSize,
                                                 Param<Float> maxAnisotropy,
                                                 Param<vec2> dx,
                                                 Param<vec2> dy,
                                                 DynamicBufferWrapper<UInt> &readbackOffsets,
                                                 DynamicBufferWrapper<UInt> &readback);

    XENGINE_EXPORT UInt readback_sample_trilinear(Param<UInt> textureID,
                                                  Param<vec2> uv,
                                                  Param<Int> wrap,
                                                  Param<ivec2> imageSize,
                                                  Param<UInt> imageMaxMip,
                                                  Param<UInt> tileSize,
                                                  Param<Float> maxAnisotropy,
                                                  Param<vec2> dx,
                                                  Param<vec2> dy,
                                                  DynamicBufferWrapper<UInt> &readbackOffsets,
                                                  DynamicBufferWrapper<UInt> &readback);

    XENGINE_EXPORT UInt readback_sample_bicubic(Param<UInt> textureID,
                                                Param<vec2> uv,
                                                Param<Int> wrap,
                                                Param<ivec2> imageSize,
                                                Param<UInt> imageMaxMip,
                                                Param<UInt> tileSize,
                                                Param<Float> maxAnisotropy,
                                                Param<vec2> dx,
                                                Param<vec2> dy,
                                                DynamicBufferWrapper<UInt> &readbackOffsets,
                                                DynamicBufferWrapper<UInt> &readback);

    XENGINE_EXPORT UInt readback_sample_bicubic_trilinear(Param<UInt> textureID,
                                                          Param<vec2> uv,
                                                          Param<Int> wrap,
                                                          Param<ivec2> imageSize,
                                                          Param<UInt> imageMaxMip,
                                                          Param<UInt> tileSize,
                                                          Param<Float> maxAnisotropy,
                                                          Param<vec2> dx,
                                                          Param<vec2> dy,
                                                          DynamicBufferWrapper<UInt> &readbackOffsets,
                                                          DynamicBufferWrapper<UInt> &readback);
}

#endif //XENGINE_VIRTUALTEXTURE_HPP
