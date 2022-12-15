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

#include "texture.hpp"
#include "shader.hpp"

#include "xng/resource/resourcehandle.hpp"

#include "xng/io/messageable.hpp"

namespace xng {
    struct XENGINE_EXPORT Material : public Resource, public Messageable {
        enum LightingModel : int {
            PHONG = 0,
            PBR = 1
        };

        ~Material() override = default;

        std::unique_ptr<Resource> clone() override {
            return std::make_unique<Material>(*this);
        }

        std::type_index getTypeIndex() override {
            return typeid(Material);
        }

        Messageable &operator<<(const Message &message) override {
            model = (LightingModel) message.getMessage("model", Message((int) PHONG)).asInt();

            shader << message.getMessage("shader");

            ambient << message.getMessage("ambient");
            specular << message.getMessage("specular");

            ambientTexture << message.getMessage("ambientTexture");
            specularTexture << message.getMessage("specularTexture");

            albedo << message.getMessage("albedo");
            message.value("metallic", metallic);
            message.value("roughness", roughness, 1.0f);
            message.value("ambientOcclusion", ambientOcclusion, 1.0f);

            albedoTexture << message.getMessage("albedoTexture");
            metallicTexture << message.getMessage("metallicTexture");
            roughnessTexture << message.getMessage("roughnessTexture");
            ambientOcclusionTexture << message.getMessage("ambientOcclusionTexture");

            normal << message.getMessage("normal");

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
