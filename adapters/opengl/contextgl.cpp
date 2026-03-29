/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 3 of the License, or (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.

 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <cstring>

#include "contextgl.hpp"

#include "colorbytesize.hpp"
#include "resource/oglframebuffer.hpp"

void ContextGL::uploadVertexBuffer(const Resource<VertexBuffer> &target,
                                   const uint8_t *buffer,
                                   const size_t bufferSize,
                                   const size_t targetOffset) {
    oglDebugStartGroup("ContextGL::uploadVertexBuffer");

    const auto &buf = getVertexBuffer(target);
    glBindBuffer(GL_ARRAY_BUFFER, buf.VBO);
    glBufferSubData(GL_ARRAY_BUFFER,
                    static_cast<GLintptr>(targetOffset),
                    static_cast<GLsizeiptr>(bufferSize),
                    buffer);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    oglCheckError();
    stats.bufferVRamUpload += bufferSize;

    oglDebugEndGroup();
}

void ContextGL::uploadIndexBuffer(const Resource<IndexBuffer> &target, const uint8_t *buffer, size_t bufferSize,
                                  size_t targetOffset) {
    oglDebugStartGroup("ContextGL::uploadIndexBuffer");

    const auto buf = getIndexBuffer(target);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf.EBO);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,
                    static_cast<GLintptr>(targetOffset),
                    static_cast<GLsizeiptr>(bufferSize),
                    buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    oglCheckError();
    stats.indexVRamUpload += bufferSize;

    oglDebugEndGroup();
}

void ContextGL::uploadStorageBuffer(const Resource<StorageBuffer> &target, const uint8_t *buffer, size_t bufferSize,
                                    size_t targetOffset) {
    oglDebugStartGroup("ContextGL::uploadStorageBuffer");

    const auto buf = getStorageBuffer(target);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buf.SSBO);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER,
                    static_cast<GLintptr>(targetOffset),
                    static_cast<GLsizeiptr>(bufferSize),
                    buffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    oglCheckError();
    stats.storageBufferVRamUpload += bufferSize;

    oglDebugEndGroup();
}

void ContextGL::uploadTextureBuffer(const Resource<Texture> &texture, const uint8_t *buffer, size_t bufferSize,
                                    ColorFormat bufferFormat, size_t index, CubeMapFace face, size_t mipMapLevel,
                                    const Vec2i &size, const Vec2i &offset) {
    oglDebugStartGroup("ContextGL::uploadTexture");

    auto &tex = getTextureBuffer(texture);

    if (bufferSize > tex.texture.size.x * tex.texture.size.y * getColorByteSize(tex.texture.format)) {
        throw std::runtime_error("Invalid buffer size");
    }

    // Invert the rows because opengl requires bottom-up texture data
    uint8_t *bufferCpy = new uint8_t[bufferSize];
    for (auto row = 0; row < size.y; ++row) {
        auto dstOffset = row * size.x * getColorByteSize(bufferFormat);
        auto srcOffset = (size.y - row - 1) * size.x * getColorByteSize(bufferFormat);
        memcpy(bufferCpy + dstOffset, buffer + srcOffset, size.x * getColorByteSize(bufferFormat));
    }

    auto &textureSize = tex.texture.size;

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
                        static_cast<GLint>(mipMapLevel),
                        offset.x,
                        textureSize.y - offset.y - 1 - size.y + 1,
                        size.x,
                        size.y,
                        pixelFormat,
                        dataType,
                        bufferCpy);
    } else if (tex.textureType == GL_TEXTURE_CUBE_MAP) {
        glTexSubImage2D(convert(face),
                        static_cast<GLint>(mipMapLevel),
                        offset.x,
                        textureSize.y - offset.y - 1 - size.y + 1,
                        size.x,
                        size.y,
                        pixelFormat,
                        dataType,
                        bufferCpy);
    } else if (tex.textureType == GL_TEXTURE_2D_ARRAY) {
        glTexSubImage3D(tex.textureType,
                        static_cast<GLint>(mipMapLevel),
                        offset.x,
                        textureSize.y - offset.y - 1 - size.y + 1,
                        static_cast<GLint>(index),
                        size.x,
                        size.y,
                        1,
                        pixelFormat,
                        dataType,
                        bufferCpy);
    } else if (tex.textureType == GL_TEXTURE_CUBE_MAP_ARRAY) {
        glTexSubImage3D(tex.textureType,
                        static_cast<GLint>(mipMapLevel),
                        offset.x,
                        textureSize.y - offset.y - 1 - size.y + 1,
                        static_cast<GLint>(index) * 6 + face,
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

void ContextGL::generateMipMaps(const Resource<Texture> &texture) {
    oglDebugStartGroup("ContextGL::generateMipMaps");

    auto &tex = getTextureBuffer(texture);
    glBindTexture(tex.textureType, tex.handle);
    glGenerateMipmap(tex.textureType);
    glBindTexture(tex.textureType, 0);

    oglCheckError();

    oglDebugEndGroup();
}

void ContextGL::clearTextureColor(const Resource<Texture> &texture,
                                  const ColorRGBA &clearColor,
                                  const size_t index,
                                  const CubeMapFace face,
                                  const size_t mipMapLevel) {
    oglDebugStartGroup("ContextGL::clearTextureColor");

    auto &tex = getTextureBuffer(texture);

    const GLsizei mipWidth = std::max(1, tex.texture.size.x >> static_cast<int>(mipMapLevel));
    const GLsizei mipHeight = std::max(1, tex.texture.size.y >> static_cast<int>(mipMapLevel));

    glClearTexSubImage(tex.handle,
                       static_cast<GLint>(mipMapLevel),
                       0,
                       0,
                       tex.textureType == TEXTURE_CUBE_MAP
                           ? static_cast<GLint>(index * 6 + face)
                           : static_cast<GLint>(index),
                       mipWidth,
                       mipHeight,
                       1,
                       GL_RGBA,
                       GL_UNSIGNED_BYTE,
                       clearColor.data);

    oglCheckError();

    oglDebugEndGroup();
}

void ContextGL::clearTextureColor(const Resource<Texture> &texture,
                                  const Vec4f &clearColor,
                                  const size_t index,
                                  const CubeMapFace face,
                                  const size_t mipMapLevel) {
    oglDebugStartGroup("ContextGL::clearTextureColor");

    auto &tex = getTextureBuffer(texture);

    const GLfloat data[4] = {clearColor.x, clearColor.y, clearColor.z, clearColor.w};

    const GLsizei mipWidth = std::max(1, tex.texture.size.x >> static_cast<int>(mipMapLevel));
    const GLsizei mipHeight = std::max(1, tex.texture.size.y >> static_cast<int>(mipMapLevel));

    glClearTexSubImage(tex.handle,
                       static_cast<GLint>(mipMapLevel),
                       0,
                       0,
                       tex.textureType == TEXTURE_CUBE_MAP
                           ? static_cast<GLint>(index * 6 + face)
                           : static_cast<GLint>(index),
                       mipWidth,
                       mipHeight,
                       1,
                       GL_RGBA,
                       GL_FLOAT,
                       data);

    oglCheckError();

    oglDebugEndGroup();
}

void ContextGL::clearTextureColor(const Resource<Texture> &texture,
                                  const Vec4i &clearColor,
                                  const size_t index,
                                  const CubeMapFace face,
                                  const size_t mipMapLevel) {
    oglDebugStartGroup("ContextGL::clearTextureColor");

    auto &tex = getTextureBuffer(texture);

    const GLint data[4] = {clearColor.x, clearColor.y, clearColor.z, clearColor.w};

    const GLsizei mipWidth = std::max(1, tex.texture.size.x >> static_cast<int>(mipMapLevel));
    const GLsizei mipHeight = std::max(1, tex.texture.size.y >> static_cast<int>(mipMapLevel));

    glClearTexSubImage(tex.handle,
                       static_cast<GLint>(mipMapLevel),
                       0,
                       0,
                       tex.textureType == TEXTURE_CUBE_MAP
                           ? static_cast<GLint>(index * 6 + face)
                           : static_cast<GLint>(index),
                       mipWidth,
                       mipHeight,
                       1,
                        GL_RGBA_INTEGER,
                       GL_INT,
                       data);

    oglCheckError();

    oglDebugEndGroup();
}

void ContextGL::clearTextureColor(const Resource<Texture> &texture,
                                  const Vec4u &clearColor,
                                  const size_t index,
                                  const CubeMapFace face,
                                  const size_t mipMapLevel) {
    oglDebugStartGroup("ContextGL::clearTextureColor");

    auto &tex = getTextureBuffer(texture);

    const GLuint data[4] = {clearColor.x, clearColor.y, clearColor.z, clearColor.w};

    const GLsizei mipWidth = std::max(1, tex.texture.size.x >> static_cast<int>(mipMapLevel));
    const GLsizei mipHeight = std::max(1, tex.texture.size.y >> static_cast<int>(mipMapLevel));

    glClearTexSubImage(tex.handle,
                       static_cast<GLint>(mipMapLevel),
                       0,
                       0,
                       tex.textureType == TEXTURE_CUBE_MAP
                           ? static_cast<GLint>(index * 6 + face)
                           : static_cast<GLint>(index),
                       mipWidth,
                       mipHeight,
                       1,
                       GL_RGBA_INTEGER,
                       GL_UNSIGNED_INT,
                       data);

    oglCheckError();

    oglDebugEndGroup();
}

void ContextGL::clearTextureDepthStencil(const Resource<Texture> &texture,
                                         const float clearDepth,
                                         const unsigned int clearStencil,
                                         const size_t index,
                                         const CubeMapFace face,
                                         const size_t mipMapLevel) {
    oglDebugStartGroup("ContextGL::clearTextureDepthStencil");

    auto &tex = getTextureBuffer(texture);

    if (tex.texture.format == DEPTH_STENCIL
        || tex.texture.format == DEPTH24_STENCIL8) {
        //Workaround using the pipeline to clear because there doesn't seem to be a standard 24bit_8bit depth stencil format.
        auto clearFb = std::make_unique<OGLFramebuffer>();
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, clearFb->FBO);
        clearFb->attach(tex, GL_DEPTH_STENCIL_ATTACHMENT, 0);
        glClearDepth(clearDepth);
        glClearStencil(static_cast<GLint>(clearStencil));
        glDepthMask(GL_TRUE);
        glStencilMask(0xff);
        glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        if (framebuffer) {
            if (boundPipeline.isAssigned()) {
                if (boundPipeline.getData().depthTestWrite) {
                    glDepthMask(GL_TRUE);
                } else {
                    glDepthMask(GL_FALSE);
                }
                glStencilMask(boundPipeline.getData().stencilTestMask);
            }
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer->FBO);
        }
    } else if (tex.texture.format == DEPTH
               || tex.texture.format == DEPTH_32F
               || tex.texture.format == DEPTH_24
               || tex.texture.format == DEPTH_16) {
        const GLfloat depthData = clearDepth;
        glClearTexSubImage(tex.handle,
                           static_cast<GLint>(mipMapLevel),
                           0,
                           0,
                           tex.textureType == TEXTURE_CUBE_MAP
                               ? static_cast<GLint>(index * 6 + face)
                               : static_cast<GLint>(index),
                           tex.texture.size.x,
                           tex.texture.size.y,
                           1,
                           GL_DEPTH_COMPONENT,
                           GL_FLOAT,
                           &depthData);
    } else if (tex.texture.format == STENCIL
               || tex.texture.format == STENCIL_32
               || tex.texture.format == STENCIL_16
               || tex.texture.format == STENCIL_8) {
        const GLuint stencilData = clearStencil;
        glClearTexSubImage(tex.handle,
                           static_cast<GLint>(mipMapLevel),
                           0,
                           0,
                           tex.textureType == TEXTURE_CUBE_MAP
                               ? static_cast<GLint>(index * 6 + face)
                               : static_cast<GLint>(index),
                           tex.texture.size.x,
                           tex.texture.size.y,
                           1,
                           GL_STENCIL_INDEX,
                           GL_UNSIGNED_INT,
                           &stencilData);
    }

    oglCheckError();

    oglDebugEndGroup();
}

void ContextGL::copyVertexBuffer(Resource<VertexBuffer> target, Resource<VertexBuffer> source, size_t targetOffset,
                                 size_t sourceOffset, size_t count) {
    oglDebugStartGroup("ContextGL::copyVertexBuffer");

    GLuint readBuffer = 0;
    GLuint writeBuffer = 0;
    readBuffer = resources.vertexBuffers.at(source)->VBO;
    writeBuffer = resources.vertexBuffers.at(target)->VBO;
    stats.bufferVRamCopy += count;

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

    oglDebugEndGroup();
}

void ContextGL::copyIndexBuffer(Resource<IndexBuffer> target, Resource<IndexBuffer> source, size_t targetOffset,
                                size_t sourceOffset, size_t count) {
    oglDebugStartGroup("ContextGL::copyIndexBuffer");

    GLuint readBuffer = 0;
    GLuint writeBuffer = 0;
    readBuffer = resources.indexBuffers.at(source)->EBO;
    writeBuffer = resources.indexBuffers.at(target)->EBO;
    stats.indexVRamCopy += count;

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

    oglDebugEndGroup();
}

void ContextGL::copyStorageBuffer(Resource<StorageBuffer> target, Resource<StorageBuffer> source, size_t targetOffset,
                                  size_t sourceOffset, size_t count) {
    oglDebugStartGroup("ContextGL::copyStorageBuffer");

    GLuint readBuffer = 0;
    GLuint writeBuffer = 0;
    readBuffer = resources.storageBuffers.at(source)->SSBO;
    writeBuffer = resources.storageBuffers.at(target)->SSBO;
    stats.storageBufferVRamCopy += count;

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

    oglDebugEndGroup();
}

void ContextGL::copyTextureBuffer(Resource<Texture> target, Resource<Texture> source) {
    oglDebugStartGroup("ContextGL::copyTexture");

    auto srcTexture = getTextureBuffer(source);
    auto dstTexture = getTextureBuffer(target);
    glCopyImageSubData(srcTexture.handle,
                       srcTexture.textureType,
                       0,
                       0,
                       0,
                       0,
                       dstTexture.handle,
                       dstTexture.textureType,
                       0,
                       0,
                       0,
                       0,
                       srcTexture.texture.size.x,
                       srcTexture.texture.size.y,
                       1);
    oglCheckError();

    stats.textureVRamCopy += srcTexture.texture.size.x * srcTexture.texture.size.y
            * getColorByteSize(srcTexture.texture.format);

    oglDebugEndGroup();
}

void ContextGL::copyTextureBuffer(Resource<Texture> target,
                                  Resource<Texture> source,
                                  Vec3i srcOffset,
                                  Vec3i dstOffset,
                                  Vec3i size,
                                  size_t srcMipMapLevel,
                                  size_t dstMipMapLevel) {
    oglDebugStartGroup("ContextGL::copyTexture");

    auto srcTexture = getTextureBuffer(source);
    auto dstTexture = getTextureBuffer(target);
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
                       srcTexture.texture.size.x,
                       srcTexture.texture.size.y,
                       1);
    oglCheckError();

    stats.textureVRamCopy += srcTexture.texture.size.x * srcTexture.texture.size.y
            * getColorByteSize(srcTexture.texture.format);

    oglDebugEndGroup();
}

void ContextGL::beginRenderPass(const std::vector<Attachment> &colorAttachments,
                                const Attachment &depthAttachment,
                                const Attachment &stencilAttachment) {
    oglDebugStartGroup("ContextGL::beginRenderPass");

    framebufferColorAttachments = colorAttachments;
    framebufferDepthAttachment = depthAttachment;
    framebufferStencilAttachment = stencilAttachment;

    framebuffer = std::make_shared<OGLFramebuffer>();

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer->FBO);

    for (auto i = 0; i < colorAttachments.size(); ++i) {
        auto &attachment = colorAttachments.at(i);
        auto &tex = getTextureBuffer(attachment.texture);
        if (attachment.clearAttachment) {
            if (tex.texture.format >= R8I && tex.texture.format <= RGBA32I) {
                clearColorAttachment(i, attachment.clearColorInt);
            } else if (tex.texture.format >= R8UI && tex.texture.format <= RGBA32UI) {
                clearColorAttachment(i, attachment.clearColorUint);
            } else if (tex.texture.format >= R8 && tex.texture.format <= RGBA32F) {
                clearColorAttachment(i, attachment.clearColorFloat);
            } else {
                clearColorAttachment(i, attachment.clearColor);
            }
        }
        framebuffer->attach(attachment, tex, GL_COLOR_ATTACHMENT0 + i);
    }

    if (depthAttachment.texture.isAssigned()) {
        auto &attachment = depthAttachment;
        auto &tex = getTextureBuffer(attachment.texture);
        if (attachment.clearAttachment) {
            clearDepthAttachment(attachment.clearDepth);
        }
        framebuffer->attach(attachment, tex, GL_DEPTH_ATTACHMENT);
    }

    if (stencilAttachment.texture.isAssigned()) {
        auto &attachment = stencilAttachment;
        auto &tex = getTextureBuffer(attachment.texture);
        if (attachment.clearAttachment) {
            clearStencilAttachment(attachment.clearStencil);
        }
        framebuffer->attach(attachment, tex, GL_STENCIL_ATTACHMENT);
    }

    glBindVertexArray(vertexArray->VAO);

    auto fstatus = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
    if (fstatus != GL_FRAMEBUFFER_COMPLETE) {
        const char *msg = "UNKNOWN";
        switch (fstatus) {
            case GL_FRAMEBUFFER_UNDEFINED: msg = "UNDEFINED";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: msg = "INCOMPLETE_ATTACHMENT";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: msg = "MISSING_ATTACHMENT";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: msg = "INCOMPLETE_DRAW_BUFFER";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: msg = "INCOMPLETE_READ_BUFFER";
                break;
            case GL_FRAMEBUFFER_UNSUPPORTED: msg = "UNSUPPORTED";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: msg = "INCOMPLETE_MULTISAMPLE";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS: msg = "INCOMPLETE_LAYER_TARGETS";
                break;
        }
        throw std::runtime_error(std::string("Framebuffer is incomplete ") + msg);
    }
    oglCheckError();

    oglDebugEndGroup();
}

void ContextGL::beginRenderPass(const std::vector<Attachment> &colorAttachments,
                                const Attachment &depthStencilAttachment) {
    oglDebugStartGroup("ContextGL::beginRenderPass");

    framebufferColorAttachments = colorAttachments;
    framebufferDepthStencilAttachment = depthStencilAttachment;

    framebuffer = std::make_shared<OGLFramebuffer>();

    std::vector<GLenum> drawBuffers;

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer->FBO);

    for (auto i = 0; i < colorAttachments.size(); ++i) {
        auto &attachment = colorAttachments.at(i);
        auto &tex = getTextureBuffer(attachment.texture);
        if (attachment.clearAttachment) {
            if (tex.texture.format >= R8I && tex.texture.format <= RGBA32I) {
                clearColorAttachment(i, attachment.clearColorInt);
            } else if (tex.texture.format >= R8UI && tex.texture.format <= RGBA32UI) {
                clearColorAttachment(i, attachment.clearColorUint);
            } else if (tex.texture.format >= R8 && tex.texture.format <= RGBA32F) {
                clearColorAttachment(i, attachment.clearColorFloat);
            } else {
                clearColorAttachment(i, attachment.clearColor);
            }
        }
        framebuffer->attach(attachment, tex, GL_COLOR_ATTACHMENT0 + i);
        drawBuffers.emplace_back(GL_COLOR_ATTACHMENT0 + i);
    }

    glDrawBuffers(static_cast<GLsizei>(drawBuffers.size()), drawBuffers.data());

    if (depthStencilAttachment.texture.isAssigned()) {
        auto &attachment = depthStencilAttachment;
        auto &tex = getTextureBuffer(attachment.texture);
        if (tex.texture.format == DEPTH
            || tex.texture.format >= DEPTH_32F && tex.texture.format <= DEPTH_16) {
            if (attachment.clearAttachment) {
                clearDepthAttachment(attachment.clearDepth);
            }
            framebuffer->attach(depthStencilAttachment, tex, GL_DEPTH_ATTACHMENT);
        } else if (tex.texture.format == STENCIL
                   || tex.texture.format >= STENCIL_32 && tex.texture.format <= STENCIL_8) {
            if (attachment.clearAttachment) {
                clearStencilAttachment(attachment.clearStencil);
            }
            framebuffer->attach(depthStencilAttachment, tex, GL_STENCIL_ATTACHMENT);
        } else if (tex.texture.format == DEPTH_STENCIL || tex.texture.format == DEPTH24_STENCIL8) {
            if (attachment.clearAttachment) {
                clearDepthStencilAttachment(attachment.clearDepth, attachment.clearStencil);
            }
            framebuffer->attach(depthStencilAttachment, tex, GL_DEPTH_STENCIL_ATTACHMENT);
        } else {
            throw std::runtime_error("Invalid depth/stencil attachment");
        }
    }

    glBindVertexArray(vertexArray->VAO);

    auto fstatus = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
    if (fstatus != GL_FRAMEBUFFER_COMPLETE) {
        const char *msg = "UNKNOWN";
        switch (fstatus) {
            case GL_FRAMEBUFFER_UNDEFINED: msg = "UNDEFINED";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: msg = "INCOMPLETE_ATTACHMENT";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: msg = "MISSING_ATTACHMENT";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: msg = "INCOMPLETE_DRAW_BUFFER";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: msg = "INCOMPLETE_READ_BUFFER";
                break;
            case GL_FRAMEBUFFER_UNSUPPORTED: msg = "UNSUPPORTED";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: msg = "INCOMPLETE_MULTISAMPLE";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS: msg = "INCOMPLETE_LAYER_TARGETS";
                break;
        }
        throw std::runtime_error(std::string("Framebuffer is incomplete ") + msg);
    }

    oglCheckError();

    oglDebugEndGroup();
}

void ContextGL::bindPipeline(const Resource<RasterPipeline> &pipeline) {
    oglDebugStartGroup("ContextGL::bindPipeline");

    // Bind shader program
    const auto shaderProgram = getShaderProgram(pipeline);
    glUseProgram(shaderProgram.programHandle);

    if (pipeline.getData().multisample)
        glEnable(GL_MULTISAMPLE);
    else
        glDisable(GL_MULTISAMPLE);

    if (pipeline.getData().multiSampleEnableFrequency)
        glEnable(GL_SAMPLE_COVERAGE);
    else
        glDisable(GL_SAMPLE_COVERAGE);

    glSampleCoverage(pipeline.getData().multiSampleFrequency, GL_TRUE);

    // Setup pipeline state
    glDepthFunc(convert(pipeline.getData().depthTestMode));
    if (pipeline.getData().depthTestWrite)
        glDepthMask(GL_TRUE);
    else
        glDepthMask(GL_FALSE);

    if (pipeline.getData().enableDepthTest) {
        glEnable(GL_DEPTH_TEST);
    } else {
        glDisable(GL_DEPTH_TEST);
    }

    glStencilMask(pipeline.getData().stencilTestMask);
    glStencilFunc(convert(pipeline.getData().stencilMode),
                  pipeline.getData().stencilReference,
                  pipeline.getData().stencilFunctionMask);
    glStencilOp(convert(pipeline.getData().stencilFail),
                convert(pipeline.getData().stencilDepthFail),
                convert(pipeline.getData().stencilPass));

    if (pipeline.getData().enableStencilTest) {
        glEnable(GL_STENCIL_TEST);
    } else {
        glDisable(GL_STENCIL_TEST);
    }

    glCullFace(convert(pipeline.getData().faceCullMode));
    if (pipeline.getData().faceCullWinding == RasterPipeline::CLOCKWISE)
        glFrontFace(GL_CW);
    else
        glFrontFace(GL_CCW);

    if (pipeline.getData().enableFaceCulling) {
        glEnable(GL_CULL_FACE);
    } else {
        glDisable(GL_CULL_FACE);
    }

    glBlendFuncSeparate(convert(pipeline.getData().colorBlendSourceMode),
                        convert(pipeline.getData().colorBlendDestinationMode),
                        convert(pipeline.getData().alphaBlendSourceMode),
                        convert(pipeline.getData().alphaBlendDestinationMode));
    glBlendEquationSeparate(convert(pipeline.getData().colorBlendEquation),
                            convert(pipeline.getData().alphaBlendEquation));

    if (pipeline.getData().enableBlending) {
        glEnable(GL_BLEND);
    } else {
        glDisable(GL_BLEND);
    }

    // Enable seamless cubemap filtering so texel lookups blend across face boundaries
    // For the render graph abstraction this is assumed as default on.
    // On Vulkan / DirectX this should be on without additional configuration.
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    oglCheckError();

    boundPipeline = pipeline;

    oglDebugEndGroup();
}

void ContextGL::bindVertexBuffer(const Resource<VertexBuffer> &buffer) {
    if (!boundPipeline.isAssigned()) {
        throw std::runtime_error("No current pipeline set");
    }

    oglDebugStartGroup("ContextGL::bindVertexBuffer");

    glBindBuffer(GL_ARRAY_BUFFER, resources.vertexBuffers.at(buffer)->VBO);

    // Setup vertex layout
    auto &vertexLayout = getVertexLayout(boundPipeline);

    const auto vertexStride = static_cast<GLsizei>(vertexLayout.getLayoutSize());

    auto &attributes = vertexLayout.getElements();
    size_t currentOffset = 0;
    for (int i = 0; i < attributes.size(); i++) {
        auto &binding = attributes.at(i);
        glEnableVertexAttribArray(i);
        if (binding.component > ShaderPrimitiveType::SIGNED_INT) {
            glVertexAttribPointer(i,
                                  ShaderPrimitiveType::getCount(binding.type),
                                  getType(binding.component),
                                  GL_FALSE,
                                  vertexStride,
                                  reinterpret_cast<void *>(currentOffset));
        } else {
            glVertexAttribIPointer(i,
                                   ShaderPrimitiveType::getCount(binding.type),
                                   getType(binding.component),
                                   vertexStride,
                                   reinterpret_cast<void *>(currentOffset));
        }
        currentOffset += binding.stride();
    }

    glBindVertexArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(vertexArray->VAO);

    oglCheckError();

    oglDebugEndGroup();
}

void ContextGL::bindIndexBuffer(const Resource<IndexBuffer> &buffer) {
    if (!boundPipeline.isAssigned()) {
        throw std::runtime_error("No current pipeline set");
    }

    oglDebugStartGroup("ContextGL::bindIndexBuffer");

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, resources.indexBuffers.at(buffer)->EBO);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(vertexArray->VAO);
    oglCheckError();

    oglDebugEndGroup();
}

void ContextGL::bindTexture(const std::string &target,
                                  const std::vector<Resource<Texture> > &textureArray) {
    if (!boundPipeline.isAssigned()) {
        throw std::runtime_error("No current pipeline");
    }

    oglDebugStartGroup("ContextGL::bindTexture");

    const auto &pipeline = getCompiledShader(boundPipeline);

    const auto binding = pipeline.getTextureArrayBinding(target);
    for (auto i = 0; i < textureArray.size(); i++) {
        auto &texture = resources.textureBuffers.at(textureArray.at(i));
        glActiveTexture(getTextureSlot(binding + i));
        glBindTexture(texture->textureType, texture->handle);
    }

    oglCheckError();

    oglDebugEndGroup();
}

void ContextGL::bindStorageBuffer(const std::string &target,
                                  const Resource<StorageBuffer> &buffer,
                                  const size_t offset,
                                  const size_t size) {
    if (!boundPipeline.isAssigned()) {
        throw std::runtime_error("No current pipeline");
    }

    oglDebugStartGroup("ContextGL::bindShaderBuffer");

    const auto &pipeline = getCompiledShader(boundPipeline);
    const auto binding = pipeline.getShaderBufferBinding(target);
    const auto &buf = getStorageBuffer(buffer);
    if (size == 0) {
        if (buf.size == 0) {
            // Bind SSBO with a size of one byte to avoid undefined behavior
            // caused by binding a SSBO with size = 0 per OpenGL spec.
            // On AMD this was handled by the driver but Intel driver breaks if SSBO is 0
            glBindBufferRange(GL_SHADER_STORAGE_BUFFER, static_cast<GLuint>(binding), emptySSBO->SSBO, 0, 1);
        } else {
            glBindBufferRange(GL_SHADER_STORAGE_BUFFER,
                              static_cast<GLuint>(binding),
                              buf.SSBO,
                              static_cast<GLintptr>(offset),
                              static_cast<GLsizeiptr>(buf.size - offset));
        }
    } else {
        glBindBufferRange(GL_SHADER_STORAGE_BUFFER,
                          static_cast<GLuint>(binding),
                          buf.SSBO,
                          static_cast<GLintptr>(offset),
                          static_cast<GLsizeiptr>(size));
    }

    oglCheckError();

    oglDebugEndGroup();
}

void ContextGL::setShaderParameter(const std::string &name, const ShaderPrimitive &value) {
    throw std::runtime_error("Not implemented");
}

void ContextGL::clearColorAttachment(const size_t binding, const ColorRGBA clearColor) {
    oglDebugStartGroup("ContextGL::clearColorAttachment");

    const auto &attachment = framebufferColorAttachments.at(binding);
    auto &tex = getTextureBuffer(attachment.texture);

    if ((tex.texture.textureType == TEXTURE_CUBE_MAP
         || tex.texture.textureType == TEXTURE_2D_ARRAY
         || tex.texture.textureType == TEXTURE_CUBE_MAP_ARRAY
         || tex.texture.textureType == TEXTURE_2D_MULTISAMPLE_ARRAY)
        && attachment.layered) {
        if (tex.texture.textureType == TEXTURE_CUBE_MAP_ARRAY) {
            for (auto i = 0; i < tex.texture.arrayLayers; i++) {
                for (auto face = 0; face < 6; face++) {
                    clearTextureColor(attachment.texture,
                                      clearColor,
                                      (i * 6) + face,
                                      {},
                                      attachment.mipMapLevel);
                }
            }
        } else {
            for (auto i = 0; i < tex.texture.arrayLayers; i++) {
                clearTextureColor(attachment.texture,
                                  clearColor,
                                  i,
                                  {},
                                  attachment.mipMapLevel);
            }
        }
    } else {
        clearTextureColor(attachment.texture,
                          clearColor,
                          attachment.index,
                          attachment.face,
                          attachment.mipMapLevel);
    }

    oglCheckError();

    oglDebugEndGroup();
}

void ContextGL::clearColorAttachment(const size_t binding, const Vec4i &clearColor) {
    oglDebugStartGroup("ContextGL::clearColorAttachment");

    const auto &attachment = framebufferColorAttachments.at(binding);
    auto &tex = getTextureBuffer(attachment.texture);

    if ((tex.texture.textureType == TEXTURE_CUBE_MAP
         || tex.texture.textureType == TEXTURE_2D_ARRAY
         || tex.texture.textureType == TEXTURE_CUBE_MAP_ARRAY
         || tex.texture.textureType == TEXTURE_2D_MULTISAMPLE_ARRAY)
        && attachment.layered) {
        if (tex.texture.textureType == TEXTURE_CUBE_MAP_ARRAY) {
            for (auto i = 0; i < tex.texture.arrayLayers; i++) {
                for (auto face = 0; face < 6; face++) {
                    clearTextureColor(attachment.texture,
                                      clearColor,
                                      (i * 6) + face,
                                      {},
                                      attachment.mipMapLevel);
                }
            }
        } else {
            for (auto i = 0; i < tex.texture.arrayLayers; i++) {
                clearTextureColor(attachment.texture,
                                  clearColor,
                                  i,
                                  {},
                                  attachment.mipMapLevel);
            }
        }
    } else {
        clearTextureColor(attachment.texture,
                          clearColor,
                          attachment.index,
                          attachment.face,
                          attachment.mipMapLevel);
    }

    oglDebugEndGroup();
}

void ContextGL::clearColorAttachment(const size_t binding, const Vec4u &clearColor) {
    oglDebugStartGroup("ContextGL::clearColorAttachment");

    const auto &attachment = framebufferColorAttachments.at(binding);
    auto &tex = getTextureBuffer(attachment.texture);

    if ((tex.texture.textureType == TEXTURE_CUBE_MAP
         || tex.texture.textureType == TEXTURE_2D_ARRAY
         || tex.texture.textureType == TEXTURE_CUBE_MAP_ARRAY
         || tex.texture.textureType == TEXTURE_2D_MULTISAMPLE_ARRAY)
        && attachment.layered) {
        if (tex.texture.textureType == TEXTURE_CUBE_MAP_ARRAY) {
            for (auto i = 0; i < tex.texture.arrayLayers; i++) {
                for (auto face = 0; face < 6; face++) {
                    clearTextureColor(attachment.texture,
                                      clearColor,
                                      (i * 6) + face,
                                      {},
                                      attachment.mipMapLevel);
                }
            }
        } else {
            for (auto i = 0; i < tex.texture.arrayLayers; i++) {
                clearTextureColor(attachment.texture,
                                  clearColor,
                                  i,
                                  {},
                                  attachment.mipMapLevel);
            }
        }
    } else {
        clearTextureColor(attachment.texture,
                          clearColor,
                          attachment.index,
                          attachment.face,
                          attachment.mipMapLevel);
    }

    oglDebugEndGroup();
}

void ContextGL::clearColorAttachment(size_t binding, const Vec4f &clearColor) {
    oglDebugStartGroup("ContextGL::clearColorAttachment");

    const auto &attachment = framebufferColorAttachments.at(binding);
    auto &tex = getTextureBuffer(attachment.texture);

    if ((tex.texture.textureType == TEXTURE_CUBE_MAP
         || tex.texture.textureType == TEXTURE_2D_ARRAY
         || tex.texture.textureType == TEXTURE_CUBE_MAP_ARRAY
         || tex.texture.textureType == TEXTURE_2D_MULTISAMPLE_ARRAY)
        && attachment.layered) {
        if (tex.texture.textureType == TEXTURE_CUBE_MAP_ARRAY) {
            for (auto i = 0; i < tex.texture.arrayLayers; i++) {
                for (auto face = 0; face < 6; face++) {
                    clearTextureColor(attachment.texture,
                                      clearColor,
                                      (i * 6) + face,
                                      {},
                                      attachment.mipMapLevel);
                }
            }
        } else {
            for (auto i = 0; i < tex.texture.arrayLayers; i++) {
                clearTextureColor(attachment.texture,
                                  clearColor,
                                  i,
                                  {},
                                  attachment.mipMapLevel);
            }
        }
    } else {
        clearTextureColor(attachment.texture,
                          clearColor,
                          attachment.index,
                          attachment.face,
                          attachment.mipMapLevel);
    }

    oglDebugEndGroup();
}

void ContextGL::clearDepthStencilAttachment(const float clearDepth, const unsigned int clearStencil) {
    oglDebugStartGroup("ContextGL::clearDepthStencilAttachment");

    const auto &attachment = framebufferDepthStencilAttachment;
    auto &tex = getTextureBuffer(attachment.texture);

    if ((tex.texture.textureType == TEXTURE_CUBE_MAP
         || tex.texture.textureType == TEXTURE_2D_ARRAY
         || tex.texture.textureType == TEXTURE_CUBE_MAP_ARRAY
         || tex.texture.textureType == TEXTURE_2D_MULTISAMPLE_ARRAY)
        && attachment.layered) {
        if (tex.texture.textureType == TEXTURE_CUBE_MAP_ARRAY) {
            for (auto i = 0; i < tex.texture.arrayLayers; i++) {
                for (auto face = 0; face < 6; face++) {
                    clearTextureDepthStencil(attachment.texture,
                                             clearDepth,
                                             clearStencil,
                                             (i * 6) + face,
                                             {},
                                             attachment.mipMapLevel);
                }
            }
        } else {
            for (auto i = 0; i < tex.texture.arrayLayers; i++) {
                clearTextureDepthStencil(attachment.texture,
                                         clearDepth,
                                         clearStencil,
                                         i,
                                         {},
                                         attachment.mipMapLevel);
            }
        }
    } else {
        clearTextureDepthStencil(attachment.texture,
                                 clearDepth,
                                 clearStencil,
                                 attachment.index,
                                 attachment.face,
                                 attachment.mipMapLevel);
    }

    oglDebugEndGroup();
}

void ContextGL::clearDepthAttachment(const float clearDepth) {
    oglDebugStartGroup("ContextGL::clearDepthAttachment");

    const auto &attachment = framebufferDepthStencilAttachment.texture.isAssigned()
                                 ? framebufferDepthStencilAttachment
                                 : framebufferDepthAttachment;
    auto &tex = getTextureBuffer(attachment.texture);

    if ((tex.texture.textureType == TEXTURE_CUBE_MAP
         || tex.texture.textureType == TEXTURE_2D_ARRAY
         || tex.texture.textureType == TEXTURE_CUBE_MAP_ARRAY
         || tex.texture.textureType == TEXTURE_2D_MULTISAMPLE_ARRAY)
        && attachment.layered) {
        if (tex.texture.textureType == TEXTURE_CUBE_MAP_ARRAY) {
            for (auto i = 0; i < tex.texture.arrayLayers; i++) {
                for (auto face = 0; face < 6; face++) {
                    clearTextureDepthStencil(attachment.texture,
                                             clearDepth,
                                             0,
                                             (i * 6) + face,
                                             {},
                                             attachment.mipMapLevel);
                }
            }
        } else {
            for (auto i = 0; i < tex.texture.arrayLayers; i++) {
                clearTextureDepthStencil(attachment.texture,
                                         clearDepth,
                                         0,
                                         i,
                                         {},
                                         attachment.mipMapLevel);
            }
        }
    } else {
        clearTextureDepthStencil(attachment.texture,
                                 clearDepth,
                                 0,
                                 attachment.index,
                                 attachment.face,
                                 attachment.mipMapLevel);
    }

    oglDebugEndGroup();
}

void ContextGL::clearStencilAttachment(const unsigned int clearStencil) {
    oglDebugStartGroup("ContextGL::clearStencilAttachment");

    const auto &attachment = framebufferDepthStencilAttachment.texture.isAssigned()
                                 ? framebufferDepthStencilAttachment
                                 : framebufferStencilAttachment;
    auto &tex = getTextureBuffer(attachment.texture);

    if ((tex.texture.textureType == TEXTURE_CUBE_MAP
         || tex.texture.textureType == TEXTURE_2D_ARRAY
         || tex.texture.textureType == TEXTURE_CUBE_MAP_ARRAY
         || tex.texture.textureType == TEXTURE_2D_MULTISAMPLE_ARRAY)
        && attachment.layered) {
        if (tex.texture.textureType == TEXTURE_CUBE_MAP_ARRAY) {
            for (auto i = 0; i < tex.texture.arrayLayers; i++) {
                for (auto face = 0; face < 6; face++) {
                    clearTextureDepthStencil(attachment.texture,
                                             1,
                                             clearStencil,
                                             (i * 6) + face,
                                             {},
                                             attachment.mipMapLevel);
                }
            }
        } else {
            for (auto i = 0; i < tex.texture.arrayLayers; i++) {
                clearTextureDepthStencil(attachment.texture,
                                         1,
                                         clearStencil,
                                         i,
                                         {},
                                         attachment.mipMapLevel);
            }
        }
    } else {
        clearTextureDepthStencil(attachment.texture,
                                 1,
                                 clearStencil,
                                 attachment.index,
                                 attachment.face,
                                 attachment.mipMapLevel);
    }

    oglDebugEndGroup();
}

void ContextGL::setViewport(const Vec2i viewportOffset, const Vec2i viewportSize) {
    oglDebugStartGroup("ContextGL::setViewport");

    glViewport(viewportOffset.x, viewportOffset.y, viewportSize.x, viewportSize.y);
    oglCheckError();

    oglDebugEndGroup();
}

void ContextGL::drawArray(const DrawCall &drawCall) {
    oglDebugStartGroup("ContextGL::drawArray");

    glDrawArrays(convert(boundPipeline.getData().primitive),
                 static_cast<GLint>(drawCall.offset),
                 static_cast<GLsizei>(drawCall.count));
    oglCheckError();

    stats.drawCalls++;
    stats.polygons += drawCall.count / boundPipeline.getData().primitive;

    oglDebugEndGroup();
}

void ContextGL::drawIndexed(const DrawCall &drawCall, const size_t indexOffset) {
    oglDebugStartGroup("ContextGL::drawIndexed");

    glDrawElementsBaseVertex(convert(boundPipeline.getData().primitive),
                             static_cast<GLsizei>(drawCall.count),
                             convert(drawCall.indexFormat),
                             reinterpret_cast<void *>(drawCall.offset),
                             static_cast<GLint>(indexOffset));
    oglCheckError();

    stats.drawCalls++;
    stats.polygons += drawCall.count / boundPipeline.getData().primitive;

    oglDebugEndGroup();
}

void ContextGL::endRenderPass() {
    oglDebugStartGroup("ContextGL::endRenderPass");

    glBindVertexArray(0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    oglCheckError();

    boundPipeline = {};
    framebufferColorAttachments = {};
    framebufferDepthStencilAttachment = {};
    framebufferDepthAttachment = {};
    framebufferStencilAttachment = {};
    framebuffer = nullptr;

    oglDebugEndGroup();
}

std::vector<uint8_t> ContextGL::downloadStorageBuffer(const Resource<StorageBuffer> &buffer) {
    throw std::runtime_error("downloadStorageBuffer not implemented");
}

Image<ColorRGBA> ContextGL::downloadTextureBuffer(const Resource<Texture> &texture,
                                                  const size_t index, size_t mipMapLevel,
                                                  const CubeMapFace face) {
    throw std::runtime_error("downloadTextureBuffer not implemented");
}

std::unordered_map<Shader::Stage, std::string> ContextGL::getShaderSource(const Resource<RasterPipeline> &pipeline) {
    return getCompiledShader(pipeline).sourceCode;
}

const OGLVertexBuffer &ContextGL::getVertexBuffer(const ResourceId &resource) const {
    switch (resource.getScope()) {
        case ResourceId::TRANSIENT:
            return *resources.vertexBuffers.at(resource);
        case ResourceId::HEAP:
            return *heap.vertexBuffers.at(resource);
        case ResourceId::RUNTIME:
        default:
            throw std::runtime_error("Invalid vertex buffer handle scope");
    }
}

const OGLIndexBuffer &ContextGL::getIndexBuffer(const ResourceId &resource) const {
    switch (resource.getScope()) {
        case ResourceId::TRANSIENT:
            return *resources.indexBuffers.at(resource);
        case ResourceId::HEAP:
            return *heap.indexBuffers.at(resource);
        case ResourceId::RUNTIME:
        default:
            throw std::runtime_error("Invalid index buffer handle scope");
    }
}

const OGLShaderStorageBuffer &ContextGL::getStorageBuffer(const ResourceId &resource) const {
    switch (resource.getScope()) {
        case ResourceId::TRANSIENT:
            return *resources.storageBuffers.at(resource);
        case ResourceId::HEAP:
            return *heap.storageBuffers.at(resource);
        case ResourceId::RUNTIME:
        default:
            throw std::runtime_error("Invalid storage buffer handle scope");
    }
}

const OGLTexture &ContextGL::getTextureBuffer(const ResourceId &resource) const {
    switch (resource.getScope()) {
        case ResourceId::TRANSIENT:
            return *resources.textureBuffers.at(resource);
        case ResourceId::HEAP:
            return *heap.textureBuffers.at(resource);
        case ResourceId::RUNTIME:
            if (resource.getHandle() == 0) {
                return *backBufferColor;
            }
            if (resource.getHandle() == 1) {
                return *backBufferDepthStencil;
            }
            throw std::runtime_error("Invalid texture buffer handle");
        default:
            throw std::runtime_error("Invalid texture handle scope");
    }
}

const OGLShaderProgram &ContextGL::getShaderProgram(const ResourceId &resource) const {
    switch (resource.getScope()) {
        case ResourceId::HEAP:
            return *heap.shaderPrograms.at(resource);
        default:
            throw std::runtime_error("Invalid pipeline handle scope");
    }
}

const ShaderAttributeLayout &ContextGL::getVertexLayout(const ResourceId &resource) const {
    switch (resource.getScope()) {
        case ResourceId::HEAP:
            return heap.vertexLayouts.at(resource);
        default:
            throw std::runtime_error("Invalid pipeline handle scope");
    }
}

const CompiledShader &ContextGL::getCompiledShader(const ResourceId &resource) const {
    switch (resource.getScope()) {
        case ResourceId::HEAP:
            return heap.compiledShaders.at(resource);
        default:
            throw std::runtime_error("Invalid pipeline handle scope");
    }
}
