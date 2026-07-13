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

#ifndef XENGINE_RENDERPIPELINE_HPP
#define XENGINE_RENDERPIPELINE_HPP

#include "xng/renderer/objects/rendertexture.hpp"

#include "xng/renderer/pipeline/rendershadercompiler.hpp"
#include "xng/renderer/pipeline/renderbatch.hpp"

namespace xng {
    /**
     * The RenderPipeline represents the rendering and allocation technique for RenderObjects.
     * This allows swapping the rendering / allocation technique at runtime (E.g. No Indirect draw on mobile platforms)
     *
     * Because the RenderObject layer is hardcoded to the set of available data and not extensible by users,
     * the renderer can abstract the rendering technique fully down to the shader level.
     *
     * This allows users to write pure shader code without having to write a custom pass. (In the future the editor will use this for the custom shading language and / or node-based shaders)
     */
    class RenderPipeline {
    public:
        typedef std::variant<rg::Attachment, RenderObjectHandle<RenderTexture> > Attachment;

        struct Binding {
            rg::Resource<rg::Buffer> buffer;
            size_t offset{};
            size_t size{};
        };

        virtual ~RenderPipeline() = default;

        /**
         * Users are free to do anything between input / output such as geometry stage etc. and may bind and access custom data.
         */
        virtual RenderShaderCompiler &getShaderCompiler() = 0;

        /**
         * Draw the specified draw list using the supplied pipeline, attachments and additional bindings.
         *
         * @param shader The shader to use for drawing.
         * @param batch The batch to draw.
         * @param attachments The Attachments to bind. (Type / Format must match the format in the shader)
         * @param parameters Optional user-supplied shader parameters.
         * @param storageBuffers Optional user-supplied storage buffer bindings.
         * @param textureArrays Optional user-supplied texture bindings.
         * @return The Pass for performing the draw.
         */
        virtual rg::RasterPass draw(const RenderShader &shader,
                                    const RenderBatch &batch,
                                    std::vector<Attachment> attachments,
                                    std::unordered_map<std::string, rg::ShaderPrimitive> parameters,
                                    std::unordered_map<std::string, Binding> storageBuffers,
                                    std::unordered_map<std::string, std::vector<rg::TextureBinding> > textureArrays) =0;
    };
}

#endif //XENGINE_RENDERPIPELINE_HPP
