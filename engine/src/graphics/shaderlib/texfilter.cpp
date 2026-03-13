/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
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

#include "xng/graphics/shaderlib/texfilter.hpp"

#include "xng/rendergraph/shaderscript/shaderscript.hpp"
#include "xng/rendergraph/shaderscript/macro/helpermacros.hpp"

using namespace xng::ShaderScript;

namespace xng::shaderlib
{
    vec4 textureMS(Param<Texture2DMS<RGBA>> color, Param<vec2> uv, Param<Int> samples)
    {
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

    vec4 cubic(Param<Float> v)
    {
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

    vec4 texfilter::textureBicubic(Param<Texture2D<RGBA>> texture, Param<vec2> uv)
    {
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

    vec4 texfilter::textureBicubicMS(Param<Texture2DMS<RGBA>> texture, Param<vec2> uv, Param<Int> samples)
    {
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

    // TODO: Sample texture arrays with integer coordinates / texelFetch
    vec4 texfilter::textureBicubicArray(Param<Texture2DArray<RGBA>> texture, Param<vec3> uv, Param<vec2> size)
    {
        IRFunction
        vec2 texCoords = uv.value().xy();

        vec2 invTexSize = 1.0f / size;

        texCoords = texCoords * size - 0.5f;

        vec2 fxy = fract(texCoords);
        texCoords -= fxy;

        vec4 xcubic = cubic(fxy.x());
        vec4 ycubic = cubic(fxy.y());

        vec4 c = texCoords.xxyy() + vec2(-0.5f, +1.5f).xyxy();

        vec4 s = vec4(xcubic.xz() + xcubic.yw(), ycubic.xz() + ycubic.yw());
        vec4 offset = c + vec4(xcubic.yw(), ycubic.yw()) / s;

        offset *= invTexSize.xxyy();

        vec4 sample0 = textureSampleArray(texture, vec3(offset.x(), offset.z(), uv.value().z()));
        vec4 sample1 = textureSampleArray(texture, vec3(offset.y(), offset.z(), uv.value().z()));
        vec4 sample2 = textureSampleArray(texture, vec3(offset.x(), offset.w(), uv.value().z()));
        vec4 sample3 = textureSampleArray(texture, vec3(offset.y(), offset.w(), uv.value().z()));

        Float sx = s.x() / (s.x() + s.y());
        Float sy = s.z() / (s.z() + s.w());

        IRReturn(vec4(mix(mix(sample3, sample2, sx), mix(sample1, sample0, sx), sy)));
        IRFunctionEnd
    }
}
