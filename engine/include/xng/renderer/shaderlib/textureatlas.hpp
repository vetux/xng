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

#ifndef XENGINE_TEXTUREATLAS_HPP
#define XENGINE_TEXTUREATLAS_HPP

#include "xng/shaderscript/shaderscript.hpp"

namespace xng::shaderlib::textureatlas {
    using namespace xng::ShaderScript;

    XENGINE_EXPORT vec4 sample_atlas(Param<Texture2DArray<rg::RGBA8> > texture,
                                     Param<vec2> inUv,
                                     Param<Int> layer,
                                     Param<vec2> scale,
                                     Param<vec2> size,
                                     Param<Int> minFilter,
                                     Param<Int> magFilter,
                                     Param<Int> mipFilter,
                                     Param<Int> wrap);
}

#endif //XENGINE_TEXTUREATLAS_HPP
