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
    UInt getTileIndex(Param<vec2> wrapped, Param<vec2> mipSize, Param<UInt> tileSize) {
        IRFunction

        ivec2 texel = ivec2(wrapped * mipSize);
        uvec2 tile = uvec2(texel) / tileSize;
        uvec2 tiles = (uvec2(mipSize) + tileSize - 1) / tileSize;

        IRReturn(UInt(tile.y() * tiles.x() + tile.x()));

        IRFunctionEnd
    }

    ivec3 getAtlasTexel(Param<UInt> textureID,
                        Param<vec2> wrapped,
                        Param<Int> mip,
                        Param<vec2> mipSize,
                        Param<UInt> atlasSize,
                        Param<UInt> tileSize,
                        Param<UInt> tileBorder,
                        DynamicBufferWrapper<UInt> &tileMapOffsets,
                        DynamicBufferWrapper<UInt> &tileMap) {
        IRFunction

        ivec2 texel = ivec2(wrapped * mipSize);

        uvec2 tile = uvec2(texel) / tileSize;
        uvec2 tiles = (uvec2(mipSize) + tileSize - 1) / tileSize;

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
                    Param<vec2> wrapped,
                    Param<Int> mip,
                    Param<vec2> mipSize,
                    Param<UInt> atlasSize,
                    Param<UInt> tileSize,
                    Param<UInt> tileBorder,
                    DynamicBufferWrapper<UInt> &tileMapOffsets,
                    DynamicBufferWrapper<UInt> &tileMap) {
        IRFunction

        vec2 texel = wrapped * mipSize;
        uvec2 tile = uvec2(texel) / tileSize;
        uvec2 tiles = (uvec2(mipSize) + tileSize - 1) / tileSize;

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

    Float getResidentMip(Param<UInt> textureID,
                         Param<vec2> wrapped,
                         Param<ivec2> imageSize,
                         Param<UInt> tileSize,
                         DynamicBufferWrapper<UInt> &residencyMap) {
        IRFunction

        ivec2 mip0Texel = ivec2(wrapped * imageSize);

        uvec2 mip0tile = uvec2(mip0Texel) / tileSize;
        uvec2 mip0tiles = (uvec2(imageSize) + tileSize - 1) / tileSize;

        IRReturn(Float(residencyMap[textureID + mip0tile.y() * mip0tiles.x() + mip0tile.x()]));

        IRFunctionEnd
    }

    Float getLod(Param<vec2> uv, Param<ivec2> imageSize) {
        IRFunction

        vec2 dx = partialDerivativeX(uv);
        vec2 dy = partialDerivativeY(uv);

        // Correct for seam discontinuities before scaling
        // A seam-crossing derivative is ~±1.0 in UV space — round() cancels it
        dx = dx - round(dx);
        dy = dy - round(dy);

        // Now scale to pixel space for LoD
        dx = dx * imageSize;
        dy = dy * imageSize;

        Float lod = 0.5f * log2(max(dot(dx, dx), dot(dy, dy)) + 1e-10f);

        IRReturn(lod); // raw, unclamped — callers clamp via getMaxMip

        IRFunctionEnd
    }

    Float getMaxMip(Param<UInt> w, Param<UInt> h) {
        IRFunction
        IRReturn(Float(floor(log2(Float(max(w, h))))));
        IRFunctionEnd
    }

    Float getMip(Param<UInt> textureID,
                 Param<vec2> uv,
                 Param<vec2> wrappedUV,
                 Param<ivec2> imageSize,
                 Param<UInt> tileSize,
                 DynamicBufferWrapper<UInt> &residencyMap) {
        IRFunction

        Float minMip = getResidentMip(textureID,
                                      wrappedUV,
                                      imageSize,
                                      tileSize,
                                      residencyMap);

        Float maxMip = getMaxMip(imageSize.value().x(), imageSize.value().y());

        Float lod = getLod(uv, imageSize);

        IRReturn(Float( clamp(floor(lod), minMip, maxMip)));

        IRFunctionEnd
    }

    Float getMip(Param<vec2> uv, Param<ivec2> imageSize) {
        IRFunction

        Float maxMip = getMaxMip(imageSize.value().x(), imageSize.value().y());

        Float lod = getLod(uv, imageSize);

        IRReturn(Float( clamp(floor(lod), 0.0f, maxMip)));

        IRFunctionEnd
    }

    vec2 wrapUV(Param<vec2> uv, Param<Int> wrap) {
        IRFunction

        vec2 wrapped;
        If(wrap == WRAP_REPEAT)
            wrapped = fract(uv);
        Else
            wrapped = clamp(uv, vec2(0.0f), vec2(1.0f));
        Fi
        IRReturn(wrapped);

        IRFunctionEnd
    }

    vec4 sample(Param<UInt> textureID,
                Param<vec2> uv,
                Param<Int> wrap,
                Param<Int> minFilter,
                Param<Int> magFilter,
                Param<Int> mipFilter,
                Param<ivec2> imageSize,
                Param<UInt> atlasSize,
                Param<UInt> tileSize,
                Param<UInt> tileBorder,
                DynamicBufferWrapper<UInt> &tileMapOffsets,
                DynamicBufferWrapper<UInt> &tileMap,
                DynamicBufferWrapper<UInt> &residencyMap,
                ShaderObject &sampler) {
        IRFunction

        Float lod = getLod(uv, imageSize);

        If(lod <= 0.0f)
            If(magFilter == FILTER_BICUBIC)
                IRReturn(
                    sample_bicubic(textureID, uv, wrap, imageSize, atlasSize, tileSize, tileBorder, tileMapOffsets,
                        tileMap, residencyMap, sampler));
            Else
                If(magFilter == FILTER_BILINEAR)
                    IRReturn(
                        sample_bilinear(textureID, uv, wrap, imageSize, atlasSize, tileSize, tileBorder, tileMapOffsets,
                            tileMap, residencyMap, sampler));
                Else
                    IRReturn(
                        sample_nearest(textureID, uv, wrap, imageSize, atlasSize, tileSize, tileBorder, tileMapOffsets,
                            tileMap, residencyMap, sampler));
                Fi
            Fi
        Else
            If(minFilter == FILTER_BICUBIC)
                If(mipFilter == rg::NEAREST)
                    IRReturn(
                        sample_bicubic(textureID, uv, wrap, imageSize, atlasSize, tileSize, tileBorder, tileMapOffsets,
                            tileMap, residencyMap, sampler));
                Else
                    IRReturn(
                        sample_bicubic_trilinear(textureID, uv, wrap, imageSize, atlasSize, tileSize, tileBorder,
                            tileMapOffsets,tileMap, residencyMap, sampler));
                Fi
            Else
                If(minFilter == FILTER_BILINEAR)
                    If(mipFilter == rg::NEAREST)
                        IRReturn(
                            sample_bilinear(textureID, uv, wrap, imageSize, atlasSize, tileSize, tileBorder,
                                tileMapOffsets,tileMap, residencyMap, sampler));
                    Else
                        IRReturn(
                            sample_trilinear(textureID, uv, wrap, imageSize, atlasSize, tileSize, tileBorder,
                                tileMapOffsets,tileMap, residencyMap, sampler));
                    Fi
                Else
                    If(mipFilter == rg::NEAREST)
                        IRReturn(
                            sample_nearest(textureID, uv, wrap, imageSize, atlasSize, tileSize, tileBorder,
                                tileMapOffsets,tileMap,residencyMap,sampler))
                    Else
                        IRReturn(
                            sample_nearest_linear(textureID, uv, wrap, imageSize, atlasSize, tileSize, tileBorder,
                                tileMapOffsets,tileMap, residencyMap, sampler));
                    Fi
                Fi
            Fi
        Fi

        IRFunctionEnd
    }

    vec4 sample_nearest(Param<UInt> textureID,
                        Param<vec2> uv,
                        Param<Int> wrap,
                        Param<ivec2> imageSize,
                        Param<UInt> atlasSize,
                        Param<UInt> tileSize,
                        Param<UInt> tileBorder,
                        DynamicBufferWrapper<UInt> &tileMapOffsets,
                        DynamicBufferWrapper<UInt> &tileMap,
                        DynamicBufferWrapper<UInt> &residencyMap,
                        ShaderObject &sampler) {
        IRFunction

        vec2 wrapped = wrapUV(uv, wrap);

        Float mip = getMip(textureID, uv, wrapped, imageSize, tileSize, residencyMap);

        vec2 mipSize = max(vec2(1.0f), vec2(imageSize) / exp2(mip));

        ivec3 atlasUV = getAtlasTexel(textureID,
                                      wrapped,
                                      Int(mip),
                                      mipSize,
                                      atlasSize,
                                      tileSize,
                                      tileBorder,
                                      tileMapOffsets,
                                      tileMap);

        IRReturn(vec4(texelFetchArray(sampler, atlasUV.xy(), atlasUV.z())));

        IRFunctionEnd
    }

    vec4 sample_nearest_linear(Param<UInt> textureID,
                               Param<vec2> uv,
                               Param<Int> wrap,
                               Param<ivec2> imageSize,
                               Param<UInt> atlasSize,
                               Param<UInt> tileSize,
                               Param<UInt> tileBorder,
                               DynamicBufferWrapper<UInt> &tileMapOffsets,
                               DynamicBufferWrapper<UInt> &tileMap,
                               DynamicBufferWrapper<UInt> &residencyMap,
                               ShaderObject &sampler) {
        IRFunction

        vec2 wrapped = wrapUV(uv, wrap);

        Float minMip = getResidentMip(textureID, wrapped, imageSize, tileSize, residencyMap);
        Float maxMip = getMaxMip(imageSize.value().x(), imageSize.value().y());

        Float lod = getLod(uv, imageSize);
        Float clampedLod = clamp(lod, Float(minMip), maxMip);

        Float mip0 = floor(clampedLod);
        Float mip1 = min(mip0 + 1.0f, maxMip);
        Float blend = fract(clampedLod);

        vec2 mipSize0 = max(vec2(1.0f), vec2(imageSize) / exp2(mip0));
        vec2 mipSize1 = max(vec2(1.0f), vec2(imageSize) / exp2(mip1));

        ivec3 atlasUV0 = getAtlasTexel(textureID,
                                       wrapped,
                                       Int(mip0),
                                       mipSize0,
                                       atlasSize,
                                       tileSize,
                                       tileBorder,
                                       tileMapOffsets,
                                       tileMap);
        ivec3 atlasUV1 = getAtlasTexel(textureID,
                                       wrapped,
                                       Int(mip1),
                                       mipSize1,
                                       atlasSize,
                                       tileSize,
                                       tileBorder,
                                       tileMapOffsets,
                                       tileMap);

        vec4 s0 = vec4(texelFetchArray(sampler, atlasUV0.xy(), atlasUV0.z()));
        vec4 s1 = vec4(texelFetchArray(sampler, atlasUV1.xy(), atlasUV1.z()));

        IRReturn(vec4(mix(s0, s1, blend)));

        IRFunctionEnd
    }

    vec4 sample_bilinear(Param<UInt> textureID,
                         Param<vec2> uv,
                         Param<Int> wrap,
                         Param<ivec2> imageSize,
                         Param<UInt> atlasSize,
                         Param<UInt> tileSize,
                         Param<UInt> tileBorder,
                         DynamicBufferWrapper<UInt> &tileMapOffsets,
                         DynamicBufferWrapper<UInt> &tileMap,
                         DynamicBufferWrapper<UInt> &residencyMap,
                         ShaderObject &sampler) {
        IRFunction

        vec2 wrapped = wrapUV(uv, wrap);

        Float mip = getMip(textureID, uv, wrapped, imageSize, tileSize, residencyMap);

        vec2 mipSize = max(vec2(1.0f), vec2(imageSize) / exp2(mip));

        vec3 atlasUV = getAtlasUV(textureID,
                                  wrapped,
                                  Int(mip),
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

    vec4 sample_trilinear(Param<UInt> textureID,
                          Param<vec2> uv,
                          Param<Int> wrap,
                          Param<ivec2> imageSize,
                          Param<UInt> atlasSize,
                          Param<UInt> tileSize,
                          Param<UInt> tileBorder,
                          DynamicBufferWrapper<UInt> &tileMapOffsets,
                          DynamicBufferWrapper<UInt> &tileMap,
                          DynamicBufferWrapper<UInt> &residencyMap,
                          ShaderObject &sampler) {
        IRFunction

        vec2 wrapped = wrapUV(uv, wrap);

        Float minMip = getResidentMip(textureID, wrapped, imageSize, tileSize, residencyMap);
        Float maxMip = getMaxMip(imageSize.value().x(), imageSize.value().y());

        Float lod = getLod(uv, imageSize);
        Float clampedLod = clamp(lod, Float(minMip), maxMip);

        Float mip0 = floor(clampedLod);
        Float mip1 = min(mip0 + 1.0f, maxMip);
        Float blend = fract(clampedLod);

        vec2 mipSize0 = max(vec2(1.0f), vec2(imageSize) / exp2(mip0));
        vec2 mipSize1 = max(vec2(1.0f), vec2(imageSize) / exp2(mip1));

        vec3 atlasUV0 = getAtlasUV(textureID,
                                   wrapped,
                                   Int(mip0),
                                   mipSize0,
                                   atlasSize,
                                   tileSize,
                                   tileBorder,
                                   tileMapOffsets,
                                   tileMap);
        vec3 atlasUV1 = getAtlasUV(textureID,
                                   wrapped,
                                   Int(mip1),
                                   mipSize1,
                                   atlasSize,
                                   tileSize,
                                   tileBorder,
                                   tileMapOffsets,
                                   tileMap);

        vec2 adx0 = partialDerivativeX(uv) * mipSize0 / Float(atlasSize);
        vec2 ady0 = partialDerivativeY(uv) * mipSize0 / Float(atlasSize);
        vec2 adx1 = partialDerivativeX(uv) * mipSize1 / Float(atlasSize);
        vec2 ady1 = partialDerivativeY(uv) * mipSize1 / Float(atlasSize);

        vec4 s0 = vec4(textureGradArray(sampler, atlasUV0, adx0, ady0));
        vec4 s1 = vec4(textureGradArray(sampler, atlasUV1, adx1, ady1));

        IRReturn(vec4(mix(s0, s1, blend)));

        IRFunctionEnd
    }

    vec4 sample_bicubic(Param<UInt> textureID,
                        Param<vec2> uv,
                        Param<Int> wrap,
                        Param<ivec2> imageSize,
                        Param<UInt> atlasSize,
                        Param<UInt> tileSize,
                        Param<UInt> tileBorder,
                        DynamicBufferWrapper<UInt> &tileMapOffsets,
                        DynamicBufferWrapper<UInt> &tileMap,
                        DynamicBufferWrapper<UInt> &residencyMap,
                        ShaderObject &sampler) {
        IRFunction

        vec2 wrapped = wrapUV(uv, wrap);

        Float mip = getMip(textureID, uv, wrapped, imageSize, tileSize, residencyMap);

        vec2 mipSize = max(vec2(1.0f), vec2(imageSize) / exp2(mip));

        // texel position
        vec2 texel_f = wrapped * mipSize;
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
        vec2 uv00 = wrapUV((tc + vec2(h0.x(), h0.y())) / mipSize, wrap);
        vec2 uv10 = wrapUV((tc + vec2(h1.x(), h0.y())) / mipSize, wrap);
        vec2 uv01 = wrapUV((tc + vec2(h0.x(), h1.y())) / mipSize, wrap);
        vec2 uv11 = wrapUV((tc + vec2(h1.x(), h1.y())) / mipSize, wrap);

        vec2 adx = partialDerivativeX(uv) * mipSize / Float(atlasSize);
        vec2 ady = partialDerivativeY(uv) * mipSize / Float(atlasSize);

        // atlas lookup per sample
        vec3 a00 = getAtlasUV(textureID,
                              uv00,
                              Int(mip),
                              mipSize,
                              atlasSize,
                              tileSize,
                              tileBorder,
                              tileMapOffsets,
                              tileMap);
        vec3 a10 = getAtlasUV(textureID,
                              uv10,
                              Int(mip),
                              mipSize,
                              atlasSize,
                              tileSize,
                              tileBorder,
                              tileMapOffsets,
                              tileMap);
        vec3 a01 = getAtlasUV(textureID,
                              uv01,
                              Int(mip),
                              mipSize,
                              atlasSize,
                              tileSize,
                              tileBorder,
                              tileMapOffsets,
                              tileMap);
        vec3 a11 = getAtlasUV(textureID,
                              uv11,
                              Int(mip),
                              mipSize,
                              atlasSize,
                              tileSize,
                              tileBorder,
                              tileMapOffsets,
                              tileMap);

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

    vec4 sample_bicubic_trilinear(Param<UInt> textureID,
                                  Param<vec2> uv,
                                  Param<Int> wrap,
                                  Param<ivec2> imageSize,
                                  Param<UInt> atlasSize,
                                  Param<UInt> tileSize,
                                  Param<UInt> tileBorder,
                                  DynamicBufferWrapper<UInt> &tileMapOffsets,
                                  DynamicBufferWrapper<UInt> &tileMap,
                                  DynamicBufferWrapper<UInt> &residencyMap,
                                  ShaderObject &sampler) {
        IRFunction

        vec2 wrapped = wrapUV(uv, wrap);

        Float minMip = getResidentMip(textureID, wrapped, imageSize, tileSize, residencyMap);
        Float maxMip = getMaxMip(imageSize.value().x(), imageSize.value().y());

        Float lod = getLod(uv, imageSize);
        Float clampedLod = clamp(lod, Float(minMip), maxMip);

        Float mip0 = floor(clampedLod);
        Float mip1 = min(mip0 + 1.0f, maxMip);
        Float blend = fract(clampedLod);

        vec2 mipSize0 = max(vec2(1.0f), vec2(imageSize) / exp2(mip0));
        vec2 mipSize1 = max(vec2(1.0f), vec2(imageSize) / exp2(mip1));

        // catmull-rom sample for one mip level
        auto sampleBicubic = [&](Float mip, vec2 mipSize) -> vec4 {
            vec2 texel_f = wrapped * mipSize;
            vec2 tc = floor(texel_f - 0.5f) + 0.5f;
            vec2 f = texel_f - tc;

            vec2 f2 = f * f;
            vec2 f3 = f2 * f;
            vec2 w0 = -0.5f * f3 + f2 - 0.5f * f;
            vec2 w1 = 1.5f * f3 - 2.5f * f2 + 1.0f;
            vec2 w2 = -1.5f * f3 + 2.0f * f2 + 0.5f * f;
            vec2 w3 = 0.5f * f3 - 0.5f * f2;

            vec2 g0 = w0 + w1;
            vec2 g1 = w2 + w3;
            vec2 h0 = -1.0f + w1 / g0;
            vec2 h1 = 1.0f + w3 / g1;

            vec2 uv00 = wrapUV((tc + vec2(h0.x(), h0.y())) / mipSize, wrap);
            vec2 uv10 = wrapUV((tc + vec2(h1.x(), h0.y())) / mipSize, wrap);
            vec2 uv01 = wrapUV((tc + vec2(h0.x(), h1.y())) / mipSize, wrap);
            vec2 uv11 = wrapUV((tc + vec2(h1.x(), h1.y())) / mipSize, wrap);

            vec2 adx = partialDerivativeX(uv) * mipSize / Float(atlasSize);
            vec2 ady = partialDerivativeY(uv) * mipSize / Float(atlasSize);

            vec3 a00 = getAtlasUV(textureID,
                                  uv00,
                                  Int(mip),
                                  mipSize,
                                  atlasSize,
                                  tileSize,
                                  tileBorder,
                                  tileMapOffsets,
                                  tileMap);
            vec3 a10 = getAtlasUV(textureID,
                                  uv10,
                                  Int(mip),
                                  mipSize,
                                  atlasSize,
                                  tileSize,
                                  tileBorder,
                                  tileMapOffsets,
                                  tileMap);
            vec3 a01 = getAtlasUV(textureID,
                                  uv01,
                                  Int(mip),
                                  mipSize,
                                  atlasSize,
                                  tileSize,
                                  tileBorder,
                                  tileMapOffsets,
                                  tileMap);
            vec3 a11 = getAtlasUV(textureID,
                                  uv11,
                                  Int(mip),
                                  mipSize,
                                  atlasSize,
                                  tileSize,
                                  tileBorder,
                                  tileMapOffsets,
                                  tileMap);

            vec4 s00 = vec4(textureGradArray(sampler, a00, adx, ady));
            vec4 s10 = vec4(textureGradArray(sampler, a10, adx, ady));
            vec4 s01 = vec4(textureGradArray(sampler, a01, adx, ady));
            vec4 s11 = vec4(textureGradArray(sampler, a11, adx, ady));

            return g0.x() * g0.y() * s00
                   + g1.x() * g0.y() * s10
                   + g0.x() * g1.y() * s01
                   + g1.x() * g1.y() * s11;
        };

        IRReturn(vec4(mix(sampleBicubic(mip0, mipSize0), sampleBicubic(mip1, mipSize1), blend)));

        IRFunctionEnd
    }


    UInt readback_sample(Param<vec2> uv,
                         Param<Int> wrap,
                         Param<Int> minFilter,
                         Param<Int> magFilter,
                         Param<Int> mipFilter,
                         Param<ivec2> imageSize,
                         Param<UInt> tileSize,
                         ParamOut<uvec2> readback0,
                         ParamOut<uvec2> readback1) {
        IRFunction

        Float lod = getLod(uv, imageSize);

        If(lod <= 0.0f)
            If(magFilter == FILTER_BICUBIC)
                IRReturn(
                    readback_sample_bicubic(uv,
                        wrap,
                        imageSize,
                        tileSize,
                        readback0.value()));
            Else
                If(magFilter == FILTER_BILINEAR)
                    IRReturn(
                        readback_sample_bilinear(uv,
                            wrap,
                            imageSize,
                            tileSize,
                            readback0.value()));
                Else
                    IRReturn(
                        readback_sample_nearest(uv,
                            wrap,
                            imageSize,
                            tileSize,
                            readback0.value()));
                Fi
            Fi
        Else
            If(minFilter == FILTER_BICUBIC)
                If(mipFilter == rg::NEAREST)
                    IRReturn(
                        readback_sample_bicubic(uv,
                            wrap,
                            imageSize,
                            tileSize,
                            readback0.value()));
                Else
                    IRReturn(
                        readback_sample_bicubic_trilinear(uv,
                            wrap,
                            imageSize,
                            tileSize,
                            readback0.value(),
                            readback1.value()));
                Fi
            Else
                If(minFilter == FILTER_BILINEAR)
                    If(mipFilter == rg::NEAREST)
                        IRReturn(
                            readback_sample_bilinear(uv,
                                wrap,
                                imageSize,
                                tileSize,
                                readback0.value()));
                    Else
                        IRReturn(
                            readback_sample_trilinear(uv,
                                wrap,
                                imageSize,
                                tileSize,
                                readback0.value(),
                                readback1.value()));
                    Fi
                Else
                    If(mipFilter == rg::NEAREST)
                        IRReturn(
                            readback_sample_nearest(uv,
                                wrap,
                                imageSize,
                                tileSize,
                                readback0.value()))
                    Else
                        IRReturn(
                            readback_sample_nearest_linear(uv,
                                wrap,
                                imageSize,
                                tileSize,
                                readback0.value(),
                                readback1.value()));
                    Fi
                Fi
            Fi
        Fi

        IRFunctionEnd
    }

    UInt readback_sample_nearest(Param<vec2> uv,
                                 Param<Int> wrap,
                                 Param<ivec2> imageSize,
                                 Param<UInt> tileSize,
                                 ParamOut<uvec2> readback0) {
        IRFunction

        vec2 wrapped = wrapUV(uv, wrap);
        Float mip = getMip(uv, imageSize);
        vec2 mipSize = max(vec2(1.0f), vec2(imageSize) / exp2(mip));

        readback0.value().x() = UInt(mip);
        readback0.value().y() = getTileIndex(wrapped, mipSize, tileSize);

        IRReturn(UInt(1));

        IRFunctionEnd
    }

    UInt readback_sample_nearest_linear(Param<vec2> uv,
                                        Param<Int> wrap,
                                        Param<ivec2> imageSize,
                                        Param<UInt> tileSize,
                                        ParamOut<uvec2> readback0,
                                        ParamOut<uvec2> readback1) {
        IRFunction

        vec2 wrapped = wrapUV(uv, wrap);

        Float maxMip = getMaxMip(imageSize.value().x(), imageSize.value().y());

        Float lod = getLod(uv, imageSize);
        Float clampedLod = clamp(lod, Float(0.0f), maxMip);

        Float mip0 = floor(clampedLod);
        Float mip1 = min(mip0 + 1.0f, maxMip);

        vec2 mipSize0 = max(vec2(1.0f), vec2(imageSize) / exp2(mip0));
        vec2 mipSize1 = max(vec2(1.0f), vec2(imageSize) / exp2(mip1));

        readback0.value().x() = UInt(mip0);
        readback0.value().y() = getTileIndex(wrapped, mipSize0, tileSize);

        readback1.value().x() = UInt(mip1);
        readback1.value().y() = getTileIndex(wrapped, mipSize1, tileSize);

        IRReturn(UInt(2));

        IRFunctionEnd
    }

    UInt readback_sample_bilinear(Param<vec2> uv,
                                  Param<Int> wrap,
                                  Param<ivec2> imageSize,
                                  Param<UInt> tileSize,
                                  ParamOut<uvec2> readback0) {
        IRFunction

        vec2 wrapped = wrapUV(uv, wrap);
        Float mip = getMip(uv, imageSize);
        vec2 mipSize = max(vec2(1.0f), vec2(imageSize) / exp2(mip));

        readback0.value().x() = UInt(mip);
        readback0.value().y() = getTileIndex(wrapped, mipSize, tileSize);

        IRReturn(UInt(1));

        IRFunctionEnd
    }

    UInt readback_sample_trilinear(Param<vec2> uv,
                                   Param<Int> wrap,
                                   Param<ivec2> imageSize,
                                   Param<UInt> tileSize,
                                   ParamOut<uvec2> readback0,
                                   ParamOut<uvec2> readback1) {
        IRFunction

        vec2 wrapped = wrapUV(uv, wrap);

        Float maxMip = getMaxMip(imageSize.value().x(), imageSize.value().y());

        Float lod = getLod(uv, imageSize);
        Float clampedLod = clamp(lod, Float(0.0f), maxMip);

        Float mip0 = floor(clampedLod);
        Float mip1 = min(mip0 + 1.0f, maxMip);

        vec2 mipSize0 = max(vec2(1.0f), vec2(imageSize) / exp2(mip0));
        vec2 mipSize1 = max(vec2(1.0f), vec2(imageSize) / exp2(mip1));

        readback0.value().x() = UInt(mip0);
        readback0.value().y() = getTileIndex(wrapped, mipSize0, tileSize);

        readback1.value().x() = UInt(mip1);
        readback1.value().y() = getTileIndex(wrapped, mipSize1, tileSize);

        IRReturn(UInt(2));

        IRFunctionEnd
    }

    UInt readback_sample_bicubic(Param<vec2> uv,
                                 Param<Int> wrap,
                                 Param<ivec2> imageSize,
                                 Param<UInt> tileSize,
                                 ParamOut<uvec2> readback0) {
        IRFunction

        vec2 wrapped = wrapUV(uv, wrap);
        Float mip = getMip(uv, imageSize);

        vec2 mipSize = max(vec2(1.0f), vec2(imageSize) / exp2(mip));

        readback0.value().x() = UInt(mip);
        readback0.value().y() = getTileIndex(wrapped, mipSize, tileSize);

        IRReturn(UInt(1));

        IRFunctionEnd
    }

    UInt readback_sample_bicubic_trilinear(Param<vec2> uv,
                                           Param<Int> wrap,
                                           Param<ivec2> imageSize,
                                           Param<UInt> tileSize,
                                           ParamOut<uvec2> readback0,
                                           ParamOut<uvec2> readback1) {
        IRFunction

        vec2 wrapped = wrapUV(uv, wrap);

        Float maxMip = getMaxMip(imageSize.value().x(), imageSize.value().y());

        Float lod = getLod(uv, imageSize);
        Float clampedLod = clamp(lod, Float(0.0f), maxMip);

        Float mip0 = floor(clampedLod);
        Float mip1 = min(mip0 + 1.0f, maxMip);

        vec2 mipSize0 = max(vec2(1.0f), vec2(imageSize) / exp2(mip0));
        vec2 mipSize1 = max(vec2(1.0f), vec2(imageSize) / exp2(mip1));

        readback0.value().x() = UInt(mip0);
        readback0.value().y() = getTileIndex(wrapped, mipSize0, tileSize);

        readback1.value().x() = UInt(mip1);
        readback1.value().y() = getTileIndex(wrapped, mipSize1, tileSize);

        IRReturn(UInt(2));

        IRFunctionEnd
    }
}
