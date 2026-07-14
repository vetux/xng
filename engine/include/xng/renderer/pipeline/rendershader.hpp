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
     * Because the RenderObject layer already defines a hardcoded set of features
     * the pipeline can abstract the rendering technique fully down to the shader level.
     *
     * This allows users to write pure shader code without having to write a custom pass. (In the future the editor will use this for the custom shading language and / or node-based shaders)
     *
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
        typedef size_t InstanceAttributeID;

        typedef size_t GlobalAttributeID;

        typedef size_t ArrayID;

        typedef size_t ArrayAttributeID;

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
                     std::unordered_set<InstanceAttributeID> _instanceAttributes,
                     std::unordered_set<GlobalAttributeID> _globalAttributes,
                     std::unordered_set<ArrayID> _indexedAttributes)
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

        std::unordered_set<InstanceAttributeID> instanceAttributes;
        std::unordered_set<GlobalAttributeID> globalAttributes;
        std::unordered_set<ArrayID> indexedAttributes;
    };
}

#endif //XENGINE_RENDERSHADER_HPP
