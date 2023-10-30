/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2023  Julian Zampiccoli
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

#include <variant>

#include "xng/render/texture.hpp"
#include "xng/shader/shader.hpp"

#include "xng/resource/resourcehandle.hpp"

#include "xng/io/messageable.hpp"

namespace xng {
    enum ShadingModel : int {
        SHADE_PBR = 0, // PBR Shading
        SHADE_PHONG = 1, // Per fragment phong shading
        SHADE_PHONG_GOURAUD = 2, // Per vertex phong shading
        SHADE_PHONG_FLAT = 3 // Per polygon phong shading
    };

    struct XENGINE_EXPORT Material : public Resource, public Messageable {
        ~Material() override = default;

        std::unique_ptr<Resource> clone() override {
            return std::make_unique<Material>(*this);
        }

        std::type_index getTypeIndex() const override {
            return typeid(Material);
        }

        Messageable &operator<<(const Message &message) override {
            message.value("model", (int &) shadingModel, (int) ShadingModel::SHADE_PBR);
            message.value("shader", shader);
            message.value("normal", normal);

            message.value("normalIntensity", normalIntensity);

            message.value("transparent", transparent, false);

            message.value("diffuse", diffuse);
            message.value("ambient", ambient);
            message.value("specular", specular);
            message.value("shininess", shininess);

            message.value("diffuseTexture", diffuseTexture);
            message.value("ambientTexture", ambientTexture);
            message.value("specularTexture", specularTexture);
            message.value("shininessTexture", shininessTexture);

            message.value("metallic", metallic);
            message.value("roughness", roughness, 1.0f);
            message.value("ambientOcclusion", ambientOcclusion, 1.0f);

            message.value("metallicTexture", metallicTexture);
            message.value("roughnessTexture", roughnessTexture);
            message.value("ambientOcclusionTexture", ambientOcclusionTexture);

            return *this;
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);

            shadingModel >> message["model"];
            shader >> message["shader"];
            normal >> message["normal"];

            normalIntensity >> message["normalIntensity"];

            transparent >> message["transparent"];

            diffuse >> message["diffuse"];
            ambient >> message["ambient"];
            specular >> message["specular"];
            shininess >> message["shininess"];

            diffuseTexture >> message["diffuseTexture"];
            ambientTexture >> message["ambientTexture"];
            specularTexture >> message["specularTexture"];
            shininessTexture >> message["shininessTexture"];

            metallic >> message["metallic"];
            roughness >> message["roughness"];
            ambientOcclusion >> message["ambientOcclusion"];

            metallicTexture >> message["metallicTexture"];
            roughnessTexture >> message["roughnessTexture"];
            ambientOcclusionTexture >> message["ambientOcclusionTexture"];

            return message;
        }

        ShadingModel shadingModel = SHADE_PHONG;

        /**
         * Optional user specified shader.
         *
         * Currently only glsl user shaders are supported.
         *
         * User shaders are always drawn using forward shading.
         */
        ResourceHandle<Shader> shader;

        /**
         * If assigned the contained normals are sampled otherwise vertex normals are used.
         */
        ResourceHandle<Texture> normal;

        /**
         *
         */
        float normalIntensity = 1;

        /**
         * If true the alpha value of the diffuse color / texture is used as the output alpha value.
         */
        bool transparent = false;

        /**
         * PBR albedo / Phong diffuse color
         */
        ColorRGBA diffuse{};
        ResourceHandle<Texture> diffuseTexture;

        /**
         * PBR Shading Data
         */
        float metallic{};
        float roughness = 0.5;
        float ambientOcclusion = 1;

        ResourceHandle<Texture> metallicTexture;
        ResourceHandle<Texture> roughnessTexture;
        ResourceHandle<Texture> ambientOcclusionTexture;

        /**
         * Phong Shading Data
         */
        ColorRGBA ambient{};
        ColorRGBA specular{};
        float shininess = 0.1;

        ResourceHandle<Texture> ambientTexture;
        ResourceHandle<Texture> specularTexture;
        ResourceHandle<Texture> shininessTexture;

        bool isLoaded() const override {
            return shader.isLoaded()
                   && normal.isLoaded()
                   && metallicTexture.isLoaded()
                   && roughnessTexture.isLoaded()
                   && ambientOcclusionTexture.isLoaded()
                   && diffuseTexture.isLoaded()
                   && ambientTexture.isLoaded()
                   && specularTexture.isLoaded()
                   && shininessTexture.isLoaded()
                   && ((!normal.assigned()) || normal.get().isLoaded())
                   && (!metallicTexture.assigned() || metallicTexture.get().isLoaded())
                   && (!roughnessTexture.assigned() || roughnessTexture.get().isLoaded())
                   && (!ambientOcclusionTexture.assigned() || ambientOcclusionTexture.get().isLoaded())
                   && (!diffuseTexture.assigned() || diffuseTexture.get().isLoaded())
                   && (!ambientTexture.assigned() || ambientTexture.get().isLoaded())
                   && (!specularTexture.assigned() || specularTexture.get().isLoaded())
                   && (!shininessTexture.assigned() || shininessTexture.get().isLoaded());
        }
    };
}

#endif //XENGINE_MATERIAL_HPP
