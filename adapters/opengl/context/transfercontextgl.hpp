/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
 *
 *  This program is free software{} you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation{} either
 *  version 3 of the License, or (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY{} without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.

 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program{} if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef XENGINE_TRANSFERCONTEXTGL_HPP
#define XENGINE_TRANSFERCONTEXTGL_HPP

#include "xng/rendergraph/context/transfercontext.hpp"
#include "xng/rendergraph/statistics.hpp"

#include "heapgl.hpp"
#include "passresources.hpp"
#include "resource/framebuffer.hpp"

namespace xng::opengl {
    class TransferContextGL final : public rendergraph::TransferContext {
    public:
        TransferContextGL(PassResources &&resources, Statistics &stats)
            : resources(std::move(resources)), stats(stats) {
        }

        ~TransferContextGL() override = default;

        void uploadBuffer(const Resource<Buffer> &target,
                          const uint8_t *buffer,
                          const size_t bufferSize,
                          const size_t targetOffset) override {
            oglDebugStartGroup("TransferContextGL::uploadBuffer");

            const auto &buf = resources.getBuffer(target);
            glBindBuffer(buf.target, buf.handle);
            glBufferSubData(buf.target,
                            static_cast<GLintptr>(targetOffset),
                            static_cast<GLsizeiptr>(bufferSize),
                            buffer);
            glBindBuffer(buf.target, 0);
            oglCheckError();

            stats.bufferVRamUpload += bufferSize;

            oglDebugEndGroup();
        }

        void uploadTexture(const Resource<Texture> &texture,
                           const Texture::SubResource target,
                           const uint8_t *buffer,
                           const size_t bufferSize,
                           const ColorFormat bufferFormat,
                           const Vec2i &offset,
                           const Vec2i &size) override {
            oglDebugStartGroup("TransferContextGL::uploadTexture");

            auto &tex = resources.getTexture(texture);

            if (bufferSize > tex.desc.size.x * tex.desc.size.y * getColorByteSize(tex.desc.format)) {
                throw std::runtime_error("Invalid buffer size");
            }

            // Invert the rows because opengl requires bottom-up texture data
            auto *bufferCpy = new uint8_t[bufferSize];
            for (auto row = 0; row < size.y; ++row) {
                auto dstOffset = row * size.x * getColorByteSize(bufferFormat);
                auto srcOffset = (size.y - row - 1) * size.x * getColorByteSize(bufferFormat);
                memcpy(bufferCpy + dstOffset, buffer + srcOffset, size.x * getColorByteSize(bufferFormat));
            }

            const auto &textureSize = tex.desc.size;

            glBindTexture(tex.textureType, tex.handle);

            // determine pixel format (GL_RGB/GL_RGBA/GL_RED/GL_RG) and data type for upload
            GLenum dataType = GL_UNSIGNED_BYTE;
            GLenum pixelFormat = GL_RGBA;

            switch (bufferFormat) {
                case ColorFormat::R:
                case ColorFormat::R8:
                case ColorFormat::R16:
                case ColorFormat::R16F:
                case ColorFormat::R32F:
                case ColorFormat::R8I:
                case ColorFormat::R16I:
                case ColorFormat::R32I:
                case ColorFormat::R8UI:
                case ColorFormat::R16UI:
                case ColorFormat::R32UI:
                    pixelFormat = GL_RED;
                    dataType = (bufferFormat == ColorFormat::R16F || bufferFormat == ColorFormat::R32F)
                                   ? GL_FLOAT
                                   : GL_UNSIGNED_BYTE;
                    break;
                case ColorFormat::RG:
                case ColorFormat::RG8:
                case ColorFormat::RG16:
                case ColorFormat::RG16F:
                case ColorFormat::RG32F:
                case ColorFormat::RG8I:
                case ColorFormat::RG16I:
                case ColorFormat::RG32I:
                case ColorFormat::RG8UI:
                case ColorFormat::RG16UI:
                case ColorFormat::RG32UI:
                    pixelFormat = GL_RG;
                    dataType = (bufferFormat == ColorFormat::RG16F || bufferFormat == ColorFormat::RG32F)
                                   ? GL_FLOAT
                                   : GL_UNSIGNED_BYTE;
                    break;
                case ColorFormat::RGB:
                case ColorFormat::RGB8:
                case ColorFormat::RGB16:
                case ColorFormat::RGB16F:
                case ColorFormat::RGB32F:
                case ColorFormat::RGB8I:
                case ColorFormat::RGB16I:
                case ColorFormat::RGB32I:
                case ColorFormat::RGB8UI:
                case ColorFormat::RGB16UI:
                case ColorFormat::RGB32UI:
                    pixelFormat = GL_RGB;
                    dataType = (bufferFormat == ColorFormat::RGB16F || bufferFormat == ColorFormat::RGB32F)
                                   ? GL_FLOAT
                                   : GL_UNSIGNED_BYTE;
                    break;
                default:
                    // Assume RGBA-like default
                    pixelFormat = GL_RGBA;
                    dataType = (bufferFormat == ColorFormat::RGBA16F || bufferFormat == ColorFormat::RGBA32F)
                                   ? GL_FLOAT
                                   : GL_UNSIGNED_BYTE;
                    break;
            }

            if (tex.textureType == GL_TEXTURE_2D) {
                glTexSubImage2D(GL_TEXTURE_2D,
                                static_cast<GLint>(target.mipLevel),
                                offset.x,
                                textureSize.y - offset.y - 1 - size.y + 1,
                                size.x,
                                size.y,
                                pixelFormat,
                                dataType,
                                bufferCpy);
            } else if (tex.textureType == GL_TEXTURE_CUBE_MAP) {
                glTexSubImage2D(convert(target.face),
                                static_cast<GLint>(target.mipLevel),
                                offset.x,
                                textureSize.y - offset.y - 1 - size.y + 1,
                                size.x,
                                size.y,
                                pixelFormat,
                                dataType,
                                bufferCpy);
            } else if (tex.textureType == GL_TEXTURE_2D_ARRAY) {
                glTexSubImage3D(tex.textureType,
                                static_cast<GLint>(target.mipLevel),
                                offset.x,
                                textureSize.y - offset.y - 1 - size.y + 1,
                                static_cast<GLint>(target.arrayLayer),
                                size.x,
                                size.y,
                                1,
                                pixelFormat,
                                dataType,
                                bufferCpy);
            } else if (tex.textureType == GL_TEXTURE_CUBE_MAP_ARRAY) {
                glTexSubImage3D(tex.textureType,
                                static_cast<GLint>(target.mipLevel),
                                offset.x,
                                textureSize.y - offset.y - 1 - size.y + 1,
                                static_cast<GLint>(target.arrayLayer) * 6 + target.face,
                                size.x,
                                size.y,
                                1,
                                pixelFormat,
                                dataType,
                                bufferCpy);
            } else {
                throw std::runtime_error("Invalid texture type");
            }

            delete[] bufferCpy;

            glBindTexture(tex.textureType, 0);

            oglCheckError();

            stats.textureVRamUpload += bufferSize;

            oglDebugEndGroup();
        }

        void copyBuffer(const Resource<Buffer> &target,
                        const Resource<Buffer> &source,
                        const size_t targetOffset,
                        const size_t sourceOffset,
                        const size_t count) override {
            oglDebugStartGroup("TransferContextGL::copyBuffer");

            const auto readBuffer = resources.getBuffer(source).handle;
            const auto writeBuffer = resources.getBuffer(target).handle;

            glBindBuffer(GL_COPY_READ_BUFFER, readBuffer);
            glBindBuffer(GL_COPY_WRITE_BUFFER, writeBuffer);
            glCopyBufferSubData(GL_COPY_READ_BUFFER,
                                GL_COPY_WRITE_BUFFER,
                                static_cast<GLintptr>(sourceOffset),
                                static_cast<GLintptr>(targetOffset),
                                static_cast<GLsizeiptr>(count));
            glBindBuffer(GL_COPY_READ_BUFFER, 0);
            glBindBuffer(GL_COPY_WRITE_BUFFER, 0);

            oglCheckError();

            stats.bufferVRamCopy += count;

            oglDebugEndGroup();
        }

        void copyTexture(const Resource<Texture> &target,
                         const Resource<Texture> &source,
                         const Vec3i &srcOffset,
                         const Vec3i &dstOffset,
                         const Vec3i &size,
                         const size_t srcMipMapLevel,
                         const size_t dstMipMapLevel) override {
            oglDebugStartGroup("TransferContextGL::copyTexture");

            const auto srcTexture = resources.getTexture(source);
            const auto dstTexture = resources.getTexture(target);
            glCopyImageSubData(srcTexture.handle,
                               srcTexture.textureType,
                               static_cast<GLint>(srcMipMapLevel),
                               srcOffset.x,
                               srcOffset.y,
                               srcOffset.z,
                               dstTexture.handle,
                               dstTexture.textureType,
                               static_cast<GLint>(dstMipMapLevel),
                               dstOffset.x,
                               dstOffset.y,
                               dstOffset.z,
                               srcTexture.desc.size.x,
                               srcTexture.desc.size.y,
                               1);
            oglCheckError();

            stats.textureVRamCopy += size.x * size.y * getColorByteSize(srcTexture.desc.format);

            oglDebugEndGroup();
        }

        void copyBufferToTexture(const Resource<Texture> &texture,
                                 const Resource<Buffer> &buffer,
                                 Texture::SubResource textureSubResource,
                                 size_t bufferOffset,
                                 const Recti &textureOffset) override {
            //TODO: PBO buffer copy
            throw std::runtime_error("Not implemented");
        }

        void copyTextureToBuffer(const Resource<Buffer> &buffer,
                                 const Resource<Texture> &texture,
                                 Texture::SubResource textureSubResource,
                                 size_t bufferOffset,
                                 const Recti &textureOffset) override {
            throw std::runtime_error("Not implemented");
        }

        void clearTexture(const Resource<Texture> &texture,
                          const Texture::SubResource &target,
                          const Texture::ClearValue &clearVal) override {
            oglDebugStartGroup("TransferContextGL::clearTexture");

            const auto &tex = resources.getTexture(texture);

            if (tex.desc.format == DEPTH_STENCIL
                || tex.desc.format == DEPTH24_STENCIL8) {
                //Workaround using the raster pipeline to clear because there doesn't seem to be a standard 24bit_8bit depth stencil format.
                const auto clearValue = std::get<Texture::DepthStencilClearValue>(clearVal);
                const auto clearFb = std::make_unique<Framebuffer>();
                glBindFramebuffer(GL_DRAW_FRAMEBUFFER, clearFb->FBO);
                Framebuffer::attach(GL_DEPTH_STENCIL_ATTACHMENT, tex, target);
                glClearDepth(clearValue.clearDepth);
                glClearStencil(static_cast<GLint>(clearValue.clearStencil));
                glDepthMask(GL_TRUE);
                glStencilMask(0xff);
                glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
                glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
            } else if (tex.desc.format == DEPTH
                       || tex.desc.format == DEPTH_32F
                       || tex.desc.format == DEPTH_24
                       || tex.desc.format == DEPTH_16) {
                const GLfloat depthData = std::get<float>(clearVal);
                glClearTexSubImage(tex.handle,
                                   static_cast<GLint>(target.mipLevel),
                                   0,
                                   0,
                                   tex.textureType == TEXTURE_CUBE_MAP
                                       ? static_cast<GLint>(target.arrayLayer * 6 + target.face)
                                       : static_cast<GLint>(target.arrayLayer),
                                   tex.desc.size.x,
                                   tex.desc.size.y,
                                   1,
                                   GL_DEPTH_COMPONENT,
                                   GL_FLOAT,
                                   &depthData);
            } else if (tex.desc.format == STENCIL
                       || tex.desc.format == STENCIL_32
                       || tex.desc.format == STENCIL_16
                       || tex.desc.format == STENCIL_8) {
                const GLuint stencilData = std::get<unsigned>(clearVal);
                glClearTexSubImage(tex.handle,
                                   static_cast<GLint>(target.mipLevel),
                                   0,
                                   0,
                                   tex.textureType == TEXTURE_CUBE_MAP
                                       ? static_cast<GLint>(target.arrayLayer * 6 + target.face)
                                       : static_cast<GLint>(target.arrayLayer),
                                   tex.desc.size.x,
                                   tex.desc.size.y,
                                   1,
                                   GL_STENCIL_INDEX,
                                   GL_UNSIGNED_INT,
                                   &stencilData);
            } else {
                const void *clearData = nullptr;
                GLenum format = GL_RGBA;
                GLenum type = GL_UNSIGNED_BYTE;

                GLfloat fClearData[4];
                GLint iClearData[4];
                GLuint uiClearData[4];

                switch (clearVal.index()) {
                    case 0:
                        format = GL_RGBA;
                        type = GL_UNSIGNED_BYTE;
                        clearData = &std::get<ColorRGBA>(clearVal);
                        break;
                    case 1: {
                        format = GL_RGBA;
                        type = GL_FLOAT;
                        const auto &vec = std::get<Vec4f>(clearVal);
                        fClearData[0] = vec.x;
                        fClearData[1] = vec.y;
                        fClearData[2] = vec.z;
                        fClearData[3] = vec.w;
                        clearData = fClearData;
                        break;
                    }
                    case 2: {
                        format = GL_RGBA_INTEGER;
                        type = GL_INT;
                        const auto &ivec = std::get<Vec4i>(clearVal);
                        iClearData[0] = ivec.x;
                        iClearData[1] = ivec.y;
                        iClearData[2] = ivec.z;
                        iClearData[3] = ivec.w;
                        clearData = iClearData;
                    }
                    case 3: {
                        format = GL_RGBA_INTEGER;
                        type = GL_UNSIGNED_INT;
                        const auto &uvec = std::get<Vec4u>(clearVal);
                        uiClearData[0] = uvec.x;
                        uiClearData[1] = uvec.y;
                        uiClearData[2] = uvec.z;
                        uiClearData[3] = uvec.w;
                        clearData = uiClearData;
                        break;
                    }
                    default:
                        throw std::runtime_error("Invalid clear value index");
                }
                glClearTexSubImage(tex.handle,
                                   static_cast<GLint>(target.mipLevel),
                                   0,
                                   0,
                                   tex.textureType == TEXTURE_CUBE_MAP
                                       ? static_cast<GLint>(target.arrayLayer * 6 + target.face)
                                       : static_cast<GLint>(target.arrayLayer),
                                   tex.desc.size.x,
                                   tex.desc.size.y,
                                   1,
                                   format,
                                   type,
                                   clearData);
            }

            oglCheckError();

            oglDebugEndGroup();
        }

        void generateMipMaps(const Resource<Texture> &texture) override {
            oglDebugStartGroup("TransferContextGL::generateMipMaps");

            const auto &tex = resources.getTexture(texture);

            glBindTexture(tex.textureType, tex.handle);
            glGenerateMipmap(tex.textureType);
            glBindTexture(tex.textureType, 0);

            oglCheckError();

            oglDebugEndGroup();
        }

    private:
        PassResources resources;
        Statistics &stats;
    };
}

#endif //XENGINE_TRANSFERCONTEXTGL_HPP
