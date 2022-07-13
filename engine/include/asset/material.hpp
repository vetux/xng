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

#ifndef XENGINE_MATERIAL_HPP
#define XENGINE_MATERIAL_HPP

#include "asset/texture.hpp"
#include "resource/resourcehandle.hpp"
#include "io/messageable.hpp"

namespace xng {
    struct XENGINE_EXPORT Material : public Resource, public Messageable {
        ~Material() override = default;

        std::unique_ptr<Resource> clone() override {
            return std::make_unique<Material>(*this);
        }

        std::type_index getTypeIndex() override {
            return typeid(Material);
        }

        Messageable &operator<<(const Message &message) override {
            diffuse << message.value("diffuse");
            ambient << message.value("ambient");
            specular << message.value("specular");
            emissive << message.value("emissive");
            shininess = message.value("shininess", 32.0f);
            diffuseTexture << message.value("diffuseTexture");
            ambientTexture << message.value("ambientTexture");
            specularTexture << message.value("specularTexture");
            emissiveTexture <<message.value("emissiveTexture");
            shininessTexture << message.value("shininessTexture");
            normalTexture << message.value("normalTexture");
            return *this;
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            diffuse >> message["diffuse"];
            ambient >> message["ambient"];
            specular >> message["specular"];
            emissive >> message["emissive"];
            message["shininess"] = shininess;
            if (diffuseTexture)
                diffuseTexture >> message["diffuseTexture"];
            if (ambientTexture)
                ambientTexture >> message["ambientTexture"];
            if (specularTexture)
                specularTexture >> message["specularTexture"];
            if (emissiveTexture)
                emissiveTexture >> message["emissiveTexture"];
            if (shininessTexture)
                shininessTexture >> message["shininessTexture"];
            if (normalTexture)
                normalTexture >> message["normalTexture"];
            return message;
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
