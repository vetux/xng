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

#ifndef XENGINE_MATERIAL_HPP
#define XENGINE_MATERIAL_HPP

#include "xng/assets/image.hpp"

#include "xng/resource/resourcehandle.hpp"

#include "xng/io/messageable.hpp"

namespace xng {
    /**
     * A PBR material.
     */
    struct Material final : ResourceBase, Messageable {
        RESOURCE_TYPENAME(Material)

        ColorRGBA albedo = ColorRGBA::white();
        float metallic = 0;
        float roughness = 0.5;
        float ambientOcclusion = 1;

        ResourceHandle<ImageRGBA> albedoTexture;
        ResourceHandle<ImageRGBA> metallicTexture;
        ResourceHandle<ImageRGBA> roughnessTexture;
        ResourceHandle<ImageRGBA> ambientOcclusionTexture;

        ResourceHandle<ImageRGBA> normal; // If assigned, the contained normals replace the vertex normals

        ~Material() override = default;

        std::unique_ptr<ResourceBase> clone() override {
            return std::make_unique<Material>(*this);
        }

        Messageable &operator<<(const Message &message) override {
            message.value("albedo", albedo);;
            message.value("metallic", metallic);
            message.value("roughness", roughness);
            message.value("ambientOcclusion", ambientOcclusion);

            message.value("albedoTexture", albedoTexture);
            message.value("metallicTexture", metallicTexture);
            message.value("roughnessTexture", roughnessTexture);
            message.value("ambientOcclusionTexture", ambientOcclusionTexture);

            message.value("normal", normal);

            return *this;
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);

            albedo >> message["albedo"];
            metallic >> message["metallic"];
            roughness >> message["roughness"];
            ambientOcclusion >> message["ambientOcclusion"];

            albedoTexture >> message["albedoTexture"];
            metallicTexture >> message["metallicTexture"];
            roughnessTexture >> message["roughnessTexture"];
            ambientOcclusionTexture >> message["ambientOcclusionTexture"];

            normal >> message["normal"];

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
