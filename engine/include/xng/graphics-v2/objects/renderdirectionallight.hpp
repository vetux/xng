/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 3 of the License, or (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.

 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef XENGINE_RENDERDIRECTIONALLIGHT_HPP
#define XENGINE_RENDERDIRECTIONALLIGHT_HPP

#include "xng/graphics-v2/renderobject.hpp"
#include "xng/graphics-v2/stream/streamtexture.hpp"
#include "xng/graphics-v2/stream/bufferstreamer.hpp"

namespace xng {
    class RenderDirectionalLight final : public RenderObject {
    public:
        static Mat4f getShadowProjection(const Transform &transform,
                                         const float near,
                                         const float far,
                                         const float extent) {
            return MatrixMath::ortho(-extent,
                                     extent,
                                     -extent,
                                     extent,
                                     near,
                                     far)
                   * MatrixMath::inverse(transform.getRotation().matrix());
        }

        RenderDirectionalLight(const Id id,
                               BufferStreamer<ShaderDirectionalLight::CPU> &lightStream,
                               StreamTexture &shadowMapTexture)
            : RenderObject(id, OBJECT_DIRECTIONAL_LIGHT),
              lightStream(lightStream),
              shadowMapTexture(shadowMapTexture) {
            lightHandle = lightStream.create();
        }

        ~RenderDirectionalLight() override {
            lightStream.destroy(lightHandle);
            deallocateShadowMap();
        }

        void set(const Vec3f &direction,
                 const ColorRGB color,
                 const float power,
                 const bool castShadows,
                 const float shadowNearPlane,
                 const float shadowFarPlane,
                 const float shadowExtent) {
            const auto colorF = color.divide() * power;

            ShaderDirectionalLight::CPU light;
            light.color = Vec4f(colorF.x, colorF.y, colorF.z, 0);
            light.direction = Vec4f(direction.x, direction.y, direction.z, 0);

            light.shadowFarPlane = Vec4f(shadowFarPlane, 0, 0, 0);

            if (castShadows) {
                if (!shadowMapAllocated) {
                    shadowMap = shadowMapTexture.create();
                    shadowMapAllocated = true;
                }
                light.shadowLayer = Vec4i(static_cast<int>(shadowMap), 0, 0, 0);
                light.shadowProjectionMatrix = getShadowProjection(Transform({}, direction, {}),
                                                                   shadowNearPlane,
                                                                   shadowFarPlane,
                                                                   shadowExtent);
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

        BufferStreamer<ShaderDirectionalLight::CPU> lightStream;
        BufferStreamer<ShaderDirectionalLight::CPU>::Handle lightHandle;

        StreamTexture &shadowMapTexture;
        StreamTexture::Handle shadowMap{};

        bool shadowMapAllocated = false;
    };
}
#endif //XENGINE_RENDERDIRECTIONALLIGHT_HPP
