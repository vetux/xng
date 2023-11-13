/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2023  Julian Zampiccoli
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

#include "gpu/opengl/oglvideomemory.hpp"
#include "gpu/opengl/oglrenderdevice.hpp"

namespace xng::opengl {
    std::unique_ptr<VertexBuffer> opengl::OGLVideoMemory::createVertexBuffer(const VertexBufferDesc &desc,
                                                                             size_t offset) {
        return device->createVertexBuffer(desc);
    }

    std::unique_ptr<IndexBuffer> opengl::OGLVideoMemory::createIndexBuffer(const IndexBufferDesc &desc,
                                                                           size_t offset) {
        return device->createIndexBuffer(desc);
    }

    std::unique_ptr<ShaderUniformBuffer> opengl::OGLVideoMemory::createShaderBuffer(const ShaderUniformBufferDesc &desc,
                                                                                    size_t offset) {
        return device->createShaderUniformBuffer(desc);
    }

    std::unique_ptr<TextureBuffer> opengl::OGLVideoMemory::createTextureBuffer(const TextureBufferDesc &desc,
                                                                               size_t offset) {
        return device->createTextureBuffer(desc);
    }

    std::unique_ptr<TextureArrayBuffer>
    opengl::OGLVideoMemory::createTextureArrayBuffer(const TextureArrayBufferDesc &desc,
                                                     size_t offset) {
        return device->createTextureArrayBuffer(desc);
    }
}