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

#ifndef XENGINE_TRANSFERCONTEXTGL_HPP
#define XENGINE_TRANSFERCONTEXTGL_HPP

#include "xng/rendergraph/context/transfercontext.hpp"

#include "heapgl.hpp"
#include "passresources.hpp"
#include "resource/framebuffer.hpp"
#include "resource/shaderprogram.hpp"

namespace xng::opengl {
    class TransferContextGL final : public rg::TransferContext {
    public:
        static void getFormatAndDataType(const ColorFormat bufferFormat, GLenum &dataType, GLenum &pixelFormat) {
            // determine pixel format (GL_RGB/GL_RGBA/GL_RED/GL_RG) and data type for upload/copy
            dataType = GL_UNSIGNED_BYTE;
            pixelFormat = GL_RGBA;

            switch (bufferFormat) {
                case R8:
                case R8I:
                case R8UI:
                case R16:
                case R16I:
                case R16UI:
                case R32F:
                case R32I:
                case R32UI:
                    pixelFormat = GL_RED;
                    break;
                case RG8:
                case RG8I:
                case RG8UI:
                case RG16:
                case RG16I:
                case RG16UI:
                case RG32F:
                case RG32I:
                case RG32UI:
                    pixelFormat = GL_RG;
                    break;
                case RGB8:
                case SRGB8:
                case RGB8I:
                case RGB8UI:
                case RGB16:
                case RGB16I:
                case RGB16UI:
                case RGB32F:
                case RGB32I:
                case RGB32UI:
                    pixelFormat = GL_RGB;
                    break;
                case RGBA8:
                case SRGB8_ALPHA8:
                case RGBA8I:
                case RGBA8UI:
                case RGBA16:
                case RGBA16I:
                case RGBA16UI:
                case RGBA32F:
                case RGBA32I:
                case RGBA32UI:
                    pixelFormat = GL_RGBA;
                    break;
                case DEPTH_16:
                case DEPTH_32F:
                    pixelFormat = GL_DEPTH_COMPONENT;
                    break;
                case STENCIL_8:
                    pixelFormat = GL_STENCIL_INDEX;
                    break;
                default:
                    throw std::runtime_error("Unsupported buffer format");
            }

            switch (bufferFormat) {
                case R8I:
                case RG8I:
                case RGB8I:
                case RGBA8I:
                    dataType = GL_BYTE;
                    break;
                case R8:
                case R8UI:
                case RG8:
                case RG8UI:
                case RGB8:
                case SRGB8:
                case SRGB8_ALPHA8:
                case RGB8UI:
                case RGBA8:
                case RGBA8UI:
                case STENCIL_8:
                    dataType = GL_UNSIGNED_BYTE;
                    break;
                case R16:
                case R16UI:
                case RG16:
                case RG16UI:
                case RGB16:
                case RGB16UI:
                case RGBA16:
                case RGBA16UI:
                case DEPTH_16:
                    dataType = GL_UNSIGNED_SHORT;
                    break;
                case R16I:
                case RG16I:
                case RGB16I:
                case RGBA16I:
                    dataType = GL_SHORT;
                    break;
                case R32F:
                case RG32F:
                case RGB32F:
                case RGBA32F:
                case DEPTH_32F:
                    dataType = GL_FLOAT;
                    break;
                case R32I:
                case RG32I:
                case RGB32I:
                case RGBA32I:
                    dataType = GL_INT;
                    break;
                case R32UI:
                case RG32UI:
                case RGB32UI:
                case RGBA32UI:
                    dataType = GL_UNSIGNED_INT;
                    break;
                default:
                    throw std::runtime_error("Unsupported buffer format");
            }
        }

        explicit TransferContextGL(const PassResources &res)
            : TransferContextGL() {
            resources = res;
        }

        TransferContextGL() {
            constexpr auto flipShaderSource = R"(#version 430 core
            layout(local_size_x = 64, local_size_y = 1) in;

            layout(std430, binding = 0) readonly  buffer Src { uint srcData[]; };
            layout(std430, binding = 1) writeonly buffer Dst { uint dstData[]; };

            uniform uint uRowUints;   // rowSize / 4
            uniform uint uHeight;     // number of rows
            uniform uint uSrcBase;    // bufferOffset / 4

            void main() {
                uint col = gl_GlobalInvocationID.x;
                uint row = gl_GlobalInvocationID.y;
                if (col >= uRowUints || row >= uHeight) return;

                uint srcRow = uHeight - 1u - row;
                dstData[row * uRowUints + col] = srcData[uSrcBase + srcRow * uRowUints + col];
            })";
            flipShader = ShaderProgram();
            flipShader.buildShader(flipShaderSource);

            flipRowUintsLoc = glGetUniformLocation(flipShader.programHandle, "uRowUints");
            flipHeightLoc = glGetUniformLocation(flipShader.programHandle, "uHeight");
            flipSrcBaseLoc = glGetUniformLocation(flipShader.programHandle, "uSrcBase");

            oglCheckError();
        }

        ~TransferContextGL() override = default;

        void setResources(const PassResources &res) {
            resources = res;
        }

        void copyBuffer(const Resource<Buffer> &target,
                        const Resource<Buffer> &source,
                        const size_t targetOffset,
                        const size_t sourceOffset,
                        const size_t count) override {
            if (!target.isAssigned() || !source.isAssigned()) {
                throw std::runtime_error("Unassigned buffer resource");
            }

            if (target.getDescription().size < targetOffset + count ||
                source.getDescription().size < sourceOffset + count) {
                throw std::runtime_error("Invalid buffer offset");
            }

            oglDebugStartGroup("TransferContextGL::copyBuffer");

            const auto readBuffer = resources.getBuffer(source).handle;
            const auto writeBuffer = resources.getBuffer(target).handle;

            glBindBuffer(GL_COPY_READ_BUFFER, readBuffer);
            oglCheckError();

            glBindBuffer(GL_COPY_WRITE_BUFFER, writeBuffer);
            oglCheckError();

            glCopyBufferSubData(GL_COPY_READ_BUFFER,
                                GL_COPY_WRITE_BUFFER,
                                static_cast<GLintptr>(sourceOffset),
                                static_cast<GLintptr>(targetOffset),
                                static_cast<GLsizeiptr>(count));
            oglCheckError();

            glBindBuffer(GL_COPY_READ_BUFFER, 0);
            glBindBuffer(GL_COPY_WRITE_BUFFER, 0);

            oglCheckError();

            oglDebugEndGroup();
        }

        void copyTexture(const Resource<Texture> &target,
                         const Resource<Texture> &source,
                         const std::vector<TextureCopyRegion> &regions) override {
            if (!target.isAssigned() || !source.isAssigned()) {
                throw std::runtime_error("Unassigned texture resource");
            }

            oglDebugStartGroup("TransferContextGL::copyTexture");

            const auto srcTexture = resources.getTexture(source);
            const auto dstTexture = resources.getTexture(target);

            for (auto &region: regions) {
                const Vec2u srcMipSize = source.getDescription().getMipLevelSize(region.src.mipLevel);
                const Vec2u dstMipSize = target.getDescription().getMipLevelSize(region.dst.mipLevel);

                const auto srcRect = getCorrectedTextureRect(Rectu(region.srcOffset, region.size),
                                                             srcMipSize);
                const auto dstRect = getCorrectedTextureRect(Rectu(region.dstOffset, region.size),
                                                             dstMipSize);

                glCopyImageSubData(srcTexture.handle,
                                   srcTexture.textureType,
                                   static_cast<GLint>(region.src.mipLevel),
                                   static_cast<GLint>(srcRect.position.x),
                                   static_cast<GLint>(srcRect.position.y),
                                   static_cast<GLint>(region.src.arrayLayer),
                                   dstTexture.handle,
                                   dstTexture.textureType,
                                   static_cast<GLint>(region.dst.mipLevel),
                                   static_cast<GLint>(dstRect.position.x),
                                   static_cast<GLint>(dstRect.position.y),
                                   static_cast<GLint>(region.dst.arrayLayer),
                                   static_cast<GLint>(region.size.x),
                                   static_cast<GLint>(region.size.y),
                                   static_cast<GLsizei>(region.depth));
            }

            oglCheckError();

            oglDebugEndGroup();
        }

        void copyBufferToTexture(const Resource<Texture> &texture,
                                 const Resource<Buffer> &buffer,
                                 const Texture::SubResource textureSubResource,
                                 const size_t bufferOffset,
                                 const Rectu &textureOffset,
                                 const ColorFormat bufferFormat) override {
            if (!texture.isAssigned() || !buffer.isAssigned()) {
                throw std::runtime_error("Unassigned resource");
            }

            oglDebugStartGroup("TransferContextGL::copyBufferToTexture");

            const auto &tex = resources.getTexture(texture);
            const auto &buf = resources.getBuffer(buffer);

            const auto pixelSize = getColorByteSize(bufferFormat);

            const BufferGL unpackCopy(Buffer(textureOffset.dimensions.x * textureOffset.dimensions.y * pixelSize,
                                             Buffer::CAPABILITY_TRANSFER_SRC,
                                             Buffer::MEMORY_GPU_ONLY));

            const auto rowSize = textureOffset.dimensions.x * pixelSize;

            flipRowsGL(buf.handle, unpackCopy.handle, rowSize, textureOffset.dimensions.y, bufferOffset);

            GLenum pixelFormat;
            GLenum dataType;
            getFormatAndDataType(bufferFormat, dataType, pixelFormat);

            const auto rect = getCorrectedTextureRect(textureOffset,
                                                      tex.desc.getMipLevelSize(textureSubResource.mipLevel));

            glBindBuffer(GL_PIXEL_UNPACK_BUFFER, unpackCopy.handle);

            glBindTexture(tex.textureType, tex.handle);

            if (tex.textureType == GL_TEXTURE_2D) {
                glTexSubImage2D(GL_TEXTURE_2D,
                                static_cast<GLint>(textureSubResource.mipLevel),
                                rect.position.x,
                                rect.position.y,
                                rect.dimensions.x,
                                rect.dimensions.y,
                                pixelFormat,
                                dataType,
                                nullptr);
            } else if (tex.textureType == GL_TEXTURE_CUBE_MAP) {
                glTexSubImage2D(convert(textureSubResource.face),
                                static_cast<GLint>(textureSubResource.mipLevel),
                                rect.position.x,
                                rect.position.y,
                                rect.dimensions.x,
                                rect.dimensions.y,
                                pixelFormat,
                                dataType,
                                nullptr);
            } else if (tex.textureType == GL_TEXTURE_2D_ARRAY) {
                glTexSubImage3D(tex.textureType,
                                static_cast<GLint>(textureSubResource.mipLevel),
                                rect.position.x,
                                rect.position.y,
                                static_cast<GLint>(textureSubResource.arrayLayer),
                                rect.dimensions.x,
                                rect.dimensions.y,
                                1,
                                pixelFormat,
                                dataType,
                                nullptr);
            } else if (tex.textureType == GL_TEXTURE_CUBE_MAP_ARRAY) {
                glTexSubImage3D(tex.textureType,
                                static_cast<GLint>(textureSubResource.mipLevel),
                                rect.position.x,
                                rect.position.y,
                                static_cast<GLint>(textureSubResource.arrayLayer) * 6 + textureSubResource.face,
                                rect.dimensions.x,
                                rect.dimensions.y,
                                1,
                                pixelFormat,
                                dataType,
                                nullptr);
            } else {
                throw std::runtime_error("Invalid texture type");
            }

            glBindTexture(tex.textureType, 0);

            glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

            oglCheckError();

            oglDebugEndGroup();
        }

        void copyTextureToBuffer(const Resource<Buffer> &buffer,
                                 const Resource<Texture> &texture,
                                 const Texture::SubResource textureSubResource,
                                 const size_t bufferOffset,
                                 const Rectu &textureOffset,
                                 const ColorFormat bufferFormat) override {
            if (!texture.isAssigned() || !buffer.isAssigned()) {
                throw std::runtime_error("Unassigned resource");
            }

            oglDebugStartGroup("TransferContextGL::copyTextureToBuffer");

            const auto &tex = resources.getTexture(texture);
            const auto &buf = resources.getBuffer(buffer);

            const auto pixelSize = getColorByteSize(bufferFormat);
            const auto rowSize = textureOffset.dimensions.x * pixelSize;
            const auto dataSize = textureOffset.dimensions.x * textureOffset.dimensions.y * pixelSize;

            GLenum pixelFormat;
            GLenum dataType;
            getFormatAndDataType(bufferFormat, dataType, pixelFormat);

            // Temporary pack PBO
            const BufferGL packCopy(Buffer(dataSize,
                                           Buffer::CAPABILITY_TRANSFER_DST,
                                           Buffer::MEMORY_GPU_ONLY));

            glBindBuffer(GL_PIXEL_PACK_BUFFER, packCopy.handle);

            GLint zOffset = 0;
            GLint depth = 1;
            if (tex.textureType == GL_TEXTURE_2D) {
                zOffset = 0;
                depth = 1;
            } else if (tex.textureType == GL_TEXTURE_CUBE_MAP) {
                zOffset = textureSubResource.face;
                depth = 1;
            } else if (tex.textureType == GL_TEXTURE_2D_ARRAY) {
                zOffset = static_cast<GLint>(textureSubResource.arrayLayer);
                depth = 1;
            } else if (tex.textureType == GL_TEXTURE_CUBE_MAP_ARRAY) {
                zOffset = static_cast<GLint>(textureSubResource.arrayLayer) * 6 + textureSubResource.face;
                depth = 1;
            } else {
                throw std::runtime_error("Invalid texture type");
            }

            const auto rect = getCorrectedTextureRect(textureOffset,
                                                      tex.desc.getMipLevelSize(textureSubResource.mipLevel));

            glGetTextureSubImage(tex.handle,
                                 static_cast<GLint>(textureSubResource.mipLevel),
                                 rect.position.x,
                                 rect.position.y,
                                 zOffset,
                                 rect.dimensions.x,
                                 rect.dimensions.y,
                                 depth,
                                 pixelFormat,
                                 dataType,
                                 static_cast<GLsizei>(dataSize),
                                 nullptr);

            glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

            flipRowsGL(packCopy.handle, buf.handle, rowSize, textureOffset.dimensions.y, bufferOffset);

            oglCheckError();

            oglDebugEndGroup();
        }

        void clearTexture(const Resource<Texture> &texture,
                          const Texture::SubResource &target,
                          const Texture::ClearValue &clearVal) override {
            if (!texture.isAssigned()) {
                throw std::runtime_error("Unassigned resource");
            }

            const auto &tex = resources.getTexture(texture);
            clearTexture(tex, target, clearVal);
        }

        void blitTexture(const Resource<Texture> &src,
                         const Resource<Texture> &dst,
                         const Texture::SubResource &srcTarget,
                         const Texture::SubResource &dstTarget,
                         const Rectu &srcRect,
                         const Rectu &dstRect,
                         const TextureFiltering &filtering) override {
            if (!src.isAssigned() || !dst.isAssigned()) {
                throw std::runtime_error("Unassigned resource");
            }

            if (src == dst && srcTarget == dstTarget) {
                return;
            }

            if (src.getDescription().format != dst.getDescription().format) {
                throw std::runtime_error("Source and target textures must have the same format");
            }

            GLbitfield mask = 0;
            GLenum bindingPoint;
            GLenum filter = convert(filtering);
            if (src.getDescription().format == DEPTH_16 || src.getDescription().format == DEPTH_32F) {
                bindingPoint = GL_DEPTH_ATTACHMENT;
                mask = GL_DEPTH_BUFFER_BIT;
                filter = GL_NEAREST;
            } else if (src.getDescription().format == DEPTH24_STENCIL8 || src.getDescription().format ==
                       DEPTH32F_STENCIL8) {
                bindingPoint = GL_DEPTH_STENCIL_ATTACHMENT;
                mask = GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT;
                filter = GL_NEAREST;
            } else if (src.getDescription().format == STENCIL_8) {
                bindingPoint = GL_STENCIL_ATTACHMENT;
                mask = GL_STENCIL_BUFFER_BIT;
                filter = GL_NEAREST;
            } else {
                bindingPoint = GL_COLOR_ATTACHMENT0;
                mask = GL_COLOR_BUFFER_BIT;
            }

            const auto &srcTex = resources.getTexture(src);
            const auto &dstTex = resources.getTexture(dst);

            blitSrcFb.bind(GL_READ_FRAMEBUFFER);
            blitDstFb.bind(GL_DRAW_FRAMEBUFFER);

            blitSrcFb.attach(bindingPoint, srcTex, srcTarget);
            blitDstFb.attach(bindingPoint, dstTex, dstTarget);

            const auto correctedSourceRect = getCorrectedTextureRect(srcRect,
                                                                     srcTex.desc.getMipLevelSize(srcTarget.mipLevel));
            const auto correctedTargetRect = getCorrectedTextureRect(dstRect,
                                                                     dstTex.desc.getMipLevelSize(dstTarget.mipLevel));

            glBlitFramebuffer(correctedSourceRect.position.x,
                              correctedSourceRect.position.y,
                              correctedSourceRect.position.x + correctedSourceRect.dimensions.x,
                              correctedSourceRect.position.y + correctedSourceRect.dimensions.y,
                              correctedTargetRect.position.x,
                              correctedTargetRect.position.y,
                              correctedTargetRect.position.x + correctedTargetRect.dimensions.x,
                              correctedTargetRect.position.y + correctedTargetRect.dimensions.y,
                              mask,
                              filter);
        }

        void generateMipMaps(const Resource<Texture> &texture) override {
            if (!texture.isAssigned()) {
                throw std::runtime_error("Unassigned resource");
            }

            oglDebugStartGroup("TransferContextGL::generateMipMaps");

            const auto &tex = resources.getTexture(texture);

            glBindTexture(tex.textureType, tex.handle);
            glGenerateMipmap(tex.textureType);
            glBindTexture(tex.textureType, 0);

            oglCheckError();

            oglDebugEndGroup();
        }

        static void clearTexture(const TextureGL &tex,
                                 const Texture::SubResource &target,
                                 const Texture::ClearValue &clearVal) {
            oglDebugStartGroup("TransferContextGL::clearTexture");

            const auto mipSize = tex.desc.getMipLevelSize(target.mipLevel);

            if (tex.desc.format == DEPTH24_STENCIL8 || tex.desc.format == DEPTH32F_STENCIL8) {
                //Workaround using the raster pipeline to clear because there doesn't seem to be a standard 24bit_8bit depth stencil format.
                const auto clearValue = std::get<Texture::DepthStencilClearValue>(clearVal);
                GLint previousDrawFb = 0;
                glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &previousDrawFb);
                Framebuffer clearFb;
                clearFb.bind(GL_DRAW_FRAMEBUFFER);
                {
                    clearFb.attach(GL_DEPTH_STENCIL_ATTACHMENT, tex, target);
                    glClearDepth(clearValue.clearDepth);
                    glClearStencil(static_cast<GLint>(clearValue.clearStencil));
                    glDepthMask(GL_TRUE);
                    glStencilMask(0xff);
                    glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
                }
                clearFb.unbind();
                glBindFramebuffer(GL_DRAW_FRAMEBUFFER, static_cast<GLuint>(previousDrawFb));
            } else if (tex.desc.format == DEPTH_16 || tex.desc.format == DEPTH_32F) {
                const GLfloat depthData = std::get<float>(clearVal);
                glClearTexSubImage(tex.handle,
                                   static_cast<GLint>(target.mipLevel),
                                   0,
                                   0,
                                   tex.textureType == TEXTURE_CUBE_MAP
                                       ? static_cast<GLint>(target.arrayLayer * 6 + target.face)
                                       : static_cast<GLint>(target.arrayLayer),
                                   mipSize.x,
                                   mipSize.y,
                                   1,
                                   GL_DEPTH_COMPONENT,
                                   GL_FLOAT,
                                   &depthData);
            } else if (tex.desc.format == STENCIL_8) {
                const GLuint stencilData = std::get<unsigned>(clearVal);
                glClearTexSubImage(tex.handle,
                                   static_cast<GLint>(target.mipLevel),
                                   0,
                                   0,
                                   tex.textureType == TEXTURE_CUBE_MAP
                                       ? static_cast<GLint>(target.arrayLayer * 6 + target.face)
                                       : static_cast<GLint>(target.arrayLayer),
                                   mipSize.x,
                                   mipSize.y,
                                   1,
                                   GL_STENCIL_INDEX,
                                   GL_UNSIGNED_INT,
                                   &stencilData);
            } else {
                const void *clearData = nullptr;
                GLenum format = GL_RGBA;
                GLenum type = GL_UNSIGNED_BYTE;

                GLubyte bClearData[4];
                GLfloat fClearData[4];
                GLint iClearData[4];
                GLuint uiClearData[4];

                switch (clearVal.index()) {
                    case 0: {
                        format = GL_RGBA;
                        type = GL_UNSIGNED_BYTE;
                        const auto &vec = std::get<Vector4<uint8_t> >(clearVal);
                        bClearData[0] = vec.x;
                        bClearData[1] = vec.y;
                        bClearData[2] = vec.z;
                        bClearData[3] = vec.w;
                        clearData = bClearData;
                        break;
                    }
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
                        break;
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

                GLint zOffset = 0;
                if (tex.desc.textureType == TEXTURE_CUBE_MAP_ARRAY) {
                    zOffset = static_cast<GLint>(target.arrayLayer * 6 + target.face);
                } else if (tex.desc.textureType == TEXTURE_CUBE_MAP) {
                    zOffset = target.face;
                } else if (tex.desc.textureType == TEXTURE_2D_ARRAY) {
                    zOffset = static_cast<GLint>(target.arrayLayer);
                }
                glClearTexSubImage(tex.handle,
                                   static_cast<GLint>(target.mipLevel),
                                   0,
                                   0,
                                   zOffset,
                                   mipSize.x,
                                   mipSize.y,
                                   1,
                                   format,
                                   type,
                                   clearData);
            }

            oglCheckError();

            oglDebugEndGroup();
        }

    private:
        static Recti getCorrectedTextureRect(const Rectu &region, const Vec2u &textureSize) {
            auto ret = region.convert<int>();
            ret.position.y = textureSize.y - ret.position.y - ret.dimensions.y;
            return ret;
        }

        void flipRowsGL(const GLuint srcBuffer,
                        const GLuint dstBuffer,
                        const size_t rowSizeBytes,
                        const size_t height,
                        const size_t srcOffsetBytes) const {
            assert(rowSizeBytes % 4 == 0 && srcOffsetBytes % 4 == 0);

            const auto rowUints = static_cast<GLuint>(rowSizeBytes / 4);
            const auto h = static_cast<GLuint>(height);
            const auto srcBase = static_cast<GLuint>(srcOffsetBytes / 4);

            glUseProgram(flipShader.programHandle);
            glUniform1ui(flipRowUintsLoc, rowUints);
            glUniform1ui(flipHeightLoc, h);
            glUniform1ui(flipSrcBaseLoc, srcBase);

            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, srcBuffer);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, dstBuffer);

            glDispatchCompute((rowUints + 63u) / 64u, h, 1);

            // dst is consumed next as the unpack PBO
            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_PIXEL_BUFFER_BARRIER_BIT);

            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, 0);
            glUseProgram(0);

            oglCheckError();
        }

        PassResources resources;

        Framebuffer blitSrcFb;
        Framebuffer blitDstFb;

        ShaderProgram flipShader;
        GLint flipRowUintsLoc;
        GLint flipHeightLoc;
        GLint flipSrcBaseLoc;
    };
}

#endif //XENGINE_TRANSFERCONTEXTGL_HPP
