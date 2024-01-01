/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2024  Julian Zampiccoli
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

#ifndef XENGINE_OGLVIDEOMEMORY_HPP
#define XENGINE_OGLVIDEOMEMORY_HPP

#include "xng/gpu/videomemory.hpp"

namespace xng::opengl {
    class OGLRenderDevice;

    class OGLVideoMemory : public VideoMemory {
    public:
        OGLRenderDevice *device;
        VideoMemoryDesc desc;

        OGLVideoMemory(OGLRenderDevice &device,
                       VideoMemoryDesc desc)
                : device(&device),
                  desc(desc) {}

        VideoMemoryDesc getDescription() override {
            return desc;
        }

        size_t getBufferSize(const VertexBufferDesc &desc) override {
            return 1;
        }

        size_t getBufferSize(const IndexBufferDesc &desc) override {
            return 1;
        }

        size_t getBufferSize(const ShaderUniformBufferDesc &desc) override {
            return 1;
        }

        size_t getBufferSize(const TextureBufferDesc &desc) override {
            return 1;
        }

        size_t getBufferSize(const TextureArrayBufferDesc &desc) override {
            return 1;
        }

        std::unique_ptr<VertexBuffer> createVertexBuffer(const VertexBufferDesc &desc, size_t offset) override;

        std::unique_ptr<IndexBuffer> createIndexBuffer(const IndexBufferDesc &desc, size_t offset) override;

        std::unique_ptr<ShaderUniformBuffer> createShaderBuffer(const ShaderUniformBufferDesc &desc, size_t offset) override;

        std::unique_ptr<TextureBuffer> createTextureBuffer(const TextureBufferDesc &desc, size_t offset) override;

        std::unique_ptr<TextureArrayBuffer> createTextureArrayBuffer(const TextureArrayBufferDesc &desc,
                                                                     size_t offset) override;
    };
}

#endif //XENGINE_OGLVIDEOMEMORY_HPP
