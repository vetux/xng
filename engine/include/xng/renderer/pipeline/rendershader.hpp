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
        /**
         * Per draw attributes.
         */
        enum InstanceAttribute : int {
            TRANSFORM_MODEL_VIEW_PROJECTION = 0,
            TRANSFORM_MODEL,
            TRANSFORM_VIEW,
            TRANSFORM_PROJECTION,

            // Either sampled material textures or material color.
            MATERIAL_ALBEDO,
            MATERIAL_METALLIC,
            MATERIAL_ROUGHNESS,
            MATERIAL_AMBIENT_OCCLUSION,
            MATERIAL_EMISSIVE,
            MATERIAL_HEIGHT_MAP,
            MATERIAL_NORMAL,
            MATERIAL_HAS_NORMAL,

            RECEIVE_SHADOWS,
        };

        /**
         * Pipeline invocation wide attributes.
         */
        enum GlobalAttribute : int {
            CONFIG_GAMMA = 0,

            CAMERA_POSITION,

            POINT_LIGHT_COUNT,
            DIRECTIONAL_LIGHT_COUNT,
            SPOT_LIGHT_COUNT,
        };

        /**
         * Pipeline invocation wide indexed attributes
         */
        enum IndexedAttribute : int {
            POINT_LIGHT_COLOR,
            POINT_LIGHT_POSITION,
            POINT_LIGHT_CAST_SHADOW,
            POINT_LIGHT_SHADOW_FAR_PLANE,
            POINT_LIGHT_SHADOW_PROJECTION, // Array[6] of mat4

            DIRECTIONAL_LIGHT_COLOR,
            DIRECTIONAL_LIGHT_DIRECTION,
            DIRECTIONAL_LIGHT_CAST_SHADOW,
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
            SPOT_LIGHT_CAST_SHADOW,
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
                     std::unordered_set<VertexAttribute> _vertexAttributes,
                     std::unordered_set<InstanceAttribute> _instanceAttributes,
                     std::unordered_set<GlobalAttribute> _globalAttributes,
                     std::unordered_set<IndexedAttribute> _indexedAttributes)
            : cache(cache),
              pipeline(pipeline),
              attachments(std::move(_attachments)),
              vertexAttributes(std::move(_vertexAttributes)),
              instanceAttributes(std::move(_instanceAttributes)),
              globalAttributes(std::move(_globalAttributes)),
              indexedAttributes(std::move(_indexedAttributes)) {
        }

        ~RenderShader() {
            cache.destroy(pipeline);
        }

    private:
        rg::PipelineCache &cache;
        rg::PipelineCache::Handle pipeline;

        std::vector<Attachment> attachments;

        std::unordered_set<VertexAttribute> vertexAttributes;

        std::unordered_set<InstanceAttribute> instanceAttributes;
        std::unordered_set<GlobalAttribute> globalAttributes;
        std::unordered_set<IndexedAttribute> indexedAttributes;
    };
}

#endif //XENGINE_RENDERSHADER_HPP
