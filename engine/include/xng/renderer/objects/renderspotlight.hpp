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

        explicit RenderSpotLight(const Id id, BufferStreamer<ShaderSpotLight::CPU> &lightStream)
            : RenderObject(OBJECT_SPOT_LIGHT, id),
              lightStream(lightStream) {
            lightHandle = lightStream.create();
        }

        ~RenderSpotLight() override {
            lightStream.destroy(lightHandle);
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
                light.shadowProjectionMatrix = getShadowProjection(Transform(position, direction, {}),
                                                                   shadowNearPlane,
                                                                   shadowFarPlane);
            }

            lightStream.upload(lightHandle, light);
        }

        [[nodiscard]] BufferStreamer<ShaderPointLight::CPU>::Slot getSlot() const {
            return lightHandle;
        }

        bool isUploadComplete() override {
            return lightStream.isUploadComplete(lightHandle);
        }

        void flush() override {
            lightStream.flush(lightHandle);
        }

    private:
        BufferStreamer<ShaderSpotLight::CPU> &lightStream;
        BufferStreamer<ShaderSpotLight::CPU>::Slot lightHandle;
    };
}

#endif //XENGINE_RENDERSPOTLIGHT_HPP
