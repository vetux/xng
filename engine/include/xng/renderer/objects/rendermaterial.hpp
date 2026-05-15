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

namespace xng {
    class RenderMaterial final : public RenderObject {
    public:
        explicit RenderMaterial(BufferStreamer<ShaderMaterial::CPU> &materialStream,
                                ColorRGBA albedoColor,
                                float metallicColor,
                                float roughnessColor,
                                float ambientOcclusionColor,
                                RenderObjectHandle<RenderTexture> _albedo,
                                RenderObjectHandle<RenderTexture> _metallic,
                                RenderObjectHandle<RenderTexture> _roughness,
                                RenderObjectHandle<RenderTexture> _ambientOcclusion,
                                RenderObjectHandle<RenderTexture> _normal,
                                const Vec4f &normalIntensity)
            : RenderObject(OBJECT_MATERIAL),
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

                material.albedo.level_index_filtering_assigned = Vec4i(handle.level,
                                                                       static_cast<int>(handle.slot),
                                                                       0,
                                                                       1);
                material.albedo.scale_texSize = Vec4f(scale.x,
                                                      scale.y,
                                                      static_cast<float>(handle.size.x),
                                                      static_cast<float>(handle.size.y));
            } else {
                material.albedo.level_index_filtering_assigned = Vec4i(0, 0, 0, 0);
            }

            if (metallic) {
                const auto &handle = metallic->getHandle();
                auto scale = handle.getScale();

                material.metallic.level_index_filtering_assigned = Vec4i(handle.level,
                                                                         static_cast<int>(handle.slot),
                                                                         0,
                                                                         1);
                material.metallic.scale_texSize = Vec4f(scale.x,
                                                        scale.y,
                                                        static_cast<float>(handle.size.x),
                                                        static_cast<float>(handle.size.y));
            } else {
                material.metallic.level_index_filtering_assigned = Vec4i(0, 0, 0, 0);
            }

            if (roughness) {
                const auto &handle = roughness->getHandle();
                auto scale = handle.getScale();

                material.roughness.level_index_filtering_assigned = Vec4i(handle.level,
                                                                          static_cast<int>(handle.slot),
                                                                          0,
                                                                          1);
                material.roughness.scale_texSize = Vec4f(scale.x,
                                                         scale.y,
                                                         static_cast<float>(handle.size.x),
                                                         static_cast<float>(handle.size.y));
            } else {
                material.roughness.level_index_filtering_assigned = Vec4i(0, 0, 0, 0);
            }

            if (ambientOcclusion) {
                const auto &handle = ambientOcclusion->getHandle();
                auto scale = handle.getScale();

                material.ambientOcclusion.level_index_filtering_assigned = Vec4i(handle.level,
                    static_cast<int>(handle.slot),
                    0,
                    1);

                material.ambientOcclusion.scale_texSize = Vec4f(scale.x,
                                                                scale.y,
                                                                static_cast<float>(handle.size.x),
                                                                static_cast<float>(handle.size.y));
            } else {
                material.ambientOcclusion.level_index_filtering_assigned = Vec4i(0, 0, 0, 0);
            }

            if (normal) {
                const auto &handle = normal->getHandle();
                auto scale = handle.getScale();

                material.normal.level_index_filtering_assigned = Vec4i(handle.level,
                                                                       static_cast<int>(handle.slot),
                                                                       0,
                                                                       1);
                material.normal.scale_texSize = Vec4f(scale.x,
                                                      scale.y,
                                                      static_cast<float>(handle.size.x),
                                                      static_cast<float>(handle.size.y));
            } else {
                material.normal.level_index_filtering_assigned = Vec4i(0, 0, 0, 0);
            }
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
