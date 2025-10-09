/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
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

#include "xng/graphics/scene/texture.hpp"

#include "xng/resource/resourcehandle.hpp"

#include "xng/io/messageable.hpp"

namespace xng {
    /**
     * A PBR material for the built-in lighting passes.
     */
    struct XENGINE_EXPORT Material final : Resource, Messageable {
        RESOURCE_TYPENAME(Material)

        ColorRGBA albedo = ColorRGBA::white();
        float metallic = 0;
        float roughness = 0.5;
        float ambientOcclusion = 1;

        ResourceHandle<Texture> albedoTexture;
        ResourceHandle<Texture> metallicTexture;
        ResourceHandle<Texture> roughnessTexture;
        ResourceHandle<Texture> ambientOcclusionTexture;

        ResourceHandle<Texture> normal; // If assigned, the contained normals replace the vertex normals
        float normalIntensity = 1; // The specified value is used to scale texture normals

        bool transparent = false; // Whether the albedo alpha should be presented. (Forward Shading)

        ~Material() override = default;

        std::unique_ptr<Resource> clone() override {
            return std::make_unique<Material>(*this);
        }

        Messageable &operator<<(const Message &message) override {
            message.value("normal", normal);

            message.value("normalIntensity", normalIntensity);

            message.value("transparent", transparent);

            message.value("albedo", albedo);;

            message.value("albedoTexture", albedoTexture);

            message.value("metallic", metallic);
            message.value("roughness", roughness);
            message.value("ambientOcclusion", ambientOcclusion);

            message.value("metallicTexture", metallicTexture);
            message.value("roughnessTexture", roughnessTexture);
            message.value("ambientOcclusionTexture", ambientOcclusionTexture);

            return *this;
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);

            normal >> message["normal"];

            normalIntensity >> message["normalIntensity"];

            transparent >> message["transparent"];

            albedo >> message["albedo"];

            albedoTexture >> message["albedoTexture"];

            metallic >> message["metallic"];
            roughness >> message["roughness"];
            ambientOcclusion >> message["ambientOcclusion"];

            metallicTexture >> message["metallicTexture"];
            roughnessTexture >> message["roughnessTexture"];
            ambientOcclusionTexture >> message["ambientOcclusionTexture"];

            return message;
        }

        bool isLoaded() const override {
            return normal.isLoaded()
                   && metallicTexture.isLoaded()
                   && roughnessTexture.isLoaded()
                   && ambientOcclusionTexture.isLoaded()
                   && albedoTexture.isLoaded()
                   && ((!normal.assigned()) || normal.get().isLoaded())
                   && (!metallicTexture.assigned() || metallicTexture.get().isLoaded())
                   && (!roughnessTexture.assigned() || roughnessTexture.get().isLoaded())
                   && (!ambientOcclusionTexture.assigned() || ambientOcclusionTexture.get().isLoaded())
                   && (!albedoTexture.assigned() || albedoTexture.get().isLoaded());
        }
    };
}

#endif //XENGINE_MATERIAL_HPP
