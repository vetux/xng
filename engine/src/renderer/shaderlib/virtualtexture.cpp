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

#include "xng/renderer/shaderlib/virtualtexture.hpp"
#include "xng/shaderscript/macro/helpermacros.hpp"
#include "xng/renderer/samplingproperties.hpp"

namespace xng::shaderlib::virtualtexture {
    ivec3 getAtlasTexel(Param<UInt> textureID,
                        Param<vec2> uv,
                        Param<Int> mip,
                        Param<Int> wrap,
                        Param<ivec2> imageSize,
                        Param<vec2> mipSize,
                        Param<UInt> atlasSize,
                        Param<UInt> tileSize,
                        Param<UInt> tileBorder,
                        DynamicBufferWrapper<UInt> &tileMapOffsets,
                        DynamicBufferWrapper<UInt> &tileMap) {
        IRFunction

        vec2 wrapped;
        If(wrap == WRAP_REPEAT)
            wrapped = fract(uv);
        Else
            wrapped = clamp(uv, vec2(0.0f), vec2(1.0f));
        Fi

        ivec2 texel = ivec2(wrapped * mipSize);

        uvec2 tile = uvec2(texel) / tileSize;
        uvec2 tiles = (imageSize + tileSize - 1) / tileSize;

        UInt tileMapBaseOffset = tileMapOffsets[textureID + mip];
        UInt slot = tileMap[tileMapBaseOffset + tile.y() * tiles.x() + tile.x()];

        ivec2 local_texel = texel - ivec2(tile) * Int(tileSize);

        UInt paddedSize = tileSize + 2u * tileBorder;
        UInt atlas_tiles_row = atlasSize / paddedSize;
        UInt tiles_per_layer = atlas_tiles_row * atlas_tiles_row;

        UInt layer = slot / tiles_per_layer;
        UInt local_slot = slot - layer * tiles_per_layer;
        UInt aty = local_slot / atlas_tiles_row;
        UInt atx = local_slot - aty * atlas_tiles_row;

        IRReturn(ivec3(
            Int(atx) * Int(paddedSize) + Int(tileBorder) + local_texel.x(),
            Int(aty) * Int(paddedSize) + Int(tileBorder) + local_texel.y(),
            Int(layer)
        ));

        IRFunctionEnd
    }

    vec3 getAtlasUV(Param<UInt> textureID,
                    Param<vec2> uv,
                    Param<Int> mip,
                    Param<Int> wrap,
                    Param<ivec2> imageSize,
                    Param<vec2> mipSize,
                    Param<UInt> atlasSize,
                    Param<UInt> tileSize,
                    Param<UInt> tileBorder,
                    DynamicBufferWrapper<UInt> &tileMapOffsets,
                    DynamicBufferWrapper<UInt> &tileMap) {
        IRFunction

        vec2 wrapped;
        If(wrap == WRAP_REPEAT)
            wrapped = fract(uv);
        Else
            wrapped = clamp(uv, vec2(0.0f), vec2(1.0f));
        Fi

        vec2 texel = wrapped * mipSize;
        uvec2 tile = uvec2(texel) / tileSize;
        uvec2 tiles = (imageSize + tileSize - 1) / tileSize;

        UInt tileMapBaseOffset = tileMapOffsets[textureID + mip];
        UInt slot = tileMap[tileMapBaseOffset + tile.y() * tiles.x() + tile.x()];

        UInt paddedSize = tileSize + 2u * tileBorder;
        UInt atlas_tiles_row = atlasSize / paddedSize;
        UInt tiles_per_layer = atlas_tiles_row * atlas_tiles_row;

        UInt layer = slot / tiles_per_layer;
        UInt local_slot = slot - layer * tiles_per_layer;
        UInt aty = local_slot / atlas_tiles_row;
        UInt atx = local_slot - aty * atlas_tiles_row;

        vec2 localUV = mod(texel, Float(tileSize));
        vec2 atlasUV = (vec2(atx, aty) * Float(paddedSize) + Float(tileBorder) + localUV) / Float(atlasSize);

        IRReturn(vec3(atlasUV, Float(layer)));

        IRFunctionEnd
    }

    /**
     * Sample the virtual texture using nearest-filtering.
     *
     * @param textureID The ID of the texture / Base Index into tile map offsets. tileOffset = tileMapOffsets[textureID + mip]
     * @param uv The uv in virtual texture space.
     * @param wrap The wrapping mode to use.
     * @param imageSize The size of the virtual image.
     * @param atlasSize The texel count of one dimension in the rectangular atlas texture.
     * @param tileSize The texel count of one dimension of the rectangular tiles.
     * @param tileBorder The texel count of the border on one side. (atlasTileSize = tileSize + 2*tileBorder)
     * @param tileMapOffsets The dynamic buffer containing the tile map offsets.
     * @param tileMap The dynamic buffer containing the tile maps.
     * @param sampler The array texture containing the tiles.
     * @return
     */
    vec4 sample_nearest(Param<UInt> textureID,
                        Param<vec2> uv,
                        Param<Int> wrap,
                        Param<ivec2> imageSize,
                        Param<UInt> atlasSize,
                        Param<UInt> tileSize,
                        Param<UInt> tileBorder,
                        DynamicBufferWrapper<UInt> &tileMapOffsets,
                        DynamicBufferWrapper<UInt> &tileMap,
                        ShaderObject &sampler) {
        IRFunction

        Float maxMip = log2(Float(min(imageSize.value().x(), imageSize.value().y())));

        vec2 dx = partialDerivativeX(uv) * vec2(imageSize);
        vec2 dy = partialDerivativeY(uv) * vec2(imageSize);
        Float mip = clamp(floor(0.5f * log2(max(dot(dx, dx), dot(dy, dy)))), 0.0f, maxMip);

        vec2 mipSize = max(vec2(1.0f), vec2(imageSize) / pow(2.0f, mip));

        ivec3 atlasUV = getAtlasTexel(textureID,
                                     uv,
                                     Int(mip),
                                     wrap,
                                     imageSize,
                                     mipSize,
                                     atlasSize,
                                     tileSize,
                                     tileBorder,
                                     tileMapOffsets,
                                     tileMap);

        IRReturn(vec4(texelFetchArray(sampler, atlasUV.xy(), atlasUV.z())));

        IRFunctionEnd
    }

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
    vec4 sample_bilinear(Param<UInt> textureID,
                         Param<vec2> uv,
                         Param<Int> wrap,
                         Param<ivec2> imageSize,
                         Param<UInt> atlasSize,
                         Param<UInt> tileSize,
                         Param<UInt> tileBorder,
                         DynamicBufferWrapper<UInt> &tileMapOffsets,
                         DynamicBufferWrapper<UInt> &tileMap,
                         ShaderObject &sampler) {
        IRFunction

        Float maxMip = log2(Float(min(imageSize.value().x(), imageSize.value().y())));

        vec2 dx = partialDerivativeX(uv) * vec2(imageSize);
        vec2 dy = partialDerivativeY(uv) * vec2(imageSize);
        Float mip = clamp(floor(0.5f * log2(max(dot(dx, dx), dot(dy, dy)))), 0.0f, maxMip);

        vec2 mipSize = max(vec2(1.0f), vec2(imageSize) / pow(2.0f, mip));

        vec3 atlasUV = getAtlasUV(textureID,
                                  uv,
                                  Int(mip),
                                  wrap,
                                  imageSize,
                                  mipSize,
                                  atlasSize,
                                  tileSize,
                                  tileBorder,
                                  tileMapOffsets,
                                  tileMap);

        vec2 adx = partialDerivativeX(uv) * mipSize / Float(atlasSize);
        vec2 ady = partialDerivativeY(uv) * mipSize / Float(atlasSize);

        IRReturn(vec4(textureGradArray(sampler, atlasUV, adx, ady)));

        IRFunctionEnd
    }

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
    vec4 sample_bicubic(Param<UInt> textureID,
                        Param<vec2> uv,
                        Param<Int> wrap,
                        Param<ivec2> imageSize,
                        Param<UInt> atlasSize,
                        Param<UInt> tileSize,
                        Param<UInt> tileBorder,
                        DynamicBufferWrapper<UInt> &tileMapOffsets,
                        DynamicBufferWrapper<UInt> &tileMap,
                        ShaderObject &sampler) {
        IRFunction

        Float maxMip = log2(Float(min(imageSize.value().x(), imageSize.value().y())));
        vec2 dx = partialDerivativeX(uv) * vec2(imageSize);
        vec2 dy = partialDerivativeY(uv) * vec2(imageSize);
        Float mip = clamp(floor(0.5f * log2(max(dot(dx, dx), dot(dy, dy)))), 0.0f, maxMip);
        vec2 mipSize = max(vec2(1.0f), vec2(imageSize) / pow(2.0f, mip));

        // texel position
        vec2 texel_f = fract(uv) * mipSize;
        vec2 tc = floor(texel_f - 0.5f) + 0.5f;
        vec2 f = texel_f - tc;

        // catmull-rom weights
        vec2 f2 = f * f;
        vec2 f3 = f2 * f;
        vec2 w0 = -0.5f * f3 + f2 - 0.5f * f;
        vec2 w1 = 1.5f * f3 - 2.5f * f2 + 1.0f;
        vec2 w2 = -1.5f * f3 + 2.0f * f2 + 0.5f * f;
        vec2 w3 = 0.5f * f3 - 0.5f * f2;

        // merge into 2 bilinear weights per axis
        vec2 g0 = w0 + w1;
        vec2 g1 = w2 + w3;

        // bilinear sample offsets in texel space
        vec2 h0 = -1.0f + w1 / g0;
        vec2 h1 = 1.0f + w3 / g1;

        // convert offsets to UV space
        vec2 uv00 = (tc + vec2(h0.x(), h0.y())) / mipSize;
        vec2 uv10 = (tc + vec2(h1.x(), h0.y())) / mipSize;
        vec2 uv01 = (tc + vec2(h0.x(), h1.y())) / mipSize;
        vec2 uv11 = (tc + vec2(h1.x(), h1.y())) / mipSize;

        vec2 adx = partialDerivativeX(uv) * mipSize / Float(atlasSize);
        vec2 ady = partialDerivativeY(uv) * mipSize / Float(atlasSize);

        // atlas lookup per sample
        vec3 a00 = getAtlasUV(textureID, uv00, Int(mip), wrap, imageSize, mipSize, atlasSize, tileSize, tileBorder,
                              tileMapOffsets, tileMap);
        vec3 a10 = getAtlasUV(textureID, uv10, Int(mip), wrap, imageSize, mipSize, atlasSize, tileSize, tileBorder,
                              tileMapOffsets, tileMap);
        vec3 a01 = getAtlasUV(textureID, uv01, Int(mip), wrap, imageSize, mipSize, atlasSize, tileSize, tileBorder,
                              tileMapOffsets, tileMap);
        vec3 a11 = getAtlasUV(textureID, uv11, Int(mip), wrap, imageSize, mipSize, atlasSize, tileSize, tileBorder,
                              tileMapOffsets, tileMap);

        vec4 s00 = vec4(textureGradArray(sampler, a00, adx, ady));
        vec4 s10 = vec4(textureGradArray(sampler, a10, adx, ady));
        vec4 s01 = vec4(textureGradArray(sampler, a01, adx, ady));
        vec4 s11 = vec4(textureGradArray(sampler, a11, adx, ady));

        // blend with cubic weights
        vec4 color = g0.x() * g0.y() * s00
                     + g1.x() * g0.y() * s10
                     + g0.x() * g1.y() * s01
                     + g1.x() * g1.y() * s11;

        IRReturn(color);

        IRFunctionEnd
    }
}
