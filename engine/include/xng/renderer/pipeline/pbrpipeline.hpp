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

#ifndef XENGINE_PBRMATERIAL_HPP
#define XENGINE_PBRMATERIAL_HPP
#include "renderpipeline.hpp"
#include "rendershader.hpp"

namespace xng {
    class PBRPipeline {
    public:
        enum InstanceAttributes : RenderShader::InstanceAttributeID {
            MATERIAL_ALBEDO_COLOR,
            MATERIAL_ALBEDO_TEXTURE,
            RECEIVE_SHADOWS,
        };

        enum GlobalAttributes : RenderShader::GlobalAttributeID {
            CONFIG_GAMMA,
        };

        enum ArrayAttributes : RenderShader::ArrayAttributeID {
            POINT_LIGHTS,
        };

        enum PointLightAttributes : RenderShader::ArrayAttributeID {
            POINT_LIGHT_COLOR,
            POINT_LIGHT_POSITION,
            POINT_LIGHT_CAST_SHADOW,
            POINT_LIGHT_SHADOW_FAR_PLANE,
            POINT_LIGHT_SHADOW_PROJECTION,
        };

        RenderPipeline::Attributes getAttributes() const {
            RenderPipeline::Attributes attributes;

            RenderPipeline::ArrayAttribute pointLight;
            pointLight.attributes[POINT_LIGHT_COLOR] = RenderPipeline::Attribute{rg::ShaderPrimitiveType::vec4()};

            attributes.instanceAttributeTypes[MATERIAL_ALBEDO_COLOR] = rg::ShaderPrimitiveType::vec4();
            attributes.arrayAttributeTypes[POINT_LIGHTS] = pointLight;
        }

        void asdf() {
            RenderShaderCompiler c;
            c.getInstanceAttribute(MATERIAL_ALBEDO_COLOR);
            c.getArrayAttribute(POINT_LIGHTS, POINT_LIGHT_COLOR, index);
        }
    };
}
#endif //XENGINE_PBRMATERIAL_HPP