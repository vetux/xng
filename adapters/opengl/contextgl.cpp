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

#include "contextgl.hpp"

#include "colorbytesize.hpp"
#include "ogl/oglframebuffer.hpp"

#include "xng/rendergraph/shaderscript/shaderscript.hpp"

void ContextGL::uploadBuffer(const RenderGraphResource target,
                             const uint8_t *buffer,
                             const size_t bufferSize,
                             const size_t targetOffset) {
    oglDebugStartGroup("ContextGL::uploadBuffer");
    if (resources.vertexBuffers.find(target) != resources.vertexBuffers.end()) {
        const auto buf = resources.vertexBuffers.at(target);
        glBindBuffer(GL_ARRAY_BUFFER, buf->VBO);
        glBufferSubData(GL_ARRAY_BUFFER,
                        static_cast<GLintptr>(targetOffset),
                        static_cast<GLsizeiptr>(bufferSize),
                        buffer);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        oglCheckError();
        stats.vertexVRamUpload += bufferSize;
    } else if (resources.indexBuffers.find(target) != resources.indexBuffers.end()) {
        const auto buf = resources.indexBuffers.at(target);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf->EBO);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,
                        static_cast<GLintptr>(targetOffset),
                        static_cast<GLsizeiptr>(bufferSize),
                        buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        oglCheckError();
        stats.indexVRamUpload += bufferSize;
    } else if (resources.storageBuffers.find(target) != resources.storageBuffers.end()) {
        const auto buf = resources.storageBuffers.at(target);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, buf->SSBO);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER,
                        static_cast<GLintptr>(targetOffset),
                        static_cast<GLsizeiptr>(bufferSize),
                        buffer);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        oglCheckError();
        stats.shaderBufferVRamUpload += bufferSize;
    }
    oglDebugEndGroup();
}

void ContextGL::uploadTexture(const RenderGraphResource texture,
                              const uint8_t *buffer,
                              const size_t bufferSize,
                              const ColorFormat bufferFormat,
                              const size_t index,
                              const CubeMapFace face,
                              const size_t mipMapLevel) {
    oglDebugStartGroup("ContextGL::uploadTexture");

    auto &tex = resources.textures.at(texture);

    glBindTexture(tex->textureType, tex->handle);
    if (tex->textureType == GL_TEXTURE_2D) {
        glTexSubImage2D(GL_TEXTURE_2D,
                        static_cast<GLint>(mipMapLevel),
                        0,
                        0,
                        tex->texture.size.x,
                        tex->texture.size.y,
                        convert(bufferFormat),
                        GL_UNSIGNED_BYTE,
                        buffer);
    } else if (tex->textureType == GL_TEXTURE_CUBE_MAP) {
        glTexSubImage2D(convert(face),
                        static_cast<GLint>(mipMapLevel),
                        0,
                        0,
                        tex->texture.size.x,
                        tex->texture.size.y,
                        convert(bufferFormat),
                        GL_UNSIGNED_BYTE,
                        buffer);
    } else if (tex->textureType == GL_TEXTURE_2D_ARRAY) {
        glTexSubImage3D(tex->textureType,
                        static_cast<GLint>(mipMapLevel),
                        0,
                        0,
                        static_cast<GLint>(index),
                        tex->texture.size.x,
                        tex->texture.size.y,
                        1,
                        convert(bufferFormat),
                        GL_UNSIGNED_BYTE,
                        buffer);
    } else if (tex->textureType == GL_TEXTURE_CUBE_MAP_ARRAY) {
        glTexSubImage3D(tex->textureType,
                        static_cast<GLint>(mipMapLevel),
                        0,
                        0,
                        static_cast<GLint>(index) * 6 + face,
                        tex->texture.size.x,
                        tex->texture.size.y,
                        1,
                        convert(bufferFormat),
                        GL_UNSIGNED_BYTE,
                        buffer);
    } else {
        throw std::runtime_error("Invalid texture type");
    }
    glBindTexture(tex->textureType, 0);
    oglCheckError();

    stats.textureVRamUpload += bufferSize;

    oglDebugEndGroup();
}

void ContextGL::clearTextureColor(const RenderGraphResource texture,
                                  const ColorRGBA &clearColor,
                                  const size_t index,
                                  const CubeMapFace face,
                                  const size_t mipMapLevel) {
    oglDebugStartGroup("ContextGL::clearTextureColor");

    auto &tex = getTexture(texture);

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
                       GL_RGBA,
                       GL_UNSIGNED_BYTE,
                       clearColor.data);

    oglCheckError();

    oglDebugEndGroup();
}

void ContextGL::clearTextureColor(const RenderGraphResource texture,
                                  const Vec4f &clearColor,
                                  const size_t index,
                                  const CubeMapFace face,
                                  const size_t mipMapLevel) {
    oglDebugStartGroup("ContextGL::clearTextureColor");

    auto &tex = getTexture(texture);

    const GLfloat data[4] = {clearColor.x, clearColor.y, clearColor.z, clearColor.w};

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
                       GL_RGBA,
                       GL_FLOAT,
                       data);

    oglCheckError();

    oglDebugEndGroup();
}

void ContextGL::clearTextureColor(const RenderGraphResource texture,
                                  const Vec4i &clearColor,
                                  const size_t index,
                                  const CubeMapFace face,
                                  const size_t mipMapLevel) {
    oglDebugStartGroup("ContextGL::clearTextureColor");

    auto &tex = getTexture(texture);

    const GLint data[4] = {clearColor.x, clearColor.y, clearColor.z, clearColor.w};

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
                       GL_RGBA_INTEGER,
                       GL_INT,
                       data);

    oglCheckError();

    oglDebugEndGroup();
}

void ContextGL::clearTextureColor(const RenderGraphResource texture,
                                  const Vec4u &clearColor,
                                  const size_t index,
                                  const CubeMapFace face,
                                  const size_t mipMapLevel) {
    oglDebugStartGroup("ContextGL::clearTextureColor");

    auto &tex = getTexture(texture);

    const GLuint data[4] = {clearColor.x, clearColor.y, clearColor.z, clearColor.w};

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
                       GL_RGBA_INTEGER,
                       GL_UNSIGNED_INT,
                       data);

    oglCheckError();

    oglDebugEndGroup();
}

void ContextGL::clearTextureDepthStencil(const RenderGraphResource texture,
                                         const float clearDepth,
                                         const unsigned int clearStencil,
                                         const size_t index,
                                         const CubeMapFace face,
                                         const size_t mipMapLevel) {
    oglDebugStartGroup("ContextGL::clearTextureDepthStencil");

    auto &tex = getTexture(texture);

    if (tex.texture.format == DEPTH_STENCIL
        || tex.texture.format == DEPTH24_STENCIL8) {
        // Convert depth to 24-bit integer
        const GLuint depth24 = static_cast<GLuint>(clearDepth * 0xFFFFFF);

        if (clearStencil > 255) {
            throw std::runtime_error("Invalid stencil value for clearing DEPTH_STENCIL texture");
        }

        // Pack into 32-bit value: depth in top 24 bits, stencil in bottom 8 bits
        const GLuint packed_value = (depth24 << 8) | clearStencil;

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
                           GL_DEPTH_STENCIL,
                           GL_UNSIGNED_INT_24_8,
                           &packed_value);
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

void ContextGL::copyBuffer(const RenderGraphResource target,
                           const RenderGraphResource source,
                           const size_t targetOffset,
                           const size_t sourceOffset,
                           const size_t count) {
    oglDebugStartGroup("ContextGL::copyBuffer");

    GLuint readBuffer = 0;
    GLuint writeBuffer = 0;
    if (resources.vertexBuffers.find(target) != resources.vertexBuffers.end()) {
        readBuffer = resources.vertexBuffers.at(source)->VBO;
        writeBuffer = resources.vertexBuffers.at(target)->VBO;
        stats.vertexVRamCopy += count;
    } else if (resources.indexBuffers.find(target) != resources.indexBuffers.end()) {
        readBuffer = resources.indexBuffers.at(source)->EBO;
        writeBuffer = resources.indexBuffers.at(target)->EBO;
        stats.indexVRamCopy += count;
    } else if (resources.storageBuffers.find(target) != resources.storageBuffers.end()) {
        readBuffer = resources.storageBuffers.at(source)->SSBO;
        writeBuffer = resources.storageBuffers.at(target)->SSBO;
        stats.shaderBufferVRamCopy += count;
    } else {
        throw std::runtime_error("Invalid buffer");
    }

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

void ContextGL::copyTexture(const RenderGraphResource target, const RenderGraphResource source) {
    oglDebugStartGroup("ContextGL::copyTexture");

    auto srcTexture = resources.textures.at(source);
    auto dstTexture = resources.textures.at(target);
    glCopyImageSubData(srcTexture->handle,
                       srcTexture->textureType,
                       0,
                       0,
                       0,
                       0,
                       dstTexture->handle,
                       dstTexture->textureType,
                       0,
                       0,
                       0,
                       0,
                       srcTexture->texture.size.x,
                       srcTexture->texture.size.y,
                       1);
    oglCheckError();

    stats.textureVRamCopy += srcTexture->texture.size.x * srcTexture->texture.size.y
            * getColorByteSize(srcTexture->texture.format);

    oglDebugEndGroup();
}

void ContextGL::copyTexture(const RenderGraphResource target,
                            const RenderGraphResource source,
                            const Vec3i srcOffset,
                            const Vec3i dstOffset,
                            const Vec3i size,
                            const size_t srcMipMapLevel,
                            const size_t dstMipMapLevel) {
    oglDebugStartGroup("ContextGL::copyTexture");

    auto srcTexture = resources.textures.at(source);
    auto dstTexture = resources.textures.at(target);
    glCopyImageSubData(srcTexture->handle,
                       srcTexture->textureType,
                       static_cast<GLint>(srcMipMapLevel),
                       srcOffset.x,
                       srcOffset.y,
                       srcOffset.z,
                       dstTexture->handle,
                       dstTexture->textureType,
                       static_cast<GLint>(dstMipMapLevel),
                       dstOffset.x,
                       dstOffset.y,
                       dstOffset.z,
                       srcTexture->texture.size.x,
                       srcTexture->texture.size.y,
                       1);
    oglCheckError();

    stats.textureVRamCopy += srcTexture->texture.size.x * srcTexture->texture.size.y
            * getColorByteSize(srcTexture->texture.format);

    oglDebugEndGroup();
}

void ContextGL::beginRenderPass(const std::vector<RenderGraphAttachment> &colorAttachments,
                                const RenderGraphAttachment &depthAttachment,
                                const RenderGraphAttachment &stencilAttachment) {
    oglDebugStartGroup("ContextGL::beginRenderPass");

    framebufferColorAttachments = colorAttachments;
    framebuffer = std::make_shared<OGLFramebuffer>();
    for (auto i = 0; i < colorAttachments.size(); ++i) {
        auto &attachment = colorAttachments.at(i);
        auto &tex = getTexture(attachment.texture);
        framebuffer->attach(attachment, tex, GL_COLOR_ATTACHMENT0 + i);
    }

    if (depthAttachment.texture) {
        auto &attachment = depthAttachment;
        auto &tex = getTexture(attachment.texture);
        framebuffer->attach(attachment, tex, GL_DEPTH_ATTACHMENT);
    }

    if (stencilAttachment.texture) {
        auto &attachment = stencilAttachment;
        auto &tex = getTexture(attachment.texture);
        framebuffer->attach(attachment, tex, GL_STENCIL_ATTACHMENT);
    }

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer->FBO);
    glBindVertexArray(vertexArray->VAO);
    oglCheckError();

    oglDebugEndGroup();
}

void ContextGL::beginRenderPass(const std::vector<RenderGraphAttachment> &colorAttachments,
                                const RenderGraphAttachment &depthStencilAttachment) {
    oglDebugStartGroup("ContextGL::beginRenderPass");

    framebufferColorAttachments = colorAttachments;
    framebufferDepthStencilAttachment = depthStencilAttachment;
    framebuffer = std::make_shared<OGLFramebuffer>();

    std::vector<GLenum> drawBuffers;

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->FBO);
    for (auto i = 0; i < colorAttachments.size(); ++i) {
        auto &attachment = colorAttachments.at(i);
        auto &tex = getTexture(attachment.texture);
        if (attachment.clearAttachment) {
            if (tex.texture.format >= R8I && tex.texture.format <= RGBA32I) {
                clearTextureColor(attachment.texture,
                                  attachment.clearColorInt,
                                  attachment.index,
                                  attachment.face,
                                  attachment.mipMapLevel);
            } else if (tex.texture.format >= R8UI && tex.texture.format <= RGBA32UI) {
                clearTextureColor(attachment.texture,
                                  attachment.clearColorUint,
                                  attachment.index,
                                  attachment.face,
                                  attachment.mipMapLevel);
            } else if (tex.texture.format >= R8 && tex.texture.format <= RGBA32F) {
                clearTextureColor(attachment.texture,
                                  attachment.clearColorFloat,
                                  attachment.index,
                                  attachment.face,
                                  attachment.mipMapLevel);
            } else {
                clearTextureColor(attachment.texture,
                                  attachment.clearColor,
                                  attachment.index,
                                  attachment.face,
                                  attachment.mipMapLevel);
            }
        }
        framebuffer->attach(attachment, tex, GL_COLOR_ATTACHMENT0 + i);
        drawBuffers.emplace_back(GL_COLOR_ATTACHMENT0 + i);
    }

    glDrawBuffers(static_cast<GLsizei>(drawBuffers.size()), drawBuffers.data());

    if (depthStencilAttachment.texture) {
        auto &attachment = depthStencilAttachment;
        auto &tex = getTexture(attachment.texture);
        if (attachment.clearAttachment) {
            clearTextureDepthStencil(attachment.texture,
                                     attachment.clearDepth,
                                     attachment.clearStencil,
                                     attachment.index,
                                     attachment.face,
                                     attachment.mipMapLevel);
        }
        framebuffer->attach(depthStencilAttachment, tex, GL_DEPTH_STENCIL_ATTACHMENT);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer->FBO);

    glBindVertexArray(vertexArray->VAO);
    oglCheckError();

    oglDebugEndGroup();
}

void ContextGL::bindPipeline(const RenderGraphResource pipeline) {
    oglDebugStartGroup("ContextGL::bindPipeline");

    const auto &pipelineDesc = resources.pipelines.at(pipeline);
    if (pipelineDesc.multisample)
        glEnable(GL_MULTISAMPLE);
    else
        glDisable(GL_MULTISAMPLE);

    if (pipelineDesc.multiSampleEnableFrequency)
        glEnable(GL_SAMPLE_COVERAGE);
    else
        glDisable(GL_SAMPLE_COVERAGE);

    glSampleCoverage(pipelineDesc.multiSampleFrequency, GL_TRUE);

    // Bind shader program
    auto shaderProgram = resources.shaderPrograms.at(pipeline);
    glUseProgram(shaderProgram->programHandle);

    // Setup pipeline state
    glDepthFunc(convert(pipelineDesc.depthTestMode));
    if (pipelineDesc.depthTestWrite)
        glDepthMask(GL_TRUE);
    else
        glDepthMask(GL_FALSE);

    if (pipelineDesc.enableDepthTest) {
        glEnable(GL_DEPTH_TEST);
    } else {
        glDisable(GL_DEPTH_TEST);
    }

    glStencilMask(pipelineDesc.stencilTestMask);
    glStencilFunc(convert(pipelineDesc.stencilMode),
                  pipelineDesc.stencilReference,
                  pipelineDesc.stencilFunctionMask);
    glStencilOp(convert(pipelineDesc.stencilFail),
                convert(pipelineDesc.stencilDepthFail),
                convert(pipelineDesc.stencilPass));

    if (pipelineDesc.enableStencilTest) {
        glEnable(GL_STENCIL_TEST);
    } else {
        glDisable(GL_STENCIL_TEST);
    }

    glCullFace(convert(pipelineDesc.faceCullMode));
    if (pipelineDesc.faceCullClockwiseWinding)
        glFrontFace(GL_CW);
    else
        glFrontFace(GL_CCW);

    if (pipelineDesc.enableFaceCulling) {
        glEnable(GL_CULL_FACE);
    } else {
        glDisable(GL_CULL_FACE);
    }

    glBlendFuncSeparate(convert(pipelineDesc.colorBlendSourceMode),
                        convert(pipelineDesc.colorBlendDestinationMode),
                        convert(pipelineDesc.alphaBlendSourceMode),
                        convert(pipelineDesc.alphaBlendDestinationMode));
    glBlendEquationSeparate(convert(pipelineDesc.colorBlendEquation),
                            convert(pipelineDesc.alphaBlendEquation));

    if (pipelineDesc.enableBlending) {
        glEnable(GL_BLEND);
    } else {
        glDisable(GL_BLEND);
    }

    oglCheckError();

    boundPipeline = pipeline;

    oglDebugEndGroup();
}

void ContextGL::bindVertexBuffer(const RenderGraphResource buffer) {
    if (!boundPipeline) {
        throw std::runtime_error("No current pipeline set");
    }

    oglDebugStartGroup("ContextGL::bindVertexBuffer");

    glBindBuffer(GL_ARRAY_BUFFER, resources.vertexBuffers.at(buffer)->VBO);

    // Setup vertex layout
    auto &desc = resources.pipelines.at(boundPipeline);

    auto &vertexLayout = desc.getVertexLayout();

    const auto vertexStride = static_cast<GLsizei>(vertexLayout.getLayoutSize());

    auto &attributes = vertexLayout.getElements();
    size_t currentOffset = 0;
    for (int i = 0; i < attributes.size(); i++) {
        auto &binding = attributes.at(i);
        glEnableVertexAttribArray(i);
        if (binding.component > ShaderDataType::SIGNED_INT) {
            glVertexAttribPointer(i,
                                  ShaderDataType::getCount(binding.type),
                                  getType(binding.component),
                                  GL_FALSE,
                                  vertexStride,
                                  reinterpret_cast<void *>(currentOffset));
        } else {
            glVertexAttribIPointer(i,
                                   ShaderDataType::getCount(binding.type),
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

void ContextGL::bindIndexBuffer(const RenderGraphResource buffer) {
    if (!boundPipeline) {
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

void ContextGL::bindTexture(const std::string &target, const std::vector<RenderGraphResource> &textureArray) {
    if (!boundPipeline) {
        throw std::runtime_error("No current pipeline");
    }

    oglDebugStartGroup("ContextGL::bindTexture");

    const auto &pipeline = resources.compiledPipelines.at(boundPipeline);

    const auto binding = pipeline.getTextureArrayBinding(target);
    for (auto i = 0; i < textureArray.size(); i++) {
        auto &texture = resources.textures.at(textureArray.at(i));
        glActiveTexture(getTextureSlot(binding + i));
        glBindTexture(texture->textureType, texture->handle);
    }

    oglCheckError();

    oglDebugEndGroup();
}

void ContextGL::bindShaderBuffer(const std::string &target,
                                 const RenderGraphResource buffer,
                                 const size_t offset,
                                 const size_t size) {
    if (!boundPipeline) {
        throw std::runtime_error("No current pipeline");
    }

    oglDebugStartGroup("ContextGL::bindShaderBuffer");

    const auto &pipeline = resources.compiledPipelines.at(boundPipeline);
    const auto binding = pipeline.getShaderBufferBinding(target);
    const auto &buf = resources.storageBuffers.at(buffer);
    if (size == 0) {
        if (buf->size == 0) {
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, static_cast<GLuint>(binding), buf->SSBO);
        } else {
            glBindBufferRange(GL_SHADER_STORAGE_BUFFER,
                              static_cast<GLuint>(binding),
                              buf->SSBO,
                              static_cast<GLintptr>(offset),
                              static_cast<GLsizeiptr>(buf->size - offset));
        }
    } else {
        glBindBufferRange(GL_SHADER_STORAGE_BUFFER,
                          static_cast<GLuint>(binding),
                          buf->SSBO,
                          static_cast<GLintptr>(offset),
                          static_cast<GLsizeiptr>(size));
    }

    oglCheckError();

    oglDebugEndGroup();
}

void ContextGL::setShaderParameter(const std::string &name, const ShaderLiteral &value) {
    throw std::runtime_error("Not implemented");
}

void ContextGL::clearColorAttachment(const size_t binding, const ColorRGBA clearColor) {
    oglDebugStartGroup("ContextGL::clearColorAttachment");

    const auto &attachment = framebufferColorAttachments.at(binding);
    auto &tex = getTexture(attachment.texture);

    clearTextureColor(attachment.texture,
                      clearColor,
                      attachment.index,
                      attachment.face,
                      attachment.mipMapLevel);

    oglCheckError();

    oglDebugEndGroup();
}

void ContextGL::clearColorAttachment(const size_t binding, const Vec4i &clearColor) {
    oglDebugStartGroup("ContextGL::clearColorAttachment");

    const auto &attachment = framebufferColorAttachments.at(binding);
    auto &tex = getTexture(attachment.texture);

    clearTextureColor(attachment.texture,
                      clearColor,
                      attachment.index,
                      attachment.face,
                      attachment.mipMapLevel);

    oglDebugEndGroup();
}

void ContextGL::clearColorAttachment(const size_t binding, const Vec4u &clearColor) {
    oglDebugStartGroup("ContextGL::clearColorAttachment");

    const auto &attachment = framebufferColorAttachments.at(binding);

    clearTextureColor(attachment.texture,
                      clearColor,
                      attachment.index,
                      attachment.face,
                      attachment.mipMapLevel);

    oglDebugEndGroup();
}

void ContextGL::clearDepthStencilAttachment(const float clearDepth, const unsigned int clearStencil) {
    oglDebugStartGroup("ContextGL::clearDepthStencilAttachment");

    const auto &attachment = framebufferDepthStencilAttachment;

    clearTextureDepthStencil(attachment.texture,
                             clearDepth,
                             clearStencil,
                             attachment.index,
                             attachment.face,
                             attachment.mipMapLevel);

    oglDebugEndGroup();
}

void ContextGL::clearDepthAttachment(const float clearDepth) {
    oglDebugStartGroup("ContextGL::clearDepthAttachment");

    const auto &attachment = framebufferDepthStencilAttachment;
    auto &tex = getTexture(attachment.texture);

    clearTextureDepthStencil(attachment.texture,
                             clearDepth,
                             0,
                             attachment.index,
                             attachment.face,
                             attachment.mipMapLevel);

    oglDebugEndGroup();
}

void ContextGL::clearStencilAttachment(const unsigned int clearStencil) {
    oglDebugStartGroup("ContextGL::clearStencilAttachment");

    const auto &attachment = framebufferDepthStencilAttachment;
    auto &tex = getTexture(attachment.texture);

    clearTextureDepthStencil(attachment.texture,
                             0,
                             clearStencil,
                             attachment.index,
                             attachment.face,
                             attachment.mipMapLevel);

    oglDebugEndGroup();
}

void ContextGL::setViewport(const Vec2i viewportOffset, const Vec2i viewportSize) {
    oglDebugStartGroup("ContextGL::setViewport");

    glViewport(viewportOffset.x, viewportOffset.y, viewportSize.x, viewportSize.y);
    oglCheckError();

    oglDebugEndGroup();
}

// TODO: Implement automatic multi draw
void ContextGL::drawArray(const DrawCall &drawCall) {
    oglDebugStartGroup("ContextGL::drawArray");

    auto &pip = resources.pipelines.at(boundPipeline);
    glDrawArrays(convert(pip.primitive),
                 static_cast<GLint>(drawCall.offset),
                 static_cast<GLsizei>(drawCall.count));
    oglCheckError();

    stats.drawCalls++;
    stats.polygons += drawCall.count / pip.primitive;

    oglDebugEndGroup();
}

void ContextGL::drawIndexed(const DrawCall &drawCall, const size_t indexOffset) {
    oglDebugStartGroup("ContextGL::drawIndexed");

    auto &pip = resources.pipelines.at(boundPipeline);
    glDrawElementsBaseVertex(convert(pip.primitive),
                             static_cast<GLsizei>(drawCall.count),
                             convert(drawCall.indexFormat),
                             reinterpret_cast<void *>(drawCall.offset),
                             static_cast<GLint>(indexOffset));
    oglCheckError();

    stats.drawCalls++;
    stats.polygons += drawCall.count / pip.primitive;

    oglDebugEndGroup();
}

void ContextGL::endRenderPass() {
    oglDebugStartGroup("ContextGL::endRenderPass");

    glBindVertexArray(0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    oglCheckError();

    boundPipeline = {};

    oglDebugEndGroup();
}

std::vector<uint8_t> ContextGL::downloadShaderBuffer(const RenderGraphResource buffer) {
    throw std::runtime_error("DownloadShaderBuffer not implemented");
}

Image<ColorRGBA> ContextGL::downloadTexture(const RenderGraphResource texture,
                                            const size_t index, size_t mipMapLevel,
                                            const CubeMapFace face) {
    throw std::runtime_error("DownloadTexture not implemented");
}

std::unordered_map<Shader::Stage, std::string> ContextGL::getShaderSource(const RenderGraphResource pipeline) {
    return resources.compiledPipelines.at(pipeline).sourceCode;
}

const OGLTexture &ContextGL::getTexture(const RenderGraphResource resource) const {
    if (resource == resources.graph.backBufferColor) {
        return *backBufferColor;
    }
    if (resource == resources.graph.backBufferDepthStencil) {
        return *backBufferDepthStencil;
    }
    return *resources.textures.at(resource);
}
