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

#ifndef XENGINE_RASTERCONTEXTGL_HPP
#define XENGINE_RASTERCONTEXTGL_HPP

#include "xng/rendergraph/context/rastercontext.hpp"

#include "typeconversion.hpp"
#include "resource/vertexarrayobject.hpp"

namespace xng::opengl {
    class RasterContextGL final : public rendergraph::RasterContext {
    public:
        RasterContextGL(PassResources &&resources, PipelineCacheGL &pipelineCache, Statistics &stats)
            : resources(std::move(resources)),
              pipelineCache(pipelineCache),
              stats(stats),
              emptySSBO(Buffer(1, Buffer::CAPABILITY_STORAGE, Buffer::MEMORY_GPU_ONLY)) {
        }

        ~RasterContextGL() override {
        }

        void bindPipeline(const PipelineCache::Handle &handle) override {
            oglDebugStartGroup("RasterContextGL::bindPipeline");

            const auto &pipeline = pipelineCache.getRasterPipelines().at(handle);
            const auto &shaderProgram = pipelineCache.getShaderProgram(handle);

            glUseProgram(shaderProgram.programHandle);

            if (pipeline.multisample)
                glEnable(GL_MULTISAMPLE);
            else
                glDisable(GL_MULTISAMPLE);

            if (pipeline.multiSampleEnableFrequency)
                glEnable(GL_SAMPLE_COVERAGE);
            else
                glDisable(GL_SAMPLE_COVERAGE);

            glSampleCoverage(pipeline.multiSampleFrequency, GL_TRUE);

            glDepthFunc(convert(pipeline.depthTestMode));

            if (pipeline.depthTestWrite)
                glDepthMask(GL_TRUE);
            else
                glDepthMask(GL_FALSE);

            if (pipeline.enableDepthTest) {
                glEnable(GL_DEPTH_TEST);
            } else {
                glDisable(GL_DEPTH_TEST);
            }

            glStencilMask(pipeline.stencilTestMask);
            glStencilFunc(convert(pipeline.stencilMode),
                          pipeline.stencilReference,
                          pipeline.stencilFunctionMask);
            glStencilOp(convert(pipeline.stencilFail),
                        convert(pipeline.stencilDepthFail),
                        convert(pipeline.stencilPass));

            if (pipeline.enableStencilTest) {
                glEnable(GL_STENCIL_TEST);
            } else {
                glDisable(GL_STENCIL_TEST);
            }

            glCullFace(convert(pipeline.faceCullMode));
            if (pipeline.faceCullWinding == RasterPipeline::CLOCKWISE)
                glFrontFace(GL_CW);
            else
                glFrontFace(GL_CCW);

            if (pipeline.enableFaceCulling) {
                glEnable(GL_CULL_FACE);
            } else {
                glDisable(GL_CULL_FACE);
            }

            glBlendFuncSeparate(convert(pipeline.colorBlendSourceMode),
                                convert(pipeline.colorBlendDestinationMode),
                                convert(pipeline.alphaBlendSourceMode),
                                convert(pipeline.alphaBlendDestinationMode));
            glBlendEquationSeparate(convert(pipeline.colorBlendEquation),
                                    convert(pipeline.alphaBlendEquation));

            if (pipeline.enableBlending) {
                glEnable(GL_BLEND);
            } else {
                glDisable(GL_BLEND);
            }

            // Enable seamless cubemap filtering so texel lookups blend across face boundaries
            // For the render graph abstraction this is assumed as default on.
            // On Vulkan / DirectX this should be on without additional configuration.
            glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

            glBindVertexArray(vertexArray.VAO);

            oglCheckError();

            boundPipeline = handle;

            oglDebugEndGroup();
        }

        void bindVertexBuffer(const Resource<VertexBuffer> &buffer) override {
            if (!boundPipeline.has_value()) {
                throw std::runtime_error("Must bind pipeline before binding vertex buffer");
            }

            oglDebugStartGroup("RasterContextGL::bindVertexBuffer");

            glBindBuffer(GL_ARRAY_BUFFER, resources.getBuffer(buffer).handle);

            // Setup vertex layout
            const auto &vertexLayout = pipelineCache.getRasterPipeline(boundPipeline.value()).getVertexLayout();
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

            glBindVertexArray(vertexArray.VAO);

            oglCheckError();

            oglDebugEndGroup();
        }

        void bindIndexBuffer(const Resource<IndexBuffer> &buffer) override {
            if (!boundPipeline.has_value()) {
                throw std::runtime_error("Must bind pipeline before binding index buffer");
            }

            oglDebugStartGroup("RasterContextGL::bindIndexBuffer");

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, resources.getBuffer(buffer).handle);

            glBindVertexArray(0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            glBindVertexArray(vertexArray.VAO);

            oglCheckError();

            oglDebugEndGroup();
        }

        void bindStorageBuffer(const std::string &target,
                               const Resource<StorageBuffer> &buffer,
                               const size_t offset,
                               const size_t size) override {
            if (!boundPipeline.has_value()) {
                throw std::runtime_error("Must bind pipeline before binding storage buffer");
            }

            oglDebugStartGroup("RasterContextGL::bindStorageBuffer");

            const auto binding = pipelineCache.getCompiledShader(boundPipeline.value()).getShaderBufferBinding(target);

            const auto &buf = resources.getBuffer(buffer);
            if (size == 0) {
                if (buf.desc.size == 0) {
                    // Bind SSBO with a size of one byte to avoid undefined behavior
                    // caused by binding a SSBO with size = 0 per OpenGL spec.
                    // On AMD this was handled by the driver, but the Intel driver breaks if SSBO is 0
                    glBindBufferRange(GL_SHADER_STORAGE_BUFFER, static_cast<GLuint>(binding), emptySSBO.handle, 0, 1);
                } else {
                    glBindBufferRange(GL_SHADER_STORAGE_BUFFER,
                                      static_cast<GLuint>(binding),
                                      buf.handle,
                                      static_cast<GLintptr>(offset),
                                      static_cast<GLsizeiptr>(buf.desc.size - offset));
                }
            } else {
                glBindBufferRange(GL_SHADER_STORAGE_BUFFER,
                                  static_cast<GLuint>(binding),
                                  buf.handle,
                                  static_cast<GLintptr>(offset),
                                  static_cast<GLsizeiptr>(size));
            }

            oglCheckError();

            oglDebugEndGroup();
        }

        void bindTexture(const std::string &target, const std::vector<TextureBinding> &textureArray) override {
            if (!boundPipeline.has_value()) {
                throw std::runtime_error("Must bind pipeline before binding texture.");
            }

            oglDebugStartGroup("RasterContextGL::bindTexture");

            const auto binding = pipelineCache.getCompiledShader(boundPipeline.value())
                    .getTextureArrayBinding(target);

            for (auto i = 0; i < textureArray.size(); i++) {
                const auto &texture = resources.getTexture(textureArray.at(i).texture);
                glActiveTexture(getTextureSlot(binding + i));
                glBindTexture(texture.textureType, texture.handle);
            }

            oglCheckError();

            oglDebugEndGroup();
        }

        void setShaderParameter(const std::string &name, const ShaderPrimitive &value) override {
            throw std::runtime_error("Not implemented");
        }

        void setViewport(const Vec2i viewportOffset, const Vec2i viewportSize) override {
            if (!boundPipeline.has_value()) {
                throw std::runtime_error("Must bind pipeline before setting viewport.");
            }

            oglDebugStartGroup("RasterContextGL::setViewport");

            glViewport(viewportOffset.x, viewportOffset.y, viewportSize.x, viewportSize.y);
            oglCheckError();

            oglDebugEndGroup();
        }

        void drawArray(const DrawCall &drawCall) override {
            if (!boundPipeline.has_value()) {
                throw std::runtime_error("Must bind pipeline before drawing.");
            }

            const auto primitive = pipelineCache.getRasterPipeline(boundPipeline.value()).primitive;

            oglDebugStartGroup("RasterContextGL::drawArray");

            glDrawArrays(convert(primitive),
                         static_cast<GLint>(drawCall.offset),
                         static_cast<GLsizei>(drawCall.count));
            oglCheckError();

            oglDebugEndGroup();

            stats.drawCalls++;
            stats.polygons += drawCall.count / primitive;
        }

        void drawIndexed(const DrawCall &drawCall, const size_t indexOffset) override {
            if (!boundPipeline.has_value()) {
                throw std::runtime_error("Must bind pipeline before drawing.");
            }

            const auto primitive = pipelineCache.getRasterPipeline(boundPipeline.value()).primitive;

            oglDebugStartGroup("RasterContextGL::drawIndexed");

            glDrawElementsBaseVertex(convert(primitive),
                                     static_cast<GLsizei>(drawCall.count),
                                     convert(drawCall.indexFormat),
                                     reinterpret_cast<void *>(drawCall.offset),
                                     static_cast<GLint>(indexOffset));
            oglCheckError();

            stats.drawCalls++;
            stats.polygons += drawCall.count / primitive;

            oglDebugEndGroup();
        }

    private:
        PassResources resources;
        PipelineCacheGL &pipelineCache;
        Statistics &stats;

        std::optional<PipelineCache::Handle> boundPipeline;

        VertexArrayObject vertexArray;
        BufferGL emptySSBO;
    };
}

#endif //XENGINE_RASTERCONTEXTGL_HPP
