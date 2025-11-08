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

using namespace xng::ShaderScript;

DEFINE_FUNCTION3(textureMS)
DEFINE_FUNCTION1(cubic)

namespace xng::shaderlib {
    void defTextureMS() {
        Function("textureMS",
                 {
                     {ShaderTexture(TEXTURE_2D_MULTISAMPLE, RGBA), "color"},
                     {ShaderDataType::vec2(), "uv"},
                     {ShaderDataType::Int(), "samples"}
                 },
                 ShaderDataType::vec4());
        {
            ARGUMENT(TextureSampler, color)
            ARGUMENT(vec2, uv)
            ARGUMENT(Int, samples)

            ivec2 size = textureSize(color);
            ivec2 pos = ivec2(size.x() * uv.x(), size.y() * uv.y());

            vec4 ret;
            ret = vec4(0, 0, 0, 0);

            Int i;
            i = Int(0);
            For(i, 0, samples - 1, 1)
            {
                ret += texelFetchMS(color, pos, i);
            }
            EndFor

            ret = ret / samples;

            Return(ret);
        }
        EndFunction();
    }

    void defCubic() {
        Function("cubic", {{ShaderDataType::Float(), "v"}}, ShaderDataType::vec4());
        {
            ARGUMENT(Float, v)
            vec4 n = vec4(1.0f, 2.0f, 3.0f, 4.0f) - v;
            vec4 s = n * n * n;
            Float x = s.x();
            Float y = s.y() - 4.0f * s.x();
            Float z = s.z() - 4.0f * s.y() + 6.0f * s.x();
            Float w = 6.0f - x - y - z;
            Return(vec4(x, y, z, w) * (1.0f / 6.0f));
        }
        EndFunction();
    }

    void textureBicubic() {
        defCubic();
        defTextureMS();

        Function("textureBicubic",
                 {
                     {ShaderTexture(TEXTURE_2D, RGBA), "sampler"},
                     {ShaderDataType::vec2(), "texCoords"}
                 },
                 ShaderDataType::vec4());
        {
            ARGUMENT(TextureSampler, sampler)
            ARGUMENT(vec2, texCoords)

            vec2 texSize;
            texSize = textureSize(sampler, 0);
            vec2 invTexSize;
            invTexSize = 1.0f / texSize;

            texCoords = texCoords * texSize - 0.5f;

            vec2 fxy = fract(texCoords);
            texCoords -= fxy;

            vec4 xcubic = cubic(fxy.x());
            vec4 ycubic = cubic(fxy.y());

            vec4 c = texCoords.xxyy() + vec2(-0.5f, +1.5f).xyxy();

            vec4 s = vec4(xcubic.xz() + xcubic.yw(), ycubic.xz() + ycubic.yw());
            vec4 offset = c + vec4(xcubic.yw(), ycubic.yw()) / s;

            offset *= invTexSize.xxyy();

            vec4 sample0 = textureSample(sampler, offset.xz());
            vec4 sample1 = textureSample(sampler, offset.yz());
            vec4 sample2 = textureSample(sampler, offset.xw());
            vec4 sample3 = textureSample(sampler, offset.yw());

            Float sx = s.x() / (s.x() + s.y());
            Float sy = s.z() / (s.z() + s.w());

            Return(mix(mix(sample3, sample2, sx), mix(sample1, sample0, sx), sy));
        }
        EndFunction();

        Function("textureBicubic",
                 {
                     {ShaderTexture(TEXTURE_2D_MULTISAMPLE, RGBA), "sampler"},
                     {ShaderDataType::vec2(), "texCoords"},
                     {ShaderDataType::Int(), "samples"}
                 },
                 ShaderDataType::vec4());
        {
            ARGUMENT(TextureSampler, sampler)
            ARGUMENT(vec2, texCoords)
            ARGUMENT(Int, samples)

            vec2 texSize;
            texSize = textureSize(sampler);
            vec2 invTexSize;
            invTexSize = 1.0 / texSize;

            texCoords = texCoords * texSize - 0.5f;

            vec2 fxy = fract(texCoords);
            texCoords -= fxy;

            vec4 xcubic = cubic(fxy.x());
            vec4 ycubic = cubic(fxy.y());

            vec4 c = texCoords.xxyy() + vec2(-0.5f, +1.5f).xyxy();

            vec4 s = vec4(xcubic.xz() + xcubic.yw(), ycubic.xz() + ycubic.yw());
            vec4 offset = c + vec4(xcubic.yw(), ycubic.yw()) / s;

            offset *= invTexSize.xxyy();

            vec4 sample0 = textureMS(sampler, offset.xz(), samples);
            vec4 sample1 = textureMS(sampler, offset.yz(), samples);
            vec4 sample2 = textureMS(sampler, offset.xw(), samples);
            vec4 sample3 = textureMS(sampler, offset.yw(), samples);

            Float sx = s.x() / (s.x() + s.y());
            Float sy = s.z() / (s.z() + s.w());

            Return(mix(mix(sample3, sample2, sx), mix(sample1, sample0, sx), sy));
        }
        EndFunction();

        // TODO: Sample texture arrays with integer coordinates / texelFetch
        Function("textureBicubic",
                 {
                     {ShaderTexture(TEXTURE_2D_ARRAY, RGBA), "sampler"},
                     {ShaderDataType::vec3(), "texCoords3"},
                     {ShaderDataType::vec2(), "size"}
                 },
                 ShaderDataType::vec4());
        {
            ARGUMENT(TextureSampler, sampler)
            ARGUMENT(vec3, texCoords3)
            ARGUMENT(vec2, size)

            vec2 texCoords = texCoords3.xy();

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

            vec4 sample0 = textureSampleArray(sampler, vec3(offset.x(), offset.z(), texCoords3.z()));
            vec4 sample1 = textureSampleArray(sampler, vec3(offset.y(), offset.z(), texCoords3.z()));
            vec4 sample2 = textureSampleArray(sampler, vec3(offset.x(), offset.w(), texCoords3.z()));
            vec4 sample3 = textureSampleArray(sampler, vec3(offset.y(), offset.w(), texCoords3.z()));

            Float sx = s.x() / (s.x() + s.y());
            Float sy = s.z() / (s.z() + s.w());

            Return(mix(mix(sample3, sample2, sx), mix(sample1, sample0, sx), sy));
        }
        EndFunction();
    }
}
