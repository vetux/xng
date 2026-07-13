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

#ifndef XENGINE_RENDERMATERIAL_HPP
#define XENGINE_RENDERMATERIAL_HPP

#include "xng/renderer/objects/rendertexture.hpp"
#include "xng/renderer/samplingproperties.hpp"
#include "xng/shaderscript/macro/shaderstruct.hpp"

namespace xng {
    ShaderStruct(ShaderMaterial,
                 Vec4f, albedoColor,
                 Vec4f, metallic_roughness_ambientOcclusion,
                 Vec4f, normalIntensity_flipNormal,
                 ShaderTexture, normal,
                 ShaderTexture, metallic,
                 ShaderTexture, roughness,
                 ShaderTexture, ambientOcclusion,
                 ShaderTexture, albedo)

    class RenderMaterial final : public RenderObject {
    public:
        /**
         * @param id
         * @param materialStream
         * @param albedoColor
         * @param metallicColor
         * @param roughnessColor
         * @param ambientOcclusionColor
         * @param _albedo The albedo texture. Must use sRGB color space!
         * @param albedoProperties
         * @param _metallic
         * @param metallicProperties
         * @param _roughness
         * @param roughnessProperties
         * @param _ambientOcclusion
         * @param ambientOcclusionProperties
         * @param _normal
         * @param normalProperties
         * @param normalIntensity
         * @param flipNormal Whether to flip the y value of the sampled normal. This is required because some modeling software only allows baking normals in right-handed coordinate space and XNG uses left-handed coordinate space.
         */
        explicit RenderMaterial(const Id id,
                                BufferStreamer<ShaderMaterial::CPU> &materialStream,
                                ColorRGBA albedoColor,
                                float metallicColor,
                                float roughnessColor,
                                float ambientOcclusionColor,
                                RenderObjectHandle<RenderTexture> _albedo,
                                const SamplingProperties &albedoProperties,
                                RenderObjectHandle<RenderTexture> _metallic,
                                const SamplingProperties &metallicProperties,
                                RenderObjectHandle<RenderTexture> _roughness,
                                const SamplingProperties &roughnessProperties,
                                RenderObjectHandle<RenderTexture> _ambientOcclusion,
                                const SamplingProperties &ambientOcclusionProperties,
                                RenderObjectHandle<RenderTexture> _normal,
                                const SamplingProperties &normalProperties,
                                const float normalIntensity,
                                const bool flipNormal)
            : RenderObject(OBJECT_MATERIAL, id),
              materialStream(materialStream),
              albedo(std::move(_albedo)),
              metallic(std::move(_metallic)),
              roughness(std::move(_roughness)),
              ambientOcclusion(std::move(_ambientOcclusion)),
              normal(std::move(_normal)) {
            materialHandle = materialStream.create();
            ShaderMaterial::CPU material;

            material.albedoColor = albedoColor.divide();
            material.metallic_roughness_ambientOcclusion = Vec4f(metallicColor,
                                                                 roughnessColor,
                                                                 ambientOcclusionColor,
                                                                 0);
            material.normalIntensity_flipNormal = Vec4f(normalIntensity, flipNormal ? 1.0f : 0.0f, 0, 0);

            if (albedo) {
                material.albedo.textureSize_textureID_maxMip = Vec4i(albedo->getSize().x,
                                                                     albedo->getSize().y,
                                                                     static_cast<int>(albedo->getHandle()),
                                                                     static_cast<int>(albedo->getMaxMip()));
                material.albedo.minFilter_magFilter_mipFilter_wrap = Vec4i(albedoProperties.minFilter,
                                                                           albedoProperties.magFilter,
                                                                           albedoProperties.mipFilter,
                                                                           albedoProperties.wrapping);
            } else {
                material.albedo.textureSize_textureID_maxMip = Vec4i(-1);
            }

            if (metallic) {
                material.metallic.textureSize_textureID_maxMip = Vec4i(metallic->getSize().x,
                                                                       metallic->getSize().y,
                                                                       static_cast<int>(metallic->getHandle()),
                                                                       static_cast<int>(metallic->getMaxMip()));
                material.metallic.minFilter_magFilter_mipFilter_wrap = Vec4i(metallicProperties.minFilter,
                                                                             metallicProperties.magFilter,
                                                                             metallicProperties.mipFilter,
                                                                             metallicProperties.wrapping);
            } else {
                material.metallic.textureSize_textureID_maxMip = Vec4i(-1);
            }

            if (roughness) {
                material.roughness.textureSize_textureID_maxMip = Vec4i(roughness->getSize().x,
                                                                        roughness->getSize().y,
                                                                        static_cast<int>(roughness->getHandle()),
                                                                        static_cast<int>(roughness->getMaxMip()));
                material.roughness.minFilter_magFilter_mipFilter_wrap = Vec4i(roughnessProperties.minFilter,
                                                                              roughnessProperties.magFilter,
                                                                              roughnessProperties.mipFilter,
                                                                              roughnessProperties.wrapping);
            } else {
                material.roughness.textureSize_textureID_maxMip = Vec4i(-1);
            }

            if (ambientOcclusion) {
                material.ambientOcclusion.textureSize_textureID_maxMip = Vec4i(ambientOcclusion->getSize().x,
                                                                               ambientOcclusion->getSize().y,
                                                                               static_cast<int>(ambientOcclusion->
                                                                                   getHandle()),
                                                                               static_cast<int>(ambientOcclusion->
                                                                                   getMaxMip()));
                material.ambientOcclusion.minFilter_magFilter_mipFilter_wrap = Vec4i(
                    ambientOcclusionProperties.minFilter,
                    ambientOcclusionProperties.magFilter,
                    ambientOcclusionProperties.mipFilter,
                    ambientOcclusionProperties.wrapping);
            } else {
                material.ambientOcclusion.textureSize_textureID_maxMip = Vec4i(-1);
            }

            if (normal) {
                material.normal.textureSize_textureID_maxMip = Vec4i(normal->getSize().x,
                                                                     normal->getSize().y,
                                                                     static_cast<int>(normal->getHandle()),
                                                                     static_cast<int>(normal->getMaxMip()));
                material.normal.minFilter_magFilter_mipFilter_wrap = Vec4i(normalProperties.minFilter,
                                                                           normalProperties.magFilter,
                                                                           normalProperties.mipFilter,
                                                                           normalProperties.wrapping);
            } else {
                material.normal.textureSize_textureID_maxMip = Vec4i(-1);
            }

            materialStream.upload(materialHandle, material);
        }

        ~RenderMaterial() override {
            materialStream.destroy(materialHandle);
        }

        [[nodiscard]] BufferStreamer<ShaderMaterial::CPU>::Slot getSlot() const {
            return materialHandle;
        }

        bool isUploadComplete() override {
            if (albedo && !albedo->isUploadComplete()) {
                return false;
            }
            if (metallic && !metallic->isUploadComplete()) {
                return false;
            }
            if (roughness && !roughness->isUploadComplete()) {
                return false;
            }
            if (ambientOcclusion && !ambientOcclusion->isUploadComplete()) {
                return false;
            }
            if (normal && !normal->isUploadComplete()) {
                return false;
            }
            return materialStream.isUploadComplete(materialHandle);
        }

        void flush() override {
            if (albedo) {
                albedo->flush();
            }
            if (metallic) {
                metallic->flush();
            }
            if (roughness) {
                roughness->flush();
            }
            if (ambientOcclusion) {
                ambientOcclusion->flush();
            }
            if (normal) {
                normal->flush();
            }
            materialStream.flush(materialHandle);
        }

        [[nodiscard]] const RenderObjectHandle<RenderTexture> &getAlbedo() const {
            return albedo;
        }

        [[nodiscard]] const RenderObjectHandle<RenderTexture> &getMetallic() const {
            return metallic;
        }

        [[nodiscard]] const RenderObjectHandle<RenderTexture> &getRoughness() const {
            return roughness;
        }

        [[nodiscard]] const RenderObjectHandle<RenderTexture> &getAmbientOcclusion() const {
            return ambientOcclusion;
        }

        [[nodiscard]] const RenderObjectHandle<RenderTexture> &getNormal() const {
            return normal;
        }

    private:
        BufferStreamer<ShaderMaterial::CPU> &materialStream;
        BufferStreamer<ShaderMaterial::CPU>::Slot materialHandle;

        RenderObjectHandle<RenderTexture> albedo;
        RenderObjectHandle<RenderTexture> metallic;
        RenderObjectHandle<RenderTexture> roughness;
        RenderObjectHandle<RenderTexture> ambientOcclusion;
        RenderObjectHandle<RenderTexture> normal;
    };
}

#endif //XENGINE_RENDERMATERIAL_HPP
