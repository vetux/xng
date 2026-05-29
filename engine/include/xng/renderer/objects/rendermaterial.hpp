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

namespace xng {
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
                                const Vec4f &normalIntensity)
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
            material.normalIntensity = normalIntensity;

            if (albedo) {
                const auto &handle = albedo->getHandle();
                auto scale = handle.getScale();

                material.albedo.level_index = Vec2i(handle.level, static_cast<int>(handle.slot));
                material.albedo.minFilter_magFilter_mipFilter_wrap = Vec4i(albedoProperties.minFilter,
                                                                           albedoProperties.magFilter,
                                                                           albedoProperties.mipFilter,
                                                                           albedoProperties.wrapping);
                material.albedo.scale_texSize = Vec4f(scale.x,
                                                      scale.y,
                                                      static_cast<float>(handle.size.x),
                                                      static_cast<float>(handle.size.y));
            } else {
                material.albedo.level_index = Vec2i(-1);
            }

            if (metallic) {
                const auto &handle = metallic->getHandle();
                auto scale = handle.getScale();

                material.metallic.level_index = Vec2i(handle.level, static_cast<int>(handle.slot));
                material.metallic.minFilter_magFilter_mipFilter_wrap = Vec4i(metallicProperties.minFilter,
                                                                             metallicProperties.magFilter,
                                                                             metallicProperties.mipFilter,
                                                                             metallicProperties.wrapping);
                material.metallic.scale_texSize = Vec4f(scale.x,
                                                        scale.y,
                                                        static_cast<float>(handle.size.x),
                                                        static_cast<float>(handle.size.y));
            } else {
                material.metallic.level_index = Vec2i(-1);
            }

            if (roughness) {
                const auto &handle = roughness->getHandle();
                auto scale = handle.getScale();

                material.roughness.level_index = Vec2i(handle.level, static_cast<int>(handle.slot));
                material.roughness.minFilter_magFilter_mipFilter_wrap = Vec4i(roughnessProperties.minFilter,
                                                                              roughnessProperties.magFilter,
                                                                              roughnessProperties.mipFilter,
                                                                              roughnessProperties.wrapping);
                material.roughness.scale_texSize = Vec4f(scale.x,
                                                         scale.y,
                                                         static_cast<float>(handle.size.x),
                                                         static_cast<float>(handle.size.y));
            } else {
                material.roughness.level_index = Vec2i(-1);
            }

            if (ambientOcclusion) {
                const auto &handle = ambientOcclusion->getHandle();
                auto scale = handle.getScale();

                material.ambientOcclusion.level_index = Vec2i(handle.level, static_cast<int>(handle.slot));
                material.ambientOcclusion.minFilter_magFilter_mipFilter_wrap = Vec4i(
                    ambientOcclusionProperties.minFilter,
                    ambientOcclusionProperties.magFilter,
                    ambientOcclusionProperties.mipFilter,
                    ambientOcclusionProperties.wrapping);

                material.ambientOcclusion.scale_texSize = Vec4f(scale.x,
                                                                scale.y,
                                                                static_cast<float>(handle.size.x),
                                                                static_cast<float>(handle.size.y));
            } else {
                material.ambientOcclusion.level_index = Vec2i(-1);
            }

            if (normal) {
                const auto &handle = normal->getHandle();
                auto scale = handle.getScale();

                material.normal.level_index = Vec2i(handle.level, static_cast<int>(handle.slot));
                material.normal.minFilter_magFilter_mipFilter_wrap = Vec4i(normalProperties.minFilter,
                                                                           normalProperties.magFilter,
                                                                           normalProperties.mipFilter,
                                                                           normalProperties.wrapping);
                material.normal.scale_texSize = Vec4f(scale.x,
                                                      scale.y,
                                                      static_cast<float>(handle.size.x),
                                                      static_cast<float>(handle.size.y));
            } else {
                material.normal.level_index = Vec2i(-1);
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
