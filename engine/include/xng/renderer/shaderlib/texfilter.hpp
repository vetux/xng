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

#ifndef XENGINE_TEXFILTER_HPP
#define XENGINE_TEXFILTER_HPP

#include "xng/shaderscript/shaderscript.hpp"

namespace xng::shaderlib::texfilter {
    using namespace xng::ShaderScript;

    XENGINE_EXPORT vec4 textureBicubic(Param<Texture2D<rg::RGBA8> > texture, Param<vec2> uv);

    XENGINE_EXPORT vec4 textureBicubicMS(Param<Texture2DMS<rg::RGBA8> > texture, Param<vec2> uv, Param<Int> samples);

    XENGINE_EXPORT vec4 textureBicubicArray(Param<Texture2DArray<rg::RGBA8> > texture,
                                            Param<vec3> uv,
                                            Param<vec2> size,
                                            Param<Int> wrap);

    XENGINE_EXPORT vec4 textureBicubicArrayLod(Param<Texture2DArray<rg::RGBA8> > texture,
                                               Param<vec3> uv,
                                               Param<vec2> size,
                                               Param<Float> lod,
                                               Param<Int> wrap);

    XENGINE_EXPORT vec4 textureBilinearArray(Param<Texture2DArray<rg::RGBA8> > texture,
                                             Param<vec3> uv,
                                             Param<vec2> size,
                                             Param<Int> wrap);

    XENGINE_EXPORT vec4 textureBilinearArrayLod(Param<Texture2DArray<rg::RGBA8> > texture,
                                                Param<vec3> uv,
                                                Param<vec2> size,
                                                Param<Float> lod,
                                                Param<Int> wrap);
}


#endif //XENGINE_TEXFILTER_HPP
