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

#ifndef XENGINE_RENDERSPOTLIGHT_HPP
#define XENGINE_RENDERSPOTLIGHT_HPP

#include "xng/renderer/renderobject.hpp"
#include "xng/renderer/stream/streamtexture.hpp"

namespace xng {
    class RenderSpotLight final : public RenderObject {
    public:
        static Mat4f getShadowProjection(const Transform &transform, const float near, const float far) {
            return MatrixMath::perspective(45,
                                           1,
                                           near,
                                           far)
                   * MatrixMath::inverse(transform.getRotation().matrix())
                   * MatrixMath::translate(transform.getPosition() * -1);
        }

        RenderSpotLight(const Id id,
                        BufferStreamer<ShaderSpotLight::CPU> &lightStream,
                        StreamTexture &shadowMapTexture)
            : RenderObject(id, OBJECT_SPOT_LIGHT),
              lightStream(lightStream),
              shadowMapTexture(shadowMapTexture) {
            lightHandle = lightStream.create();
        }

        ~RenderSpotLight() override {
            lightStream.destroy(lightHandle);
            deallocateShadowMap();
        }

        void set(const Vec3f &position,
                 const Vec3f &direction,
                 const ColorRGB color,
                 const float power,
                 const float cutOff,
                 const float outerCutOff,
                 const float quadratic,
                 const float constant,
                 const float linear,
                 const bool castShadows,
                 const float shadowNearPlane,
                 const float shadowFarPlane) {
            const auto colorF = color.divide() * power;

            ShaderSpotLight::CPU light;
            light.position = Vec4f(position.x, position.y, position.z, 0);
            light.color = Vec4f(colorF.x, colorF.y, colorF.z, 0);
            light.direction_quadratic = Vec4f(direction.x, direction.y, direction.z, quadratic);
            light.cutOff_outerCutOff_constant_linear = Vec4f(cutOff, outerCutOff, constant, linear);

            light.shadowFarPlane = Vec4f(shadowFarPlane, 0, 0, 0);

            if (castShadows) {
                if (!shadowMapAllocated) {
                    shadowMap = shadowMapTexture.create();
                    shadowMapAllocated = true;
                }
                light.shadowLayer = Vec4i(static_cast<int>(shadowMap), 0, 0, 0);
                light.shadowProjectionMatrix = getShadowProjection(Transform(position, direction, {}),
                                                                   shadowNearPlane,
                                                                   shadowFarPlane);
            } else {
                deallocateShadowMap();
                light.shadowLayer = Vec4i(-1, 0, 0, 0);
            }
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

        BufferStreamer<ShaderSpotLight::CPU> lightStream;
        BufferStreamer<ShaderSpotLight::CPU>::Handle lightHandle;

        StreamTexture &shadowMapTexture;
        StreamTexture::Handle shadowMap{};

        bool shadowMapAllocated = false;
    };
}

#endif //XENGINE_RENDERSPOTLIGHT_HPP
