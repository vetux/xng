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

#include "xng/renderer/shaderlib/texfilter.hpp"

#include "xng/shaderscript/shaderscript.hpp"
#include "xng/shaderscript/macro/helpermacros.hpp"

// TODO: Handle Texture Mip filtering in custom texture sampling.
// TODO: Handle Texture Wrapping in custom texture sampling (Repeat mode for tiling etc)
// TODO: Handle sRGB decode

using namespace xng::ShaderScript;

namespace xng::shaderlib {
    vec4 textureMS(Param<Texture2DMS<rg::RGBA8> > color, Param<vec2> uv, Param<Int> samples) {
        IRFunction

        ivec2 size = textureSize(color);
        ivec2 pos = ivec2(size.x() * uv.value().x(), size.y() * uv.value().y());

        vec4 ret;
        ret = vec4(0, 0, 0, 0);

        For(Int, i, Int(0), i < samples, i + 1)
            ret += texelFetchMS(color, pos, i);
        Done

        ret = ret / samples;

        IRReturn(ret);
        IRFunctionEnd
    }

    vec4 cubic(Param<Float> v) {
        IRFunction
        vec4 n = vec4(1.0f, 2.0f, 3.0f, 4.0f) - v;
        vec4 s = n * n * n;
        Float x = s.x();
        Float y = s.y() - 4.0f * s.x();
        Float z = s.z() - 4.0f * s.y() + 6.0f * s.x();
        Float w = 6.0f - x - y - z;
        IRReturn(vec4(vec4(x, y, z, w) * (1.0f / 6.0f)));
        IRFunctionEnd
    }

    vec4 texfilter::textureBicubic(Param<Texture2D<rg::RGBA8> > texture, Param<vec2> uv) {
        IRFunction
        vec2 texSize;
        texSize = textureSize(texture, 0);
        vec2 invTexSize;
        invTexSize = 1.0f / texSize;

        vec2 texCoords = uv * texSize - 0.5f;

        vec2 fxy = fract(texCoords);
        texCoords -= fxy;

        vec4 xcubic = cubic(fxy.x());
        vec4 ycubic = cubic(fxy.y());

        vec4 c = texCoords.xxyy() + vec2(-0.5f, +1.5f).xyxy();

        vec4 s = vec4(xcubic.xz() + xcubic.yw(), ycubic.xz() + ycubic.yw());
        vec4 offset = c + vec4(xcubic.yw(), ycubic.yw()) / s;

        offset *= invTexSize.xxyy();

        vec4 sample0 = textureSample(texture, offset.xz());
        vec4 sample1 = textureSample(texture, offset.yz());
        vec4 sample2 = textureSample(texture, offset.xw());
        vec4 sample3 = textureSample(texture, offset.yw());

        Float sx = s.x() / (s.x() + s.y());
        Float sy = s.z() / (s.z() + s.w());

        IRReturn(vec4(mix(mix(sample3, sample2, sx), mix(sample1, sample0, sx), sy)));
        IRFunctionEnd
    }

    vec4 texfilter::textureBicubicMS(Param<Texture2DMS<rg::RGBA8> > texture, Param<vec2> uv, Param<Int> samples) {
        IRFunction
        vec2 texSize;
        texSize = textureSize(texture);
        vec2 invTexSize;
        invTexSize = 1.0 / texSize;

        vec2 texCoords = uv * texSize - 0.5f;

        vec2 fxy = fract(texCoords);
        texCoords -= fxy;

        vec4 xcubic = cubic(fxy.x());
        vec4 ycubic = cubic(fxy.y());

        vec4 c = texCoords.xxyy() + vec2(-0.5f, +1.5f).xyxy();

        vec4 s = vec4(xcubic.xz() + xcubic.yw(), ycubic.xz() + ycubic.yw());
        vec4 offset = c + vec4(xcubic.yw(), ycubic.yw()) / s;

        offset *= invTexSize.xxyy();

        vec4 sample0 = textureMS(texture, offset.xz(), samples);
        vec4 sample1 = textureMS(texture, offset.yz(), samples);
        vec4 sample2 = textureMS(texture, offset.xw(), samples);
        vec4 sample3 = textureMS(texture, offset.yw(), samples);

        Float sx = s.x() / (s.x() + s.y());
        Float sy = s.z() / (s.z() + s.w());

        IRReturn(vec4(mix(mix(sample3, sample2, sx), mix(sample1, sample0, sx), sy)));
        IRFunctionEnd
    }

    vec4 texfilter::textureBicubicArray(Param<Texture2DArray<rg::RGBA8> > texture,
                                        Param<vec3> uv,
                                        Param<vec2> size) {
        IRFunction

        vec2 tc = uv.value().xy() * size - 0.5f;
        ivec2 p = ivec2(Int(floor(tc.x())), Int(floor(tc.y())));
        vec2 fxy = tc - vec2(Float(p.x()), Float(p.y()));
        vec4 wx = cubic(fxy.x());
        vec4 wy = cubic(fxy.y());
        Int layer = Int(uv.value().z());

        vec4 row0 = texelFetchArray(texture, ivec3(p.x() - 1, p.y() - 1, layer), 0) * wx.x()
                    + texelFetchArray(texture, ivec3(p.x() + 0, p.y() - 1, layer), 0) * wx.y()
                    + texelFetchArray(texture, ivec3(p.x() + 1, p.y() - 1, layer), 0) * wx.z()
                    + texelFetchArray(texture, ivec3(p.x() + 2, p.y() - 1, layer), 0) * wx.w();
        vec4 row1 = texelFetchArray(texture, ivec3(p.x() - 1, p.y() + 0, layer), 0) * wx.x()
                    + texelFetchArray(texture, ivec3(p.x() + 0, p.y() + 0, layer), 0) * wx.y()
                    + texelFetchArray(texture, ivec3(p.x() + 1, p.y() + 0, layer), 0) * wx.z()
                    + texelFetchArray(texture, ivec3(p.x() + 2, p.y() + 0, layer), 0) * wx.w();
        vec4 row2 = texelFetchArray(texture, ivec3(p.x() - 1, p.y() + 1, layer), 0) * wx.x()
                    + texelFetchArray(texture, ivec3(p.x() + 0, p.y() + 1, layer), 0) * wx.y()
                    + texelFetchArray(texture, ivec3(p.x() + 1, p.y() + 1, layer), 0) * wx.z()
                    + texelFetchArray(texture, ivec3(p.x() + 2, p.y() + 1, layer), 0) * wx.w();
        vec4 row3 = texelFetchArray(texture, ivec3(p.x() - 1, p.y() + 2, layer), 0) * wx.x()
                    + texelFetchArray(texture, ivec3(p.x() + 0, p.y() + 2, layer), 0) * wx.y()
                    + texelFetchArray(texture, ivec3(p.x() + 1, p.y() + 2, layer), 0) * wx.z()
                    + texelFetchArray(texture, ivec3(p.x() + 2, p.y() + 2, layer), 0) * wx.w();

        IRReturn(vec4(row0 * wy.x() + row1 * wy.y() + row2 * wy.z() + row3 * wy.w()));
        IRFunctionEnd
    }

    vec4 texfilter::textureBicubicArrayLod(Param<Texture2DArray<rg::RGBA8> > texture,
                                           Param<vec3> uv,
                                           Param<vec2> size,
                                           Param<Float> lod) {
        IRFunction

        Int ilod = Int(lod);
        vec2 mipSize = size * pow(2.0f, lod * -1.0f); // correct size for this mip level
        vec2 tc = uv.value().xy() * mipSize - 0.5f;
        ivec2 p = ivec2(Int(floor(tc.x())), Int(floor(tc.y())));
        vec2 fxy = tc - vec2(Float(p.x()), Float(p.y()));
        vec4 wx = cubic(fxy.x());
        vec4 wy = cubic(fxy.y());
        Int layer = Int(uv.value().z());

        vec4 row0 = texelFetchArray(texture, ivec3(p.x() - 1, p.y() - 1, layer), ilod) * wx.x()
                    + texelFetchArray(texture, ivec3(p.x() + 0, p.y() - 1, layer), ilod) * wx.y()
                    + texelFetchArray(texture, ivec3(p.x() + 1, p.y() - 1, layer), ilod) * wx.z()
                    + texelFetchArray(texture, ivec3(p.x() + 2, p.y() - 1, layer), ilod) * wx.w();
        vec4 row1 = texelFetchArray(texture, ivec3(p.x() - 1, p.y() + 0, layer), ilod) * wx.x()
                    + texelFetchArray(texture, ivec3(p.x() + 0, p.y() + 0, layer), ilod) * wx.y()
                    + texelFetchArray(texture, ivec3(p.x() + 1, p.y() + 0, layer), ilod) * wx.z()
                    + texelFetchArray(texture, ivec3(p.x() + 2, p.y() + 0, layer), ilod) * wx.w();
        vec4 row2 = texelFetchArray(texture, ivec3(p.x() - 1, p.y() + 1, layer), ilod) * wx.x()
                    + texelFetchArray(texture, ivec3(p.x() + 0, p.y() + 1, layer), ilod) * wx.y()
                    + texelFetchArray(texture, ivec3(p.x() + 1, p.y() + 1, layer), ilod) * wx.z()
                    + texelFetchArray(texture, ivec3(p.x() + 2, p.y() + 1, layer), ilod) * wx.w();
        vec4 row3 = texelFetchArray(texture, ivec3(p.x() - 1, p.y() + 2, layer), ilod) * wx.x()
                    + texelFetchArray(texture, ivec3(p.x() + 0, p.y() + 2, layer), ilod) * wx.y()
                    + texelFetchArray(texture, ivec3(p.x() + 1, p.y() + 2, layer), ilod) * wx.z()
                    + texelFetchArray(texture, ivec3(p.x() + 2, p.y() + 2, layer), ilod) * wx.w();

        IRReturn(vec4(row0 * wy.x() + row1 * wy.y() + row2 * wy.z() + row3 * wy.w()));
        IRFunctionEnd
    }

    vec4 texfilter::textureBilinearArray(Param<Texture2DArray<rg::RGBA8> > texture,
                                         Param<vec3> uv,
                                         Param<vec2> size) {
        IRFunction

        vec2 tc = uv.value().xy() * size - 0.5f;
        ivec2 p = ivec2(Int(floor(tc.x())), Int(floor(tc.y())));
        vec2 fxy = tc - vec2(Float(p.x()), Float(p.y()));
        Int layer = Int(uv.value().z());
        ivec2 maxC = ivec2(Int(size.value().x()) - 1, Int(size.value().y()) - 1);

        vec4 s00 = texelFetchArray(texture, ivec3(clamp(p.x(), 0, maxC.x()), clamp(p.y(), 0, maxC.y()), layer), 0);
        vec4 s10 = texelFetchArray(texture, ivec3(clamp(p.x() + 1, 0, maxC.x()), clamp(p.y(), 0, maxC.y()), layer), 0);
        vec4 s01 = texelFetchArray(texture, ivec3(clamp(p.x(), 0, maxC.x()), clamp(p.y() + 1, 0, maxC.y()), layer), 0);
        vec4 s11 = texelFetchArray(texture, ivec3(clamp(p.x() + 1, 0, maxC.x()), clamp(p.y() + 1, 0, maxC.y()), layer), 0);

        IRReturn(vec4(mix(mix(s00, s10, fxy.x()), mix(s01, s11, fxy.x()), fxy.y())));
        IRFunctionEnd
    }

    vec4 texfilter::textureBilinearArrayLod(Param<Texture2DArray<rg::RGBA8> > texture,
                                            Param<vec3> uv,
                                            Param<vec2> size,
                                            Param<Float> lod) {
        IRFunction

        Int ilod = Int(lod);
        vec2 mipSize = size * pow(2.0f, lod * -1.0f);
        vec2 tc = uv.value().xy() * mipSize - 0.5f;
        ivec2 p = ivec2(Int(floor(tc.x())), Int(floor(tc.y())));
        vec2 fxy = tc - vec2(Float(p.x()), Float(p.y()));
        Int layer = Int(uv.value().z());
        ivec2 maxC = ivec2(Int(mipSize.x()) - 1, Int(mipSize.y()) - 1);

        vec4 s00 = texelFetchArray(texture, ivec3(clamp(p.x(), 0, maxC.x()), clamp(p.y(), 0, maxC.y()), layer), ilod);
        vec4 s10 = texelFetchArray(texture, ivec3(clamp(p.x() + 1, 0, maxC.x()), clamp(p.y(), 0, maxC.y()), layer), ilod);
        vec4 s01 = texelFetchArray(texture, ivec3(clamp(p.x(), 0, maxC.x()), clamp(p.y() + 1, 0, maxC.y()), layer), ilod);
        vec4 s11 = texelFetchArray(texture, ivec3(clamp(p.x() + 1, 0, maxC.x()), clamp(p.y() + 1, 0, maxC.y()), layer),
                              ilod);

        IRReturn(vec4(mix(mix(s00, s10, fxy.x()), mix(s01, s11, fxy.x()), fxy.y())));
        IRFunctionEnd
    }
}
