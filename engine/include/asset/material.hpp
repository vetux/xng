/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2022  Julian Zampiccoli
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
        enum LightingModel : int {
            PHONG = 0,
            PBR = 1
        };

        static LightingModel deserializeModel(int model) {
            switch (model) {
                case PHONG:
                    return PHONG;
                case PBR:
                    return PBR;
                default:
                    throw std::runtime_error("Invalid model id");
            }
        }

        ~Material() override = default;

        std::unique_ptr<Resource> clone() override {
            return std::make_unique<Material>(*this);
        }

        std::type_index getTypeIndex() override {
            return typeid(Material);
        }

        Messageable &operator<<(const Message &message) override {
            model = deserializeModel(message.value("model", static_cast<int>(PHONG)));

            diffuse << message.value("diffuse");
            ambient << message.value("ambient");
            specular << message.value("specular");
            shininess = message.value("shininess", 32.0f);

            diffuseTexture << message.value("diffuseTexture");
            ambientTexture << message.value("ambientTexture");
            specularTexture << message.value("specularTexture");
            shininessTexture << message.value("shininessTexture");

            albedo << message.value("albedo");
            metallic = message.value("metallic", 0.0f);
            roughness = message.value("roughness", 1.0f);
            ambientOcclusion = message.value("ambientOcclusion", 1.0f);

            albedoTexture << message.value("albedoTexture");
            metallicTexture << message.value("metallicTexture");
            roughnessTexture << message.value("roughnessTexture");
            ambientOcclusionTexture << message.value("ambientOcclusionTexture");

            normal << message.value("normal");

            receiveShadows = message.value("receiveShadows", false);

            return *this;
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);

            message["model"] = model;

            diffuse >> message["diffuse"];
            ambient >> message["ambient"];
            specular >> message["specular"];
            message["shininess"] = shininess;

            if (diffuseTexture.assigned())
                diffuseTexture >> message["diffuseTexture"];
            if (ambientTexture.assigned())
                ambientTexture >> message["ambientTexture"];
            if (specularTexture.assigned())
                specularTexture >> message["specularTexture"];
            if (shininessTexture.assigned())
                shininessTexture >> message["shininessTexture"];

            albedo >> message["albedo"];
            message["metallic"] = metallic;
            message["roughness"] = roughness;
            message["ambientOcclusion"] = ambientOcclusion;

            if (albedoTexture.assigned())
                albedoTexture >> message["albedoTexture"];
            if (metallicTexture.assigned())
                metallicTexture >> message["metallicTexture"];
            if (roughnessTexture.assigned())
                roughnessTexture >> message["roughnessTexture"];
            if (ambientOcclusionTexture.assigned())
                ambientOcclusionTexture >> message["ambientOcclusionTexture"];

            if (normal.assigned())
                normal >> message["normal"];

            message["receiveShadows"] = receiveShadows;

            return message;
        }

        LightingModel model{};

        ColorRGBA diffuse{};
        ColorRGBA ambient{};
        ColorRGBA specular{};
        float shininess;

        ResourceHandle<Texture> diffuseTexture;
        ResourceHandle<Texture> ambientTexture;
        ResourceHandle<Texture> specularTexture;
        ResourceHandle<Texture> shininessTexture;

        ColorRGBA albedo{};
        float metallic{};
        float roughness{};
        float ambientOcclusion{};

        ResourceHandle<ImageRGBA> albedoTexture;
        ResourceHandle<ImageRGBA> metallicTexture;
        ResourceHandle<ImageRGBA> roughnessTexture;
        ResourceHandle<ImageRGBA> ambientOcclusionTexture;

        ResourceHandle<ImageRGBA> normal; // If assigned the sampled normals are used otherwise vertex normals.

        bool receiveShadows; // If true fragments belonging to this material are shadowed by other objects in the scene.
    };
}

#endif //XENGINE_MATERIAL_HPP
