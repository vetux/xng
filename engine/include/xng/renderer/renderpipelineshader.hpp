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

#ifndef XENGINE_RENDERPIPELINESHADER_HPP
#define XENGINE_RENDERPIPELINESHADER_HPP

#include <unordered_set>

#include "xng/rendergraph/shader/shader.hpp"

#include "xng/renderer/vertexattribute.hpp"

namespace xng {
    class RenderPipelineShader {
    public:
        enum Attribute : int {
            // Either sampled material textures or material color.
            MATERIAL_ALBEDO = 0,
            MATERIAL_METALLIC,
            MATERIAL_ROUGHNESS,
            MATERIAL_AMBIENT_OCCLUSION,
            MATERIAL_NORMAL,

            // In the fragment stage either vertex normal or transformed texture normal is used, and with current abstraction the vertex -> fragment data is user-controlled.
            MATERIAL_HAS_NORMAL,

            TRANSFORM_MODEL_VIEW_PROJECTION,

            POINT_LIGHT_COUNT,
            DIRECTIONAL_LIGHT_COUNT,
            SPOT_LIGHT_COUNT,

            // Indexed attributes
            POINT_LIGHT_COLOR,
            POINT_LIGHT_POSITION,
            POINT_LIGHT_SHADOW_FAR_PLANE,
            POINT_LIGHT_SHADOW_PROJECTION, // Array[6] of mat4

            DIRECTIONAL_LIGHT_COLOR,
            DIRECTIONAL_LIGHT_DIRECTION,
            DIRECTIONAL_LIGHT_SHADOW_FAR_PLANE,
            DIRECTIONAL_LIGHT_SHADOW_PROJECTION, // mat4

            SPOT_LIGHT_COLOR,
            SPOT_LIGHT_DIRECTION,
            SPOT_LIGHT_POSITION,
            SPOT_LIGHT_QUADRATIC,
            SPOT_LIGHT_CUT_OFF,
            SPOT_LIGHT_OUTER_CUT_OFF,
            SPOT_LIGHT_CONSTANT,
            SPOT_LIGHT_LINEAR,
            SPOT_LIGHT_SHADOW_FAR_PLANE,
            SPOT_LIGHT_SHADOW_PROJECTION, // mat4
        };

        struct Attachment {
            enum Type : int {
                ATTACHMENT_NATIVE = 0, // rg::Attachment
                ATTACHMENT_TEXTURE, // RenderTexture
            } type{};

            rg::ShaderPrimitiveType value{};
        };

        virtual ~RenderPipelineShader() = default;

        /**
         * Retrieve vertex attribute value. (Only valid in Vertex stage)
         *
         * @param attr
         * @return
         */
        virtual rg::ShaderInstruction getVertexAttribute(VertexAttribute attr) = 0;

        /**
         * Retrieve non indexed attribute value.
         *
         * @param attr
         * @return
         */
        virtual rg::ShaderInstruction getAttribute(Attribute attr) = 0;

        /**
         * Retrieve the indexed attribute value.
         *
         * @param attr
         * @param index
         * @return
         */
        virtual rg::ShaderInstruction getAttribute(Attribute attr, const rg::ShaderInstruction &index) = 0;

        /**
         * Write the specified color value to the specified attachment.
         *
         * This wraps virtual texture write if the shader attachment is a virtual texture.
         *
         * @param index
         * @param color
         * @return
         */
        virtual rg::ShaderInstruction writeAttachment(unsigned int index, const rg::ShaderInstruction &color) = 0;

        /**
         * Inject the pipeline dependent required bindings, parameters, functions and vertex layout into the passed shader.
         *
         * Stage must be either VERTEX or FRAGMENT.
         *
         * @param shader The shader to inject into.
         * @param attachments The format of the attachments.
         * @param attrs The set of accessed attributes.
         */
        virtual void inject(rg::Shader &shader,
                            const std::vector<Attachment> &attachments,
                            const std::unordered_set<Attribute> &attrs) = 0;
    };
}

#endif //XENGINE_RENDERPIPELINESHADER_HPP
