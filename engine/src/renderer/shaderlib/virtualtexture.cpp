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
        texel.x() = clamp(texel.x(), 0, Int(mipSize.value().x()) - 1);
        texel.y() = clamp(texel.y(), 0, Int(mipSize.value().y()) - 1);

        uvec2 tile = uvec2(texel) / tileSize;
        uvec2 tiles = (uvec2(mipSize) + tileSize - 1) / tileSize;

        UInt tileMapBaseOffset = tileMapOffsets[textureID + UInt(mip)];
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

        UInt tileMapBaseOffset = tileMapOffsets[textureID + UInt(mip)];
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
                         DynamicBufferWrapper<UInt> &residencyMapOffsets,
                         DynamicBufferWrapper<UInt> &residencyMap) {
        IRFunction

        ivec2 mip0Texel = ivec2(wrapped * imageSize);

        uvec2 mip0tile = uvec2(mip0Texel) / tileSize;
        uvec2 mip0tiles = (uvec2(imageSize) + tileSize - 1) / tileSize;

        UInt offset = residencyMapOffsets[textureID];

        IRReturn(Float(residencyMap[offset + mip0tile.y() * mip0tiles.x() + mip0tile.x()]));

        IRFunctionEnd
    }

    Float getLod(Param<vec2> dxA, Param<vec2> dyA, Param<ivec2> imageSize, Param<Float> maxAnisotropy) {
        IRFunction

        // dxA/dyA are already seam-corrected by the caller. Scale to texel space.
        vec2 dx = dxA * imageSize;
        vec2 dy = dyA * imageSize;

        // Footprint ellipse axes = eigenvalues of J^T J = [[A,B],[B,C]] (squared axis lengths).
        Float A = dot(dx, dx);
        Float C = dot(dy, dy);
        Float B = dot(dx, dy);

        Float halfSum = 0.5f * (A + C);
        Float halfDiff = 0.5f * (A - C);

        // discriminant is a sum of squares -> always >= 0, sqrt is safe without a guard
        Float root = sqrt(halfDiff * halfDiff + B * B);

        Float majorSq = halfSum + root; // >= 0 structurally (halfSum >= 0, root >= 0)
        Float minorSq = max(halfSum - root, 0.0f); // can dip < 0 from fp when det ~ 0 -> clamp

        // + epsilon only guards the exact-zero case; negligible on any real footprint
        Float majorLod = 0.5f * log2(majorSq + 1e-20f);
        Float minorLod = 0.5f * log2(minorSq + 1e-20f);

        // clamp maxAnisotropy to [1, +inf): 1 => isotropic (lod == majorLod), and guards log2(<=0)
        Float logAniso = log2(max(maxAnisotropy, 1.0f));

        // ratio = major/minor <= maxAnisotropy  <=>  minorLod >= majorLod - log2(maxAnisotropy)
        IRReturn(Float(max(minorLod, majorLod - logAniso)));

        IRFunctionEnd
    }

    Float getMip(Param<UInt> textureID,
                 Param<vec2> dx,
                 Param<vec2> dy,
                 Param<vec2> wrappedUV,
                 Param<ivec2> imageSize,
                 Param<UInt> imageMaxMip,
                 Param<UInt> tileSize,
                 Param<Float> maxAnisotropy,
                 DynamicBufferWrapper<UInt> &residencyMapOffsets,
                 DynamicBufferWrapper<UInt> &residencyMap) {
        IRFunction

        Float minMip = getResidentMip(textureID,
                                      wrappedUV,
                                      imageSize,
                                      tileSize,
                                      residencyMapOffsets,
                                      residencyMap);

        Float maxMip = Float(imageMaxMip);

        Float lod = getLod(dx, dy, imageSize, maxAnisotropy);

        IRReturn(Float( clamp(floor(lod), minMip, maxMip)));

        IRFunctionEnd
    }

    Float getMipReadback(Param<vec2> dx,
                         Param<vec2> dy,
                         Param<ivec2> imageSize,
                         Param<UInt> imageMaxMip,
                         Param<Float> maxAnisotropy) {
        IRFunction

        Float maxMip = Float(imageMaxMip);

        Float lod = getLod(dx, dy, imageSize, maxAnisotropy);

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

    vec4 sample_virtual_readback(Param<UInt> textureID,
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
                                 DynamicBufferWrapper<UInt> &readback,
                                 ShaderObject &sampler) {
        IRFunction
        sideEffect(readback_sample(textureID,
                                   uv,
                                   wrap,
                                   minFilter,
                                   magFilter,
                                   mipFilter,
                                   imageSize,
                                   imageMaxMip,
                                   tileSize,
                                   maxAnisotropy,
                                   tileMapOffsets,
                                   readback));
        IRReturn(sample_virtual(textureID,
            uv,
            wrap,
            minFilter,
            magFilter,
            mipFilter,
            imageSize,
            imageMaxMip,
            atlasSize,
            tileSize,
            tileBorder,
            maxAnisotropy,
            tileMapOffsets,
            tileMap,
            residencyMapOffsets,
            residencyMap,
            sampler))
        IRFunctionEnd
    }

    vec4 sample_virtual(Param<UInt> textureID,
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
                        ShaderObject &sampler) {
        IRFunction

        vec2 dx = partialDerivativeX(uv);
        vec2 dy = partialDerivativeY(uv);

        // Correct for seam discontinuities before scaling
        // A seam-crossing derivative is ~±1.0 in UV space — round() cancels it
        dx = dx - round(dx);
        dy = dy - round(dy);

        Float lod = getLod(dx, dy, imageSize, maxAnisotropy);

        If(lod <= 0.0f)
            If(magFilter == FILTER_BICUBIC)
                IRReturn(
                    sample_bicubic(textureID,
                        uv,
                        wrap,
                        imageSize,
                        imageMaxMip,
                        atlasSize,
                        tileSize,
                        tileBorder,
                        maxAnisotropy,
                        dx,
                        dy,
                        tileMapOffsets,
                        tileMap,
                        residencyMapOffsets,
                        residencyMap,
                        sampler));
            Else
                If(magFilter == FILTER_BILINEAR)
                    IRReturn(
                        sample_bilinear(textureID,
                            uv,
                            wrap,
                            imageSize,
                            imageMaxMip,
                            atlasSize,
                            tileSize,
                            tileBorder,
                            maxAnisotropy,
                            dx,
                            dy,
                            tileMapOffsets,
                            tileMap,
                            residencyMapOffsets,
                            residencyMap,
                            sampler));
                Else
                    IRReturn(
                        sample_nearest(textureID,
                            uv,
                            wrap,
                            imageSize,
                            imageMaxMip,
                            atlasSize,
                            tileSize,
                            tileBorder,
                            maxAnisotropy,
                            dx,
                            dy,
                            tileMapOffsets,
                            tileMap,
                            residencyMapOffsets,
                            residencyMap,
                            sampler));
                Fi
            Fi
        Else
            If(minFilter == FILTER_BICUBIC)
                If(mipFilter == rg::NEAREST)
                    IRReturn(
                        sample_bicubic(textureID,
                            uv,
                            wrap,
                            imageSize,
                            imageMaxMip,
                            atlasSize,
                            tileSize,
                            tileBorder,
                            maxAnisotropy,
                            dx,
                            dy,
                            tileMapOffsets,
                            tileMap,
                            residencyMapOffsets,
                            residencyMap,
                            sampler));
                Else
                    IRReturn(
                        sample_bicubic_trilinear(textureID,
                            uv,
                            wrap,
                            imageSize,
                            imageMaxMip,
                            atlasSize,
                            tileSize,
                            tileBorder,
                            maxAnisotropy,
                            dx,
                            dy,
                            tileMapOffsets,
                            tileMap,
                            residencyMapOffsets,
                            residencyMap,
                            sampler));
                Fi
            Else
                If(minFilter == FILTER_BILINEAR)
                    If(mipFilter == rg::NEAREST)
                        IRReturn(
                            sample_bilinear(textureID,
                                uv,
                                wrap,
                                imageSize,
                                imageMaxMip,
                                atlasSize,
                                tileSize,
                                tileBorder,
                                maxAnisotropy,
                                dx,
                                dy,
                                tileMapOffsets,
                                tileMap,
                                residencyMapOffsets,
                                residencyMap,
                                sampler));
                    Else
                        IRReturn(
                            sample_trilinear(textureID,
                                uv,
                                wrap,
                                imageSize,
                                imageMaxMip,
                                atlasSize,
                                tileSize,
                                tileBorder,
                                maxAnisotropy,
                                dx,
                                dy,
                                tileMapOffsets,
                                tileMap,
                                residencyMapOffsets,
                                residencyMap,
                                sampler));
                    Fi
                Else
                    If(mipFilter == rg::NEAREST)
                        IRReturn(
                            sample_nearest(textureID,
                                uv,
                                wrap,
                                imageSize,
                                imageMaxMip,
                                atlasSize,
                                tileSize,
                                tileBorder,
                                maxAnisotropy,
                                dx,
                                dy,
                                tileMapOffsets,
                                tileMap,
                                residencyMapOffsets,
                                residencyMap,
                                sampler));
                    Else
                        IRReturn(
                            sample_nearest_linear(textureID,
                                uv,
                                wrap,
                                imageSize,
                                imageMaxMip,
                                atlasSize,
                                tileSize,
                                tileBorder,
                                maxAnisotropy,
                                dx,
                                dy,
                                tileMapOffsets,
                                tileMap,
                                residencyMapOffsets,
                                residencyMap,
                                sampler));
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
                        ShaderObject &sampler) {
        IRFunction

        vec2 wrapped = wrapUV(uv, wrap);

        Float mip = getMip(textureID,
                           dx,
                           dy,
                           wrapped,
                           imageSize,
                           imageMaxMip,
                           tileSize,
                           maxAnisotropy,
                           residencyMapOffsets,
                           residencyMap);

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

        IRReturn(vec4(texelFetchArray(sampler, atlasUV, 0)));

        IRFunctionEnd
    }

    vec4 sample_nearest_linear(Param<UInt> textureID,
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
                               ShaderObject &sampler) {
        IRFunction

        vec2 wrapped = wrapUV(uv, wrap);

        Float minMip = getResidentMip(textureID, wrapped, imageSize, tileSize, residencyMapOffsets, residencyMap);
        Float maxMip = Float(imageMaxMip);

        Float lod = getLod(dx, dy, imageSize, maxAnisotropy);
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

        vec4 s0 = vec4(texelFetchArray(sampler, atlasUV0, 0));
        vec4 s1 = vec4(texelFetchArray(sampler, atlasUV1, 0));

        IRReturn(vec4(mix(s0, s1, blend)));

        IRFunctionEnd
    }

    vec4 sample_bilinear(Param<UInt> textureID,
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
                         ShaderObject &sampler) {
        IRFunction

        vec2 wrapped = wrapUV(uv, wrap);

        Float mip = getMip(textureID,
                           dx,
                           dy,
                           wrapped,
                           imageSize,
                           imageMaxMip,
                           tileSize,
                           maxAnisotropy,
                           residencyMapOffsets,
                           residencyMap);

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

        vec2 adx = dx * mipSize / Float(atlasSize);
        vec2 ady = dy * mipSize / Float(atlasSize);

        IRReturn(vec4(textureGradArray(sampler, atlasUV, adx, ady)));

        IRFunctionEnd
    }

    vec4 sample_trilinear(Param<UInt> textureID,
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
                          ShaderObject &sampler) {
        IRFunction

        vec2 wrapped = wrapUV(uv, wrap);

        Float minMip = getResidentMip(textureID, wrapped, imageSize, tileSize, residencyMapOffsets, residencyMap);
        Float maxMip = Float(imageMaxMip);

        Float lod = getLod(dx, dy, imageSize, maxAnisotropy);
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

        vec2 adx0 = dx * mipSize0 / Float(atlasSize);
        vec2 ady0 = dy * mipSize0 / Float(atlasSize);
        vec2 adx1 = dx * mipSize1 / Float(atlasSize);
        vec2 ady1 = dy * mipSize1 / Float(atlasSize);

        vec4 s0 = vec4(textureGradArray(sampler, atlasUV0, adx0, ady0));
        vec4 s1 = vec4(textureGradArray(sampler, atlasUV1, adx1, ady1));

        IRReturn(vec4(mix(s0, s1, blend)));

        IRFunctionEnd
    }

    vec4 sample_bicubic(Param<UInt> textureID,
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
                        ShaderObject &sampler) {
        IRFunction

        vec2 wrapped = wrapUV(uv, wrap);

        // coarsest resident mip across the 4 catmull taps at a given mipSize
        auto residentFloor = [&](vec2 mipSize) -> Float {
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

            Float r = getResidentMip(textureID, wrapped, imageSize, tileSize, residencyMapOffsets, residencyMap);
            r = max(r, getResidentMip(textureID, uv00, imageSize, tileSize, residencyMapOffsets, residencyMap));
            r = max(r, getResidentMip(textureID, uv10, imageSize, tileSize, residencyMapOffsets, residencyMap));
            r = max(r, getResidentMip(textureID, uv01, imageSize, tileSize, residencyMapOffsets, residencyMap));
            r = max(r, getResidentMip(textureID, uv11, imageSize, tileSize, residencyMapOffsets, residencyMap));
            return r;
        };

        // single bicubic fetch at a fixed, already-resident mip
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

            vec2 adx = dx * mipSize / Float(atlasSize);
            vec2 ady = dy * mipSize / Float(atlasSize);

            vec3 a00 = getAtlasUV(textureID, uv00, Int(mip), mipSize, atlasSize, tileSize, tileBorder, tileMapOffsets,
                                  tileMap);
            vec3 a10 = getAtlasUV(textureID, uv10, Int(mip), mipSize, atlasSize, tileSize, tileBorder, tileMapOffsets,
                                  tileMap);
            vec3 a01 = getAtlasUV(textureID, uv01, Int(mip), mipSize, atlasSize, tileSize, tileBorder, tileMapOffsets,
                                  tileMap);
            vec3 a11 = getAtlasUV(textureID, uv11, Int(mip), mipSize, atlasSize, tileSize, tileBorder, tileMapOffsets,
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

        Float maxMip = Float(imageMaxMip);
        Float lod = getLod(dx, dy, imageSize, maxAnisotropy);

        // probe taps at the lod estimate, then unify to the coarsest resident mip
        Float estMip = clamp(floor(lod), 0.0f, maxMip);
        vec2 estSize = max(vec2(1.0f), vec2(imageSize) / exp2(estMip));

        Float r = residentFloor(estSize);
        Float mip = clamp(floor(lod), r, maxMip);
        vec2 mipSize = max(vec2(1.0f), vec2(imageSize) / exp2(mip));

        IRReturn(sampleBicubic(mip, mipSize));

        IRFunctionEnd
    }

    vec4 sample_bicubic_trilinear(Param<UInt> textureID,
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
                                  ShaderObject &sampler) {
        IRFunction

        vec2 wrapped = wrapUV(uv, wrap);

        // coarsest resident mip across the 4 catmull taps at a given mipSize
        auto residentFloor = [&](vec2 mipSize) -> Float {
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

            Float r = getResidentMip(textureID, uv00, imageSize, tileSize, residencyMapOffsets, residencyMap);
            r = max(r, getResidentMip(textureID, uv10, imageSize, tileSize, residencyMapOffsets, residencyMap));
            r = max(r, getResidentMip(textureID, uv01, imageSize, tileSize, residencyMapOffsets, residencyMap));
            r = max(r, getResidentMip(textureID, uv11, imageSize, tileSize, residencyMapOffsets, residencyMap));
            return r;
        };

        // single bicubic fetch at a fixed, already-resident mip
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

            vec2 adx = dx * mipSize / Float(atlasSize);
            vec2 ady = dy * mipSize / Float(atlasSize);

            vec3 a00 = getAtlasUV(textureID, uv00, Int(mip), mipSize, atlasSize, tileSize, tileBorder, tileMapOffsets,
                                  tileMap);
            vec3 a10 = getAtlasUV(textureID, uv10, Int(mip), mipSize, atlasSize, tileSize, tileBorder, tileMapOffsets,
                                  tileMap);
            vec3 a01 = getAtlasUV(textureID, uv01, Int(mip), mipSize, atlasSize, tileSize, tileBorder, tileMapOffsets,
                                  tileMap);
            vec3 a11 = getAtlasUV(textureID, uv11, Int(mip), mipSize, atlasSize, tileSize, tileBorder, tileMapOffsets,
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

        Float maxMip = Float(imageMaxMip);
        Float lod = getLod(dx, dy, imageSize, maxAnisotropy);

        // probe taps at the lod estimate, unify mip0 to the coarsest resident floor
        Float estMip = clamp(floor(lod), 0.0f, maxMip);
        vec2 estSize = max(vec2(1.0f), vec2(imageSize) / exp2(estMip));

        Float r = residentFloor(estSize);
        Float clampedLod = clamp(lod, r, maxMip);

        Float mip0 = floor(clampedLod);
        Float mip1 = min(mip0 + 1.0f, maxMip);
        Float blend = fract(clampedLod);

        vec2 mipSize0 = max(vec2(1.0f), vec2(imageSize) / exp2(mip0));
        vec2 mipSize1 = max(vec2(1.0f), vec2(imageSize) / exp2(mip1));

        IRReturn(vec4(mix(sampleBicubic(mip0, mipSize0), sampleBicubic(mip1, mipSize1), blend)));

        IRFunctionEnd
    }

    UInt readback_sample(Param<UInt> textureID,
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
                         DynamicBufferWrapper<UInt> &readback) {
        IRFunction

        vec2 dx = partialDerivativeX(uv);
        vec2 dy = partialDerivativeY(uv);

        // Correct for seam discontinuities before scaling
        // A seam-crossing derivative is ~±1.0 in UV space — round() cancels it
        dx = dx - round(dx);
        dy = dy - round(dy);

        Float lod = getLod(dx, dy, imageSize, maxAnisotropy);

        If(lod <= 0.0f)
            If(magFilter == FILTER_BICUBIC)
                IRReturn(
                    readback_sample_bicubic(textureID,
                        uv,
                        wrap,
                        imageSize,
                        imageMaxMip,
                        tileSize,
                        maxAnisotropy,
                        dx,
                        dy,
                        readbackOffsets,
                        readback));
            Else
                If(magFilter == FILTER_BILINEAR)
                    IRReturn(
                        readback_sample_bilinear(textureID,
                            uv,
                            wrap,
                            imageSize,
                            imageMaxMip,
                            tileSize,
                            maxAnisotropy,
                            dx,
                            dy,
                            readbackOffsets,
                            readback));
                Else
                    IRReturn(
                        readback_sample_nearest(textureID,
                            uv,
                            wrap,
                            imageSize,
                            imageMaxMip,
                            tileSize,
                            maxAnisotropy,
                            dx,
                            dy,
                            readbackOffsets,
                            readback));
                Fi
            Fi
        Else
            If(minFilter == FILTER_BICUBIC)
                If(mipFilter == rg::NEAREST)
                    IRReturn(
                        readback_sample_bicubic(textureID,
                            uv,
                            wrap,
                            imageSize,
                            imageMaxMip,
                            tileSize,
                            maxAnisotropy,
                            dx,
                            dy,
                            readbackOffsets,
                            readback));
                Else
                    IRReturn(
                        readback_sample_bicubic_trilinear(textureID,
                            uv,
                            wrap,
                            imageSize,
                            imageMaxMip,
                            tileSize,
                            maxAnisotropy,
                            dx,
                            dy,
                            readbackOffsets,
                            readback));
                Fi
            Else
                If(minFilter == FILTER_BILINEAR)
                    If(mipFilter == rg::NEAREST)
                        IRReturn(
                            readback_sample_bilinear(textureID,
                                uv,
                                wrap,
                                imageSize,
                                imageMaxMip,
                                tileSize,
                                maxAnisotropy,
                                dx,
                                dy,
                                readbackOffsets,
                                readback));
                    Else
                        IRReturn(
                            readback_sample_trilinear(textureID,
                                uv,
                                wrap,
                                imageSize,
                                imageMaxMip,
                                tileSize,
                                maxAnisotropy,
                                dx,
                                dy,
                                readbackOffsets,
                                readback));
                    Fi
                Else
                    If(mipFilter == rg::NEAREST)
                        IRReturn(
                            readback_sample_nearest(textureID,
                                uv,
                                wrap,
                                imageSize,
                                imageMaxMip,
                                tileSize,
                                maxAnisotropy,
                                dx,
                                dy,
                                readbackOffsets,
                                readback));
                    Else
                        IRReturn(
                            readback_sample_nearest_linear(textureID,
                                uv,
                                wrap,
                                imageSize,
                                imageMaxMip,
                                tileSize,
                                maxAnisotropy,
                                dx,
                                dy,
                                readbackOffsets,
                                readback));
                    Fi
                Fi
            Fi
        Fi

        IRFunctionEnd
    }

    UInt readback_sample_nearest(Param<UInt> textureID,
                                 Param<vec2> uv,
                                 Param<Int> wrap,
                                 Param<ivec2> imageSize,
                                 Param<UInt> imageMaxMip,
                                 Param<UInt> tileSize,
                                 Param<Float> maxAnisotropy,
                                 Param<vec2> dx,
                                 Param<vec2> dy,
                                 DynamicBufferWrapper<UInt> &readbackOffsets,
                                 DynamicBufferWrapper<UInt> &readback) {
        IRFunction

        vec2 wrapped = wrapUV(uv, wrap);

        Float mip = getMipReadback(dx, dy, imageSize, imageMaxMip, maxAnisotropy);
        vec2 mipSize = max(vec2(1.0f), vec2(imageSize) / exp2(mip));

        UInt tileIndex = getTileIndex(wrapped, mipSize, tileSize);
        UInt tileMapBaseOffset = readbackOffsets[textureID + UInt(mip)];

        atomicAdd(readback[tileMapBaseOffset + tileIndex], 1u);

        IRReturn(UInt(1));

        IRFunctionEnd
    }

    UInt readback_sample_nearest_linear(Param<UInt> textureID,
                                        Param<vec2> uv,
                                        Param<Int> wrap,
                                        Param<ivec2> imageSize,
                                        Param<UInt> imageMaxMip,
                                        Param<UInt> tileSize,
                                        Param<Float> maxAnisotropy,
                                        Param<vec2> dx,
                                        Param<vec2> dy,
                                        DynamicBufferWrapper<UInt> &readbackOffsets,
                                        DynamicBufferWrapper<UInt> &readback) {
        IRFunction

        vec2 wrapped = wrapUV(uv, wrap);

        Float maxMip = Float(imageMaxMip);

        Float lod = getLod(dx, dy, imageSize, maxAnisotropy);
        Float clampedLod = clamp(lod, Float(0.0f), maxMip);

        Float mip0 = floor(clampedLod);
        Float mip1 = min(mip0 + 1.0f, maxMip);

        vec2 mipSize0 = max(vec2(1.0f), vec2(imageSize) / exp2(mip0));
        vec2 mipSize1 = max(vec2(1.0f), vec2(imageSize) / exp2(mip1));

        UInt tileIndex0 = getTileIndex(wrapped, mipSize0, tileSize);
        UInt tileBaseOffset0 = readbackOffsets[textureID + UInt(mip0)];

        UInt tileIndex1 = getTileIndex(wrapped, mipSize1, tileSize);
        UInt tileBaseOffset1 = readbackOffsets[textureID + UInt(mip1)];

        atomicAdd(readback[tileBaseOffset0 + tileIndex0], 1u);
        atomicAdd(readback[tileBaseOffset1 + tileIndex1], 1u);

        IRReturn(UInt(2));

        IRFunctionEnd
    }

    UInt readback_sample_bilinear(Param<UInt> textureID,
                                  Param<vec2> uv,
                                  Param<Int> wrap,
                                  Param<ivec2> imageSize,
                                  Param<UInt> imageMaxMip,
                                  Param<UInt> tileSize,
                                  Param<Float> maxAnisotropy,
                                  Param<vec2> dx,
                                  Param<vec2> dy,
                                  DynamicBufferWrapper<UInt> &readbackOffsets,
                                  DynamicBufferWrapper<UInt> &readback) {
        IRFunction

        vec2 wrapped = wrapUV(uv, wrap);
        Float mip = getMipReadback(dx, dy, imageSize, imageMaxMip, maxAnisotropy);
        vec2 mipSize = max(vec2(1.0f), vec2(imageSize) / exp2(mip));

        UInt tileIndex = getTileIndex(wrapped, mipSize, tileSize);
        UInt tileMapBaseOffset = readbackOffsets[textureID + UInt(mip)];

        atomicAdd(readback[tileMapBaseOffset + tileIndex], 1u);

        IRReturn(UInt(1));

        IRFunctionEnd
    }

    UInt readback_sample_trilinear(Param<UInt> textureID,
                                   Param<vec2> uv,
                                   Param<Int> wrap,
                                   Param<ivec2> imageSize,
                                   Param<UInt> imageMaxMip,
                                   Param<UInt> tileSize,
                                   Param<Float> maxAnisotropy,
                                   Param<vec2> dx,
                                   Param<vec2> dy,
                                   DynamicBufferWrapper<UInt> &readbackOffsets,
                                   DynamicBufferWrapper<UInt> &readback) {
        IRFunction

        vec2 wrapped = wrapUV(uv, wrap);

        Float maxMip = Float(imageMaxMip);

        Float lod = getLod(dx, dy, imageSize, maxAnisotropy);
        Float clampedLod = clamp(lod, Float(0.0f), maxMip);

        Float mip0 = floor(clampedLod);
        Float mip1 = min(mip0 + 1.0f, maxMip);

        vec2 mipSize0 = max(vec2(1.0f), vec2(imageSize) / exp2(mip0));
        vec2 mipSize1 = max(vec2(1.0f), vec2(imageSize) / exp2(mip1));

        UInt tileIndex0 = getTileIndex(wrapped, mipSize0, tileSize);
        UInt tileBaseOffset0 = readbackOffsets[textureID + UInt(mip0)];

        UInt tileIndex1 = getTileIndex(wrapped, mipSize1, tileSize);
        UInt tileBaseOffset1 = readbackOffsets[textureID + UInt(mip1)];

        atomicAdd(readback[tileBaseOffset0 + tileIndex0], 1u);
        atomicAdd(readback[tileBaseOffset1 + tileIndex1], 1u);

        IRReturn(UInt(2));

        IRFunctionEnd
    }

    UInt readback_sample_bicubic(Param<UInt> textureID,
                                 Param<vec2> uv,
                                 Param<Int> wrap,
                                 Param<ivec2> imageSize,
                                 Param<UInt> imageMaxMip,
                                 Param<UInt> tileSize,
                                 Param<Float> maxAnisotropy,
                                 Param<vec2> dx,
                                 Param<vec2> dy,
                                 DynamicBufferWrapper<UInt> &readbackOffsets,
                                 DynamicBufferWrapper<UInt> &readback) {
        IRFunction

        vec2 wrapped = wrapUV(uv, wrap);
        Float mip = getMipReadback(dx, dy, imageSize, imageMaxMip, maxAnisotropy);

        vec2 mipSize = max(vec2(1.0f), vec2(imageSize) / exp2(mip));

        UInt tileIndex = getTileIndex(wrapped, mipSize, tileSize);
        UInt tileMapBaseOffset = readbackOffsets[textureID + UInt(mip)];

        atomicAdd(readback[tileMapBaseOffset + tileIndex], 1u);

        IRReturn(UInt(1));

        IRFunctionEnd
    }

    UInt readback_sample_bicubic_trilinear(Param<UInt> textureID,
                                           Param<vec2> uv,
                                           Param<Int> wrap,
                                           Param<ivec2> imageSize,
                                           Param<UInt> imageMaxMip,
                                           Param<UInt> tileSize,
                                           Param<Float> maxAnisotropy,
                                           Param<vec2> dx,
                                           Param<vec2> dy,
                                           DynamicBufferWrapper<UInt> &readbackOffsets,
                                           DynamicBufferWrapper<UInt> &readback) {
        IRFunction

        vec2 wrapped = wrapUV(uv, wrap);

        Float maxMip = Float(imageMaxMip);

        Float lod = getLod(dx, dy, imageSize, maxAnisotropy);
        Float clampedLod = clamp(lod, Float(0.0f), maxMip);

        Float mip0 = floor(clampedLod);
        Float mip1 = min(mip0 + 1.0f, maxMip);

        vec2 mipSize0 = max(vec2(1.0f), vec2(imageSize) / exp2(mip0));
        vec2 mipSize1 = max(vec2(1.0f), vec2(imageSize) / exp2(mip1));

        UInt tileIndex0 = getTileIndex(wrapped, mipSize0, tileSize);
        UInt tileBaseOffset0 = readbackOffsets[textureID + UInt(mip0)];

        UInt tileIndex1 = getTileIndex(wrapped, mipSize1, tileSize);
        UInt tileBaseOffset1 = readbackOffsets[textureID + UInt(mip1)];

        atomicAdd(readback[tileBaseOffset0 + tileIndex0], 1u);
        atomicAdd(readback[tileBaseOffset1 + tileIndex1], 1u);

        IRReturn(UInt(2));

        IRFunctionEnd
    }
}
