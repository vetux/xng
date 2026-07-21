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

        explicit RenderDirectionalLight(std::function<void()> onChangedCallback)
            : onChangedCallback(std::move(onChangedCallback)) {
        }

        ~RenderDirectionalLight() override = default;

        void set(const Vec3f &direction,
                 const ColorRGB color,
                 const float power,
                 const bool castShadows,
                 const float shadowNearPlane,
                 const float shadowFarPlane,
                 const float shadowExtent) {
            const auto colorF = color.divide() * power;

            data.color = Vec4f(colorF.x, colorF.y, colorF.z, 0);
            data.direction = Vec4f(direction.x, direction.y, direction.z, 0);

            data.shadowFarPlane = Vec4f(shadowFarPlane, 0, 0, 0);

            if (castShadows) {
                data.shadowProjectionMatrix = getShadowProjection(Transform({}, direction, {}),
                                                                   shadowNearPlane,
                                                                   shadowFarPlane,
                                                                   shadowExtent);
            }

            onChangedCallback();
        }

        [[nodiscard]] const ShaderDirectionalLight::CPU &getData() const {
            return data;
        }

    private:
        std::function<void()> onChangedCallback;
        ShaderDirectionalLight::CPU data{};
    };
}

#endif //XENGINE_RENDERDIRECTIONALLIGHT_HPP
