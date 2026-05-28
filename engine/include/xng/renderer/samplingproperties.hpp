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

#ifndef XENGINE_SAMPLINGPROPERTIES_HPP
#define XENGINE_SAMPLINGPROPERTIES_HPP

#include "xng/rendergraph/textureproperties.hpp"

namespace xng {
    struct SamplingProperties {
        rg::TextureFiltering minFilter = rg::LINEAR;
        rg::TextureFiltering magFilter = rg::LINEAR;
        rg::TextureFiltering mipFilter = rg::LINEAR;
        rg::TextureWrapping wrapping = rg::CLAMP_TO_EDGE;

        SamplingProperties() = default;

        SamplingProperties(const rg::TextureFiltering minFilter,
                           const rg::TextureFiltering magFilter,
                           const rg::TextureFiltering mipFilter,
                           const rg::TextureWrapping wrapping)
            : minFilter(minFilter),
              magFilter(magFilter),
              mipFilter(mipFilter),
              wrapping(wrapping) {
        }
    };
}

#endif //XENGINE_SAMPLINGPROPERTIES_HPP
