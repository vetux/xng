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

#ifndef XENGINE_RENDERSHADER_HPP
#define XENGINE_RENDERSHADER_HPP

#include "xng/rendergraph/pipelinecache.hpp"

namespace xng {
    /**
     * TODO: Shader Permutations
     *
     * This is where permutations would live based on material combinations including backing method and the pipeline
     * can bind different sub shaders based on accessed attribute combination.
     *
     * This also means permutations are only at pipeline level and passes would have to define their own permutations
     * separately but i think this is fine because permutations outside of the render object scope are pass specific anyway.
     */
    class RenderShader {
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

            // Either sampled paint texture with color mixed in or the paint color.
            PAINT_COLOR,

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

        RenderShader(rg::PipelineCache &cache,
                     const rg::PipelineCache::Handle pipeline,
                     std::vector<Attachment> _attachments,
                     std::unordered_set<Attribute> _attributes,
                     std::unordered_set<VertexAttribute> _vertexAttributes)
            : cache(cache),
              pipeline(pipeline),
              attachments(std::move(_attachments)),
              attributes(std::move(_attributes)),
              vertexAttributes(std::move(_vertexAttributes)) {
        }

        ~RenderShader() {
            cache.destroy(pipeline);
        }

    private:
        rg::PipelineCache &cache;
        rg::PipelineCache::Handle pipeline;

        std::vector<Attachment> attachments;
        std::unordered_set<Attribute> attributes;
        std::unordered_set<VertexAttribute> vertexAttributes;
    };
}

#endif //XENGINE_RENDERSHADER_HPP
