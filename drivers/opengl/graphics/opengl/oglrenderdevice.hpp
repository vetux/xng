/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2021  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef XENGINE_OGLRENDERDEVICE_HPP
#define XENGINE_OGLRENDERDEVICE_HPP

#include "graphics/renderdevice.hpp"

#include "graphics/opengl/oglbuildmacro.hpp"

#include "graphics/opengl/oglrenderpipeline.hpp"
#include "graphics/opengl/oglrendertarget.hpp"
#include "graphics/opengl/ogltexturebuffer.hpp"
#include "graphics/opengl/oglvertexbuffer.hpp"
#include "graphics/opengl/oglshaderprogram.hpp"
#include "graphics/opengl/oglshaderbuffer.hpp"

namespace xng::opengl {
    class OPENGL_TYPENAME(RenderDevice) : public RenderDevice OPENGL_INHERIT {
    public:
        OPENGL_TYPENAME(RenderDevice)() {
            initialize();
        }

        ~OPENGL_TYPENAME(RenderDevice)() override = default;

        int getMaxSampleCount() override {
            GLint ret = 0;
            glGetIntegerv(GL_MAX_SAMPLES, &ret);
            return ret;
        }

        std::unique_ptr<RenderPipeline> createPipeline(const RenderPipelineDesc &desc) override {
            return std::make_unique<OPENGL_TYPENAME(RenderPipeline)>(desc);
        }

        std::unique_ptr<RenderPipeline> createPipeline(const uint8_t *cacheData, size_t size) override {
            throw std::runtime_error("Not implemented");
        }

        std::unique_ptr<RenderTarget> createRenderTarget(const RenderTargetDesc &desc) override {
            return std::make_unique<OPENGL_TYPENAME(RenderTarget)>(desc);
        }

        std::unique_ptr<TextureBuffer> createTextureBuffer(const TextureBufferDesc &desc) override {
            return std::make_unique<OPENGL_TYPENAME(TextureBuffer)>(desc);
        }

        std::unique_ptr<VertexBuffer> createVertexBuffer(const VertexBufferDesc &desc) override {
            return std::make_unique<OPENGL_TYPENAME(VertexBuffer)>(desc);
        }

        std::unique_ptr<ShaderProgram> createShaderProgram(const ShaderProgramDesc &desc) override {
            return std::make_unique<OPENGL_TYPENAME(ShaderProgram)>(desc);
        }

        std::unique_ptr<ShaderBuffer> createShaderBuffer(const ShaderBufferDesc &desc) override {
            return std::make_unique<OPENGL_TYPENAME(ShaderBuffer)>(desc);
        }

        OPENGL_MEMBERS
    };
}

#endif //XENGINE_OGLRENDERDEVICE_HPP
