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

#ifndef XENGINE_MATERIALPBR_HPP
#define XENGINE_MATERIALPBR_HPP

#include "xng/renderer/pipeline/renderpipeline.hpp"

namespace xng {
    class MaterialPBR {
    public:
        enum Attribute : RenderPipelineMaterial::AttributeID {
            MATERIAL_ALBEDO_COLOR = 0,
            MATERIAL_ALBEDO_TEXTURE,
            MATERIAL_ALBEDO_HAS_TEXTURE,

            MATERIAL_METALLIC_COLOR,
            MATERIAL_METALLIC_TEXTURE,
            MATERIAL_METALLIC_HAS_TEXTURE,

            MATERIAL_ROUGHNESS_COLOR,
            MATERIAL_ROUGHNESS_TEXTURE,
            MATERIAL_ROUGHNESS_HAS_TEXTURE,

            MATERIAL_AMBIENT_OCCLUSION_COLOR,
            MATERIAL_AMBIENT_OCCLUSION_TEXTURE,
            MATERIAL_AMBIENT_OCCLUSION_HAS_TEXTURE,

            MATERIAL_EMISSIVE_COLOR,
            MATERIAL_EMISSIVE_TEXTURE,
            MATERIAL_EMISSIVE_HAS_TEXTURE,

            MATERIAL_NORMAL_TEXTURE,
            MATERIAL_NORMAL_HAS_TEXTURE,

            RECEIVE_SHADOWS,
        };

        static RenderPipeline::MaterialAttributes getAttributes() {
            RenderPipeline::MaterialAttributes ret;

            ret[MATERIAL_ALBEDO_COLOR] = getPrimitive(rg::ShaderPrimitiveType::vec4());
            ret[MATERIAL_ALBEDO_TEXTURE] = getTexture();
            ret[MATERIAL_ALBEDO_HAS_TEXTURE] = getPrimitive(rg::ShaderPrimitiveType::Bool());

            ret[MATERIAL_METALLIC_COLOR] = getPrimitive(rg::ShaderPrimitiveType::Float());
            ret[MATERIAL_METALLIC_TEXTURE] = getTexture();
            ret[MATERIAL_METALLIC_HAS_TEXTURE] = getPrimitive(rg::ShaderPrimitiveType::Bool());

            ret[MATERIAL_ROUGHNESS_COLOR] = getPrimitive(rg::ShaderPrimitiveType::Float());
            ret[MATERIAL_ROUGHNESS_TEXTURE] = getTexture();
            ret[MATERIAL_ROUGHNESS_HAS_TEXTURE] = getPrimitive(rg::ShaderPrimitiveType::Bool());

            ret[MATERIAL_AMBIENT_OCCLUSION_COLOR] = getPrimitive(
                rg::ShaderPrimitiveType::Float());
            ret[MATERIAL_AMBIENT_OCCLUSION_TEXTURE] = getTexture();
            ret[MATERIAL_AMBIENT_OCCLUSION_HAS_TEXTURE] = getPrimitive(
                rg::ShaderPrimitiveType::Bool());

            ret[MATERIAL_EMISSIVE_COLOR] = getPrimitive(rg::ShaderPrimitiveType::vec3());
            ret[MATERIAL_EMISSIVE_TEXTURE] = getTexture();
            ret[MATERIAL_EMISSIVE_HAS_TEXTURE] = getPrimitive(rg::ShaderPrimitiveType::Bool());

            ret[MATERIAL_NORMAL_TEXTURE] = getTexture();
            ret[MATERIAL_NORMAL_HAS_TEXTURE] = getPrimitive(rg::ShaderPrimitiveType::Bool());

            return ret;
        }

    private:
        static RenderPipelineMaterial::AttributeType getPrimitive(const rg::ShaderPrimitiveType type) {
            return RenderPipelineMaterial::AttributeType::primitive(type);
        }

        static RenderPipelineMaterial::AttributeType getTexture() {
            return RenderPipelineMaterial::AttributeType::texture();
        }
    };
}
#endif //XENGINE_MATERIALPBR_HPP
