/**
 *  xEngine - C++ game engine library
 *  Copyright (C) 2021  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef XENGINE_MATERIAL_HPP
#define XENGINE_MATERIAL_HPP

#include "asset/texture.hpp"

#include "resource/resourcehandle.hpp"
#include "asset/texture.hpp"

namespace xengine {
    struct XENGINE_EXPORT Material : public Resource {
        ~Material() override = default;

        Resource *clone() override {
            return new Material(*this);
        }

        ColorRGBA diffuse{};
        ColorRGBA ambient{};
        ColorRGBA specular{};
        ColorRGBA emissive{};
        float shininess{32};

        ResourceHandle<Texture> diffuseTexture;
        ResourceHandle<Texture> ambientTexture;
        ResourceHandle<Texture> specularTexture;
        ResourceHandle<Texture> emissiveTexture;
        ResourceHandle<Texture> shininessTexture;
        ResourceHandle<Texture> normalTexture;
    };
}

#endif //XENGINE_MATERIAL_HPP
