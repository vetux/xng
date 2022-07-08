/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2021  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef XENGINE_TEXTURE_HPP
#define XENGINE_TEXTURE_HPP

#include "graphics/texturebuffer.hpp"
#include "resource/resourcehandle.hpp"
#include "asset/image.hpp"

namespace xng {
    struct XENGINE_EXPORT Texture : public Resource {
        ~Texture() override = default;

        Resource *clone() override {
            return new Texture(*this);
        }

        std::type_index getTypeIndex() override {
            return typeid(Texture);
        }

        std::vector<ResourceHandle<ImageRGBA>> images;
        TextureBufferDesc textureDescription;

        bool clear = false;
        ColorRGBA clearColor;
    };
}

#endif //XENGINE_TEXTURE_HPP
