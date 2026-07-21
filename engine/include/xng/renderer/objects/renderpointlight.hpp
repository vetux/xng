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

#ifndef XENGINE_RENDERPOINTLIGHT_HPP
#define XENGINE_RENDERPOINTLIGHT_HPP

#include "xng/assets/color.hpp"
#include "xng/renderer/renderobject.hpp"

namespace xng {
    typedef std::array<Mat4f, 6> Mat4f_Array_6;

    ShaderStruct(ShaderPointLight,
                 Vec4f, position,
                 Vec4f, color,
                 Vec4f, shadowFarPlane,
                 Mat4f_Array_6, shadowMatrices)

    class RenderPointLight final : public RenderObject {
    public:
        explicit RenderPointLight(std::function<void()> onChangedCallback)
            : onChangedCallback(std::move(onChangedCallback)) {
        }

        ~RenderPointLight() override = default;

        void set(const Vec3f &position,
                 const ColorRGB color,
                 const float power,
                 const bool castShadows,
                 const float shadowNearPlane,
                 const float shadowFarPlane) {
            const auto colorF = color.divide() * power;

            data.position = Vec4f(position.x, position.y, position.z, 0);
            data.color = Vec4f(colorF.x, colorF.y, colorF.z, 0);

            data.shadowFarPlane = Vec4f(shadowFarPlane, 0, 0, 0);

            if (castShadows) {
                const auto shadowProj = MatrixMath::perspective(90.0f, 1, shadowNearPlane, shadowFarPlane);

                Mat4f_Array_6 shadowMatrices;

                shadowMatrices[0] = (shadowProj *
                                     MatrixMath::lookAt(position,
                                                        position + Vec3f(1.0, 0.0, 0.0),
                                                        Vec3f(0.0, -1.0, 0.0)));
                shadowMatrices[1] = (shadowProj *
                                     MatrixMath::lookAt(position,
                                                        position + Vec3f(-1.0, 0.0, 0.0),
                                                        Vec3f(0.0, -1.0, 0.0)));
                shadowMatrices[2] = (shadowProj *
                                     MatrixMath::lookAt(position,
                                                        position + Vec3f(0.0, 1.0, 0.0),
                                                        Vec3f(0.0, 0.0, -1.0)));
                shadowMatrices[3] = (shadowProj *
                                     MatrixMath::lookAt(position,
                                                        position + Vec3f(0.0, -1.0, 0.0),
                                                        Vec3f(0.0, 0.0, 1.0)));
                shadowMatrices[4] = (shadowProj *
                                     MatrixMath::lookAt(position,
                                                        position + Vec3f(0.0, 0.0, 1.0),
                                                        Vec3f(0.0, -1.0, 0.0)));
                shadowMatrices[5] = (shadowProj *
                                     MatrixMath::lookAt(position,
                                                        position + Vec3f(0.0, 0.0, -1.0),
                                                        Vec3f(0.0, -1.0, 0.0)));

                data.shadowMatrices = shadowMatrices;
            }

            onChangedCallback();
        }

        [[nodiscard]] const ShaderPointLight::CPU &getData() const {
            return data;
        }

    private:
        std::function<void()> onChangedCallback;
        ShaderPointLight::CPU data{};
    };
}

#endif //XENGINE_RENDERPOINTLIGHT_HPP
