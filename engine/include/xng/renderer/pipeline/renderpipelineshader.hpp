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

#include "xng/rendergraph/pipelinecache.hpp"
#include "xng/renderer/pipeline/renderpipelinematerial.hpp"

namespace xng {
    /**
     * Because the RenderObject layer already defines a hardcoded set of features
     * the pipeline can abstract the rendering technique fully down to the shader level.
     *
     * This allows users to write pure shader code without having to write a custom pass. (In the future the editor will use this for the custom shading language and / or node-based shaders)
     *
     * TODO: Shader Permutations
     *
     * The pipeline can here permutate based on texture and attachment types.
     * So there would be 2 levels of permutations one handled by users for concepts outside pipeline scope such
     * as "HAS_NORMAL_MAP" etc. and the pipeline would then permutate on the user permutated shader based on the
     * texture types in the material.
     */
    class RenderPipelineShader {
    public:
        struct Attachment {
            enum Type : int {
                ATTACHMENT_NATIVE = 0, // rg::Attachment
                ATTACHMENT_TEXTURE, // RenderTexture
            } type{};

            rg::ShaderPrimitiveType value{};
            rg::ColorFormat format{};

            Attachment() = default;

            Attachment(const Type type, const rg::ShaderPrimitiveType value, const rg::ColorFormat format)
                : type(type), value(value), format(format) {
            }
        };

        RenderPipelineShader(rg::PipelineCache &cache,
                             const rg::PipelineCache::Handle pipeline,
                             std::vector<Attachment> _attachments)
            : cache(cache),
              pipeline(pipeline),
              attachments(std::move(_attachments)) {
        }

        ~RenderPipelineShader() {
            cache.destroy(pipeline);
        }

        [[nodiscard]] rg::PipelineCache::Handle getPipeline() const {
            return pipeline;
        }

    private:
        rg::PipelineCache &cache;
        rg::PipelineCache::Handle pipeline;

        std::vector<Attachment> attachments;
    };
}

#endif //XENGINE_RENDERPIPELINESHADER_HPP
