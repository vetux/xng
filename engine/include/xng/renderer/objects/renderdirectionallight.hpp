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

#ifndef XENGINE_RENDERDIRECTIONALLIGHT_HPP
#define XENGINE_RENDERDIRECTIONALLIGHT_HPP

#include "xng/assets/color.hpp"
#include "xng/shaderscript/macro/shaderstruct.hpp"
#include "xng/renderer/renderobject.hpp"
#include "xng/renderer/stream/bufferstreamer.hpp"

namespace xng {
    ShaderStruct(ShaderDirectionalLight,
                 Vec4f, color,
                 Vec4f, direction,
                 Vec4f, shadowFarPlane,
                 Mat4f, shadowProjectionMatrix)

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

        explicit RenderDirectionalLight(const Id id, BufferStreamer<ShaderDirectionalLight::CPU> &lightStream)
            : RenderObject(OBJECT_DIRECTIONAL_LIGHT, id),
              lightStream(lightStream) {
            lightHandle = lightStream.create();
        }

        ~RenderDirectionalLight() override {
            lightStream.destroy(lightHandle);
        }

        void set(const Vec3f &direction,
                 const ColorRGB color,
                 const float power,
                 const bool castShadows,
                 const float shadowNearPlane,
                 const float shadowFarPlane,
                 const float shadowExtent) const {
            const auto colorF = color.divide() * power;

            ShaderDirectionalLight::CPU light;
            light.color = Vec4f(colorF.x, colorF.y, colorF.z, 0);
            light.direction = Vec4f(direction.x, direction.y, direction.z, 0);

            light.shadowFarPlane = Vec4f(shadowFarPlane, 0, 0, 0);

            if (castShadows) {
                light.shadowProjectionMatrix = getShadowProjection(Transform({}, direction, {}),
                                                                   shadowNearPlane,
                                                                   shadowFarPlane,
                                                                   shadowExtent);
            }

            lightStream.upload(lightHandle, light);
        }

        [[nodiscard]] BufferStreamer<ShaderDirectionalLight::CPU>::Slot getSlot() const {
            return lightHandle;
        }

        bool isUploadComplete() override {
            return lightStream.isUploadComplete(lightHandle);
        }

        void flush() override {
            lightStream.flush(lightHandle);
        }

    private:
        BufferStreamer<ShaderDirectionalLight::CPU> &lightStream;
        BufferStreamer<ShaderDirectionalLight::CPU>::Slot lightHandle;
    };
}
#endif //XENGINE_RENDERDIRECTIONALLIGHT_HPP
