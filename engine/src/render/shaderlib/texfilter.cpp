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

#include "xng/render/shaderlib/texfilter.hpp"

#include "xng/rendergraph/shaderscript/shaderscript.hpp"

using namespace xng::ShaderScript;

namespace xng::shaderlib {
    ShaderNodeWrapper textureMS(const ShaderNodeWrapper &color,
                                const vec2 &uv,
                                const Int &samples) {
        ivec2 size = textureSize(color);
        ivec2 pos = ivec2(size.x() * uv.x(), size.y() * uv.y());

        vec4 ret = vec4(0, 0, 0, 0);


        Int i = 0;
        For(i, 0, i < samples, i + 1);

        ret += texelFetch(color, pos, i);

        EndFor();

        ret = ret / samples;

        return ret;
    }

    ShaderNodeWrapper cubic(const Float &v) {
        vec4 n = vec4(1.0, 2.0, 3.0, 4.0) - v;
        vec4 s = n * n * n;
        Float x = s.x();
        Float y = s.y() - 4.0 * s.x();
        Float z = s.z() - 4.0 * s.y() + 6.0 * s.x();
        Float w = 6.0 - x - y - z;
        return vec4(x, y, z, w) * (1.0 / 6.0);
    }

    ShaderFunction textureBicubic() {
        auto &builder = ShaderBuilder::instance();
        builder.setup({}, {}, {}, {}, {}, {}, {});

        builder.Function("textureBicubic",
                         {
                             {"sampler", ShaderTexture(TEXTURE_2D, RGBA)},
                             {"texCoords", ShaderDataType::vec2()}
                         },
                         ShaderDataType::vec4());

        vec2 texCoords = argument("texCoords");

        ivec2 texSize = textureSize(argument("sampler"));
        vec2 invTexSize = 1.0 / texSize;

        texCoords = texCoords * texSize - 0.5;


        vec2 fxy = fract(texCoords);
        texCoords -= fxy;

        vec4 xcubic = cubic(fxy.x());
        vec4 ycubic = cubic(fxy.y());

        vec4 c = vec4(texCoords.x(), texCoords.x(), texCoords.y(), texCoords.y())
                 + vec4(-0.5, 1.5, -0.5, 1.5);

        vec4 s = vec4(xcubic.x() + xcubic.y(),
                      xcubic.z() + xcubic.w(),
                      ycubic.x() + ycubic.y(),
                      ycubic.z() + ycubic.w());

        vec4 offset = c + vec4(xcubic.y(), xcubic.w(), ycubic.y(), ycubic.w()) / s;

        offset *= vec4(invTexSize.x(), invTexSize.x(), invTexSize.y(), invTexSize.y());

        vec4 sample0 = texture(argument("sampler"), vec2(offset.x(), offset.z()));
        vec4 sample1 = texture(argument("sampler"), vec2(offset.y(), offset.z()));
        vec4 sample2 = texture(argument("sampler"), vec2(offset.x(), offset.w()));
        vec4 sample3 = texture(argument("sampler"), vec2(offset.y(), offset.w()));

        Float sx = s.x() / (s.x() + s.y());
        Float sy = s.z() / (s.z() + s.w());

        Return(mix(mix(sample3, sample2, sx), mix(sample1, sample0, sx), sy));

        builder.EndFunction();

        return builder.getFunctions().at("textureBicubic");
    }

    ShaderFunction textureBicubicMS() {
        throw std::runtime_error("Not implemented");
    }

    ShaderFunction textureBicubicArray() {
        throw std::runtime_error("Not implemented");
    }
}
