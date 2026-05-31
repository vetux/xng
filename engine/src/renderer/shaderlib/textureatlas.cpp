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

#include "xng/renderer/shaderlib/textureatlas.hpp"
#include "xng/renderer/shaderlib/texfilter.hpp"

#include "xng/renderer/samplingproperties.hpp"

#include "xng/shaderscript/macro/helpermacros.hpp"

namespace xng::shaderlib::textureatlas {
    using namespace xng::shaderlib::texfilter;

    vec4 sample_atlas(Param<Texture2DArray<rg::RGBA8> > texture,
                      Param<vec2> inUv,
                      Param<Int> layer,
                      Param<vec2> scale,
                      Param<vec2> size,
                      Param<Int> minFilter,
                      Param<Int> magFilter,
                      Param<Int> mipFilter,
                      Param<Int> wrap) {
        IRFunction

        vec2 wrappedUv;
        If(wrap == WRAP_REPEAT)
            wrappedUv = fract(inUv);
        Else
            If(wrap == WRAP_CLAMP_TO_EDGE)
                wrappedUv = clamp(inUv, vec2(0.0f), vec2(1.0f));
            Else
                wrappedUv = 1.0f - abs(mod(inUv, vec2(2.0f)) - 1.0f);
            Fi
        Fi

        vec2 uv = wrappedUv * scale;
        vec2 imagePixelSize = size;

        vec2 dx = partialDerivativeX(inUv);
        vec2 dy = partialDerivativeY(inUv);

        // Correct for seam discontinuities before scaling
        // A seam-crossing derivative is ~±1.0 in UV space — round() cancels it
        dx = dx - round(dx);
        dy = dy - round(dy);

        // Now scale to pixel space for LoD
        dx = dx * imagePixelSize;
        dy = dy * imagePixelSize;

        Float lod = 0.5f * log2(max(dot(dx, dx), dot(dy, dy)) + 1e-10f);

        Float maxLod = log2(min(imagePixelSize.x(), imagePixelSize.y()));
        lod = clamp(lod, 0.0f, maxLod);
        Float lodFloor = floor(lod);
        Float lodCeil = min(lodFloor + 1.0f, maxLod);
        Float lodFrac = fract(lod);

        vec4 ret;
        If(lod <= 0.0f)
            If(magFilter == FILTER_BICUBIC)
                ret = textureBicubicArray(texture,
                                          vec3(wrappedUv.x(), wrappedUv.y(), layer),
                                          imagePixelSize,
                                          wrap);
            Else
                If(magFilter == FILTER_BILINEAR)
                    ret = textureBilinearArray(texture,
                                               vec3(wrappedUv.x(), wrappedUv.y(), layer),
                                               imagePixelSize,
                                               wrap);
                Else
                    ret = textureSampleArray(texture, vec3(uv.x(), uv.y(), layer));
                Fi
            Fi
        Else
            If(minFilter == FILTER_BICUBIC)
                If(mipFilter == rg::NEAREST)
                    ret = textureBicubicArrayLod(texture,
                                                 vec3(wrappedUv.x(), wrappedUv.y(), layer),
                                                 imagePixelSize,
                                                 lodFloor,
                                                 wrap);
                Else
                    vec4 s0 = textureBicubicArrayLod(texture,
                                                     vec3(wrappedUv.x(), wrappedUv.y(), layer),
                                                     imagePixelSize,
                                                     lodFloor,
                                                     wrap);
                    vec4 s1 = textureBicubicArrayLod(texture,
                                                     vec3(wrappedUv.x(), wrappedUv.y(), layer),
                                                     imagePixelSize,
                                                     lodCeil,
                                                     wrap);
                    ret = mix(s0, s1, lodFrac);
                Fi
            Else
                If(minFilter == FILTER_BILINEAR)
                    If(mipFilter == rg::NEAREST)
                        ret = textureBilinearArrayLod(texture,
                                                      vec3(wrappedUv.x(), wrappedUv.y(), layer),
                                                      imagePixelSize,
                                                      lodFloor,
                                                      wrap);
                    Else
                        vec4 s0 = textureBilinearArrayLod(texture,
                                                          vec3(wrappedUv.x(), wrappedUv.y(), layer),
                                                          imagePixelSize,
                                                          lodFloor,
                                                          wrap);
                        vec4 s1 = textureBilinearArrayLod(texture,
                                                          vec3(wrappedUv.x(), wrappedUv.y(), layer),
                                                          imagePixelSize,
                                                          lodCeil,
                                                          wrap);
                        ret = mix(s0, s1, lodFrac);
                    Fi
                Else
                    If(mipFilter == rg::NEAREST)
                        ret = textureSampleArrayLod(texture, vec3(uv.x(), uv.y(), layer), lodFloor);
                    Else
                        vec4 s0 = textureSampleArrayLod(texture, vec3(uv.x(), uv.y(), layer), lodFloor);
                        vec4 s1 = textureSampleArrayLod(texture, vec3(uv.x(), uv.y(), layer), lodCeil);
                        ret = mix(s0, s1, lodFrac);
                    Fi
                Fi
            Fi
        Fi

        IRReturn(ret);

        IRFunctionEnd
    }
}
