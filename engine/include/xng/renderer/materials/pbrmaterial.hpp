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

#ifndef XENGINE_PBRMATERIAL_HPP
#define XENGINE_PBRMATERIAL_HPP

#include "xng/renderer/pipeline/renderpipeline.hpp"

namespace xng {
    class PBRMaterial {
    public:
        //TODO: Better solution for checking texture existence (Permutations) and default property values.
        enum Property : RenderPipelineMaterial::PropertyID {
            MATERIAL_ALBEDO_COLOR = 0,
            MATERIAL_ALBEDO_HAS_TEXTURE,

            MATERIAL_METALLIC_COLOR,
            MATERIAL_METALLIC_HAS_TEXTURE,

            MATERIAL_ROUGHNESS_COLOR,
            MATERIAL_ROUGHNESS_HAS_TEXTURE,

            MATERIAL_AMBIENT_OCCLUSION_COLOR,
            MATERIAL_AMBIENT_OCCLUSION_HAS_TEXTURE,

            MATERIAL_EMISSIVE_COLOR,
            MATERIAL_EMISSIVE_HAS_TEXTURE,

            MATERIAL_NORMAL_HAS_TEXTURE,

            MATERIAL_NORMAL_INTENSITY,
            MATERIAL_NORMAL_FLIP,

            RECEIVE_SHADOWS,
        };

        enum Texture : RenderPipelineMaterial::TextureID {
            MATERIAL_ALBEDO_TEXTURE,
            MATERIAL_METALLIC_TEXTURE,
            MATERIAL_ROUGHNESS_TEXTURE,
            MATERIAL_AMBIENT_OCCLUSION_TEXTURE,
            MATERIAL_EMISSIVE_TEXTURE,
            MATERIAL_NORMAL_TEXTURE,
        };

        static RenderPipeline::MaterialLayout getLayout() {
            RenderPipeline::MaterialLayout ret;

            ret.properties[MATERIAL_ALBEDO_COLOR] = rg::ShaderPrimitiveType::vec4();
            ret.properties[MATERIAL_ALBEDO_HAS_TEXTURE] = rg::ShaderPrimitiveType::Bool();

            ret.properties[MATERIAL_METALLIC_COLOR] = rg::ShaderPrimitiveType::Float();
            ret.properties[MATERIAL_METALLIC_HAS_TEXTURE] = rg::ShaderPrimitiveType::Bool();

            ret.properties[MATERIAL_ROUGHNESS_COLOR] = rg::ShaderPrimitiveType::Float();
            ret.properties[MATERIAL_ROUGHNESS_HAS_TEXTURE] = rg::ShaderPrimitiveType::Bool();

            ret.properties[MATERIAL_AMBIENT_OCCLUSION_COLOR] = rg::ShaderPrimitiveType::Float();
            ret.properties[MATERIAL_AMBIENT_OCCLUSION_HAS_TEXTURE] = rg::ShaderPrimitiveType::Bool();

            ret.properties[MATERIAL_EMISSIVE_COLOR] = rg::ShaderPrimitiveType::vec3();
            ret.properties[MATERIAL_EMISSIVE_HAS_TEXTURE] = rg::ShaderPrimitiveType::Bool();

            ret.properties[MATERIAL_NORMAL_HAS_TEXTURE] = rg::ShaderPrimitiveType::Bool();

            ret.properties[RECEIVE_SHADOWS] = rg::ShaderPrimitiveType::Bool();

            ret.textures.insert(MATERIAL_ALBEDO_TEXTURE);
            ret.textures.insert(MATERIAL_METALLIC_TEXTURE);
            ret.textures.insert(MATERIAL_ROUGHNESS_TEXTURE);
            ret.textures.insert(MATERIAL_AMBIENT_OCCLUSION_TEXTURE);
            ret.textures.insert(MATERIAL_EMISSIVE_TEXTURE);
            ret.textures.insert(MATERIAL_NORMAL_TEXTURE);

            return ret;
        }

        PBRMaterial() {
            properties[MATERIAL_ALBEDO_HAS_TEXTURE] = rg::ShaderPrimitive(false);
            properties[MATERIAL_METALLIC_HAS_TEXTURE] = rg::ShaderPrimitive(false);
            properties[MATERIAL_ROUGHNESS_HAS_TEXTURE] = rg::ShaderPrimitive(false);
            properties[MATERIAL_AMBIENT_OCCLUSION_HAS_TEXTURE] = rg::ShaderPrimitive(false);
            properties[MATERIAL_EMISSIVE_HAS_TEXTURE] = rg::ShaderPrimitive(false);
            properties[MATERIAL_NORMAL_HAS_TEXTURE] = rg::ShaderPrimitive(false);
            properties[RECEIVE_SHADOWS] = rg::ShaderPrimitive(false);
        }

        void setAlbedoColor(const ColorRGBA &color) {
            properties[MATERIAL_ALBEDO_COLOR] = rg::ShaderPrimitive(color.divide());
        }

        void setMetallic(const float metallic) {
            properties[MATERIAL_METALLIC_COLOR] = rg::ShaderPrimitive(metallic);
        }

        void setRoughness(const float roughness) {
            properties[MATERIAL_ROUGHNESS_COLOR] = rg::ShaderPrimitive(roughness);
        }

        void setAmbientOcclusion(const float ambientOcclusion) {
            properties[MATERIAL_AMBIENT_OCCLUSION_COLOR] = rg::ShaderPrimitive(ambientOcclusion);
        }

        void setEmissiveColor(const ColorRGBA &color) {
            properties[MATERIAL_EMISSIVE_COLOR] = rg::ShaderPrimitive(color.divide());
        }

        void setNormalIntensity(const float normalIntensity) {
            properties[MATERIAL_NORMAL_INTENSITY] = rg::ShaderPrimitive(normalIntensity);
        }

        void setNormalFlip(const bool flipNormal) {
            properties[MATERIAL_NORMAL_FLIP] = rg::ShaderPrimitive(flipNormal);
        }

        void setReceiveShadows(const bool receiveShadows) {
            properties[RECEIVE_SHADOWS] = rg::ShaderPrimitive(receiveShadows);
        }

        void setAlbedo(RenderObjectHandle<RenderTexture> texture, const SamplingProperties &samplingProperties) {
            textures[MATERIAL_ALBEDO_TEXTURE] = {std::move(texture), samplingProperties};
            properties[MATERIAL_ALBEDO_HAS_TEXTURE] = rg::ShaderPrimitive(true);
        }

        void setMetallic(RenderObjectHandle<RenderTexture> texture, const SamplingProperties &samplingProperties) {
            textures[MATERIAL_METALLIC_TEXTURE] = {std::move(texture), samplingProperties};
            properties[MATERIAL_METALLIC_HAS_TEXTURE] = rg::ShaderPrimitive(true);
        }

        void setRoughness(RenderObjectHandle<RenderTexture> texture, const SamplingProperties &samplingProperties) {
            textures[MATERIAL_ROUGHNESS_TEXTURE] = {std::move(texture), samplingProperties};
            properties[MATERIAL_ROUGHNESS_HAS_TEXTURE] = rg::ShaderPrimitive(true);
        }

        void setAmbientOcclusion(RenderObjectHandle<RenderTexture> texture,
                                 const SamplingProperties &samplingProperties) {
            textures[MATERIAL_AMBIENT_OCCLUSION_TEXTURE] = {std::move(texture), samplingProperties};
            properties[MATERIAL_AMBIENT_OCCLUSION_HAS_TEXTURE] = rg::ShaderPrimitive(true);
        }

        void setEmissive(RenderObjectHandle<RenderTexture> texture, const SamplingProperties &samplingProperties) {
            textures[MATERIAL_EMISSIVE_TEXTURE] = {std::move(texture), samplingProperties};
            properties[MATERIAL_EMISSIVE_HAS_TEXTURE] = rg::ShaderPrimitive(true);
        }

        void setNormal(RenderObjectHandle<RenderTexture> texture, const SamplingProperties &samplingProperties) {
            textures[MATERIAL_NORMAL_TEXTURE] = {std::move(texture), samplingProperties};
            properties[MATERIAL_NORMAL_HAS_TEXTURE] = rg::ShaderPrimitive(true);
        }

        const std::unordered_map<Property, rg::ShaderPrimitive> &getProperties() const {
            return properties;
        }

        const std::unordered_map<Texture, RenderPipelineMaterial::TextureSampler> &getTextures() const {
            return textures;
        }

    private:
        std::unordered_map<Property, rg::ShaderPrimitive> properties;
        std::unordered_map<Texture, RenderPipelineMaterial::TextureSampler> textures;
    };
}

#endif //XENGINE_PBRMATERIAL_HPP
