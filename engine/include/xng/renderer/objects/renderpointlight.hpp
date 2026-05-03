/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2026 Julia Zampiccoli
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

#ifndef XENGINE_RENDERPOINTLIGHT_HPP
#define XENGINE_RENDERPOINTLIGHT_HPP

#include "xng/assets/color.hpp"
#include "xng/renderer/renderobject.hpp"

namespace xng {
    class RenderPointLight final : public RenderObject {
    public:
        RenderPointLight(const Id id,
                         BufferStreamer<ShaderPointLight::CPU> &lightStream,
                         StreamTexture &shadowMapTexture)
            : RenderObject(id, OBJECT_POINT_LIGHT),
              lightStream(lightStream),
              shadowMapTexture(shadowMapTexture) {
            lightHandle = lightStream.create();
        }

        ~RenderPointLight() override {
            lightStream.destroy(lightHandle);
            deallocateShadowMap();
        }

        void set(const Vec3f &position,
                 const ColorRGB color,
                 const float power,
                 const bool castShadows,
                 const float shadowNearPlane,
                 const float shadowFarPlane) {
            const auto colorF = color.divide() * power;

            ShaderPointLight::CPU light;
            light.position = Vec4f(position.x, position.y, position.z, 0);
            light.color = Vec4f(colorF.x, colorF.y, colorF.z, 0);

            light.shadowFarPlane = Vec4f(shadowFarPlane, 0, 0, 0);

            if (castShadows) {
                if (!shadowMapAllocated) {
                    shadowMap = shadowMapTexture.create();
                    shadowMapAllocated = true;
                }
                light.shadowLayer = Vec4i(static_cast<int>(shadowMap), 0, 0, 0);

                const auto shadowProj = MatrixMath::perspective(90.0f, 1, shadowNearPlane, shadowFarPlane);

                light.shadowMatrices.value[0] = (shadowProj *
                                                 MatrixMath::lookAt(position,
                                                                    position + Vec3f(1.0, 0.0, 0.0),
                                                                    Vec3f(0.0, -1.0, 0.0)));
                light.shadowMatrices.value[1] = (shadowProj *
                                                 MatrixMath::lookAt(position,
                                                                    position + Vec3f(-1.0, 0.0, 0.0),
                                                                    Vec3f(0.0, -1.0, 0.0)));
                light.shadowMatrices.value[2] = (shadowProj *
                                                 MatrixMath::lookAt(position,
                                                                    position + Vec3f(0.0, 1.0, 0.0),
                                                                    Vec3f(0.0, 0.0, -1.0)));
                light.shadowMatrices.value[3] = (shadowProj *
                                                 MatrixMath::lookAt(position,
                                                                    position + Vec3f(0.0, -1.0, 0.0),
                                                                    Vec3f(0.0, 0.0, 1.0)));
                light.shadowMatrices.value[4] = (shadowProj *
                                                 MatrixMath::lookAt(position,
                                                                    position + Vec3f(0.0, 0.0, 1.0),
                                                                    Vec3f(0.0, -1.0, 0.0)));
                light.shadowMatrices.value[5] = (shadowProj *
                                                 MatrixMath::lookAt(position,
                                                                    position + Vec3f(0.0, 0.0, -1.0),
                                                                    Vec3f(0.0, -1.0, 0.0)));
            } else {
                deallocateShadowMap();
                light.shadowLayer = Vec4i(-1, 0, 0, 0);
            }

            lightStream.upload(lightHandle, light);
        }

        [[nodiscard]] BufferStreamer<ShaderPointLight::CPU>::Handle getLightHandle() const {
            return lightHandle;
        }

        bool isUploadComplete() override {
            return lightStream.isUploadComplete(lightHandle);
        }

        void flush() override {
            lightStream.flush(lightHandle);
        }

    private:
        void deallocateShadowMap() {
            if (shadowMapAllocated) {
                shadowMapTexture.destroy(shadowMap);
            }
            shadowMapAllocated = false;
        }

        BufferStreamer<ShaderPointLight::CPU> lightStream;
        BufferStreamer<ShaderPointLight::CPU>::Handle lightHandle;

        StreamTexture &shadowMapTexture;
        StreamTexture::Handle shadowMap{};

        bool shadowMapAllocated = false;
    };
}
#endif //XENGINE_RENDERPOINTLIGHT_HPP
