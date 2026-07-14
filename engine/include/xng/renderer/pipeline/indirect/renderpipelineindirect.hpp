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

#ifndef XENGINE_RENDERPIPELINEINDIRECT_HPP
#define XENGINE_RENDERPIPELINEINDIRECT_HPP

#include "xng/renderer/pipeline/renderpipeline.hpp"

#include "xng/renderer/pipeline/indirect/rendershadercompilerindirect.hpp"
#include "xng/renderer/pipeline/indirect/renderbatchindirect.hpp"

namespace xng {
    class RenderPipelineIndirect final : public RenderPipeline {
    public:
        ~RenderPipelineIndirect() override;

        RenderShaderCompiler &getShaderCompiler() override {
            return compiler;
        }

        rg::RasterPass draw(const RenderShader &shader,
                            const RenderBatch &batch,
                            std::vector<Attachment> attachments,
                            std::unordered_map<std::string, rg::ShaderPrimitive> parameters,
                            std::unordered_map<std::string, Binding> storageBuffers,
                            std::unordered_map<std::string, std::vector<rg::TextureBinding> > textureArrays) override {
        }

    private:
        RenderShaderCompilerIndirect compiler;
    };
}

#endif //XENGINE_RENDERPIPELINEINDIRECT_HPP
