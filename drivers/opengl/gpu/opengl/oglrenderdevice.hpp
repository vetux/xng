/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2022  Julian Zampiccoli
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

#include "xng/gpu/renderdevice.hpp"

#include <utility>

#include "opengl_include.hpp"

#include "gpu/opengl/oglrenderpipeline.hpp"
#include "gpu/opengl/oglrendertarget.hpp"
#include "gpu/opengl/ogltexturebuffer.hpp"
#include "gpu/opengl/oglvertexbuffer.hpp"
#include "gpu/opengl/oglshaderprogram.hpp"
#include "gpu/opengl/oglshaderbuffer.hpp"

namespace xng::opengl {
    class OGLRenderDevice : public RenderDevice {
    public:
        RenderDeviceInfo info;
        std::set<RenderObject *> objects;

        std::function<void(RenderObject *)> destructor;

        explicit OGLRenderDevice(RenderDeviceInfo info) : info(std::move(info)) {
            destructor = [this](RenderObject *obj) {
                objects.erase(obj);
            };
        }

        ~OGLRenderDevice() override {
            std::set<RenderObject *> o = objects;
            for (auto *ptr: o) {
                delete ptr;
            }
        }

        std::set<RenderObject *> getAllocatedObjects() override {
            return objects;
        }

        const RenderDeviceInfo &getInfo() override {
            return info;
        }

        std::unique_ptr<RenderPipeline> createRenderPipeline(const RenderPipelineDesc &desc,
                                                             ShaderProgram &shader) override {
            auto ret = std::make_unique<OGLRenderPipeline>(destructor, desc, dynamic_cast<OGLShaderProgram &>(shader));
            checkGLError();
            return ret;
        }

        std::unique_ptr<RenderPipeline> createRenderPipeline(const uint8_t *cacheData, size_t size) override {
            throw std::runtime_error("Not implemented");
        }

        std::unique_ptr<ComputePipeline> createComputePipeline(const ComputePipelineDesc &desc) override {
            throw std::runtime_error("Not implemented");
        }

        std::unique_ptr<RaytracePipeline> createRaytracePipeline(const RaytracePipelineDesc &desc) override {
            throw std::runtime_error("Not implemented");
        }

        std::unique_ptr<RenderTarget> createRenderTarget(const RenderTargetDesc &desc) override {
            auto ret = std::make_unique<OGLRenderTarget>(destructor, desc);
            checkGLError();
            return ret;
        }

        std::unique_ptr<TextureBuffer> createTextureBuffer(const TextureBufferDesc &desc) override {
            auto ret = std::make_unique<OGLTextureBuffer>(destructor, desc);
            checkGLError();
            return ret;
        }

        std::unique_ptr<VertexBuffer> createVertexBuffer(const VertexBufferDesc &desc) override {
            auto ret = std::make_unique<OGLVertexBuffer>(destructor, desc);
            checkGLError();
            return ret;
        }

        std::unique_ptr<ShaderProgram> createShaderProgram(const SPIRVDecompiler &decompiler,
                                                           const ShaderProgramDesc &desc) override {
            auto ret = std::make_unique<OGLShaderProgram>(destructor, decompiler, desc);
            checkGLError();
            return ret;
        }

        std::unique_ptr<ShaderBuffer> createShaderBuffer(const ShaderBufferDesc &desc) override {
            auto ret = std::make_unique<OGLShaderBuffer>(destructor, desc);
            checkGLError();
            return ret;
        }
    };
}

#endif //XENGINE_OGLRENDERDEVICE_HPP
