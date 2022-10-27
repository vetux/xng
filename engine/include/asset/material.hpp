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
#include "asset/shader.hpp"

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

            shader << message.value("shader");

            ambient << message.value("ambient");
            specular << message.value("specular");

            ambientTexture << message.value("ambientTexture");
            specularTexture << message.value("specularTexture");

            albedo << message.value("albedo");
            metallic = message.value("metallic", 0.0f);
            roughness = message.value("roughness", 1.0f);
            ambientOcclusion = message.value("ambientOcclusion", 1.0f);

            albedoTexture << message.value("albedoTexture");
            metallicTexture << message.value("metallicTexture");
            roughnessTexture << message.value("roughnessTexture");
            ambientOcclusionTexture << message.value("ambientOcclusionTexture");

            normal << message.value("normal");

            return *this;
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);

            message["model"] = model;

            shader >> message["shader"];

            ambient >> message["ambient"];
            specular >> message["specular"];

            if (ambientTexture.assigned())
                ambientTexture >> message["ambientTexture"];
            if (specularTexture.assigned())
                specularTexture >> message["specularTexture"];

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

            return message;
        }

        LightingModel model{};

        /**
         * Optional user specified shader.
         *
         * Currently only glsl user shaders are supported.
         *
         * In a deferred pipeline the vertex/geometry shaders would replace the shaders in the gbuffer construction pass
         * and the fragment shader would replace the lighting pass.
         *
         * The vertex/geometry shaders might be run with platform specific fragment shaders and
         * the fragment shader might be run with a platform specific vertex/geometry shader (Deferred Shading).
         *
         * To make shader code compatible across pipelines the shaders must access external resources
         * through the stable shader interface described in framegraphshader.hpp
         * by including either "fragment.glsl" or "vertex.glsl" respectively.
         */
        ResourceHandle<Shader> shader;

        // PBR
        ColorRGBA albedo{}; // -> diffuse in phong
        float metallic{};
        float roughness{}; // -> invert for shininess in phong
        float ambientOcclusion{};

        ResourceHandle<ImageRGBA> albedoTexture;
        ResourceHandle<ImageRGBA> metallicTexture;
        ResourceHandle<ImageRGBA> roughnessTexture;
        ResourceHandle<ImageRGBA> ambientOcclusionTexture;

        // Phong Shading
        ColorRGBA ambient{};
        ColorRGBA specular{};

        ResourceHandle<ImageRGBA> ambientTexture;
        ResourceHandle<ImageRGBA> specularTexture;

        ResourceHandle<ImageRGBA> normal; // If assigned the sampled normals are used otherwise vertex normals.
    };
}

#endif //XENGINE_MATERIAL_HPP
