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

#include "xng/render/scene/texture.hpp"
#include "xng/shader/shader.hpp"

#include "xng/resource/resourcehandle.hpp"

#include "xng/io/messageable.hpp"

namespace xng {
    /**
     * A PBR material for the provided lighting passes.
     *
     * Users can create custom lighting passes if needed and write to the SLOT_DEFERRED/FORWARD_COLOR and SLOT_DEFERRED/FORWARD_DEPTH.
     */
    struct XENGINE_EXPORT Material : public Resource, public Messageable {
        ~Material() override = default;

        std::unique_ptr<Resource> clone() override {
            return std::make_unique<Material>(*this);
        }

        std::type_index getTypeIndex() const override {
            return typeid(Material);
        }

        Messageable &operator<<(const Message &message) override {
            message.value("normal", normal);

            message.value("normalIntensity", normalIntensity);

            message.value("transparent", transparent, false);

            message.value("albedo", albedo);;

            message.value("albedoTexture", albedoTexture);

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

        /**
         * If true the alpha value of the diffuse color / texture is used as the output alpha value.
         * This is implemented using forward rendering instead of the default deferred rendering path used for non transparent objects.
         */
        bool transparent = false;

        /**
         * If assigned the contained normals are sampled otherwise vertex normals are used.
         */
        ResourceHandle<Texture> normal;

        /**
         * The sampled normals are scaled by the specified value
         */
        float normalIntensity = 1;

        ColorRGBA albedo{};
        ResourceHandle<Texture> albedoTexture;

        float metallic{};
        ResourceHandle<Texture> metallicTexture;

        float roughness = 0.5;
        ResourceHandle<Texture> roughnessTexture;

        float ambientOcclusion = 1;
        ResourceHandle<Texture> ambientOcclusionTexture;

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
