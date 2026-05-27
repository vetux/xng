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

#ifndef XENGINE_RASTERCONTEXTGL_HPP
#define XENGINE_RASTERCONTEXTGL_HPP

#include "xng/rendergraph/context/rastercontext.hpp"
#include "xng/rendergraph/shader/shaderprimitive.hpp"

#include "typeconversion.hpp"
#include "resource/vertexarrayobject.hpp"

namespace xng::opengl {
    class RasterContextGL final : public rg::RasterContext {
    public:
        RasterContextGL(const PassResources &resources, PipelineCacheGL &pipelineCache)
            : resources(resources),
              pipelineCache(pipelineCache),
              emptySSBO(Buffer(1, Buffer::CAPABILITY_STORAGE, Buffer::MEMORY_GPU_ONLY)) {
        }

        ~RasterContextGL() override = default;

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

            // Setup VAO
            glBindVertexArray(vertexArray.VAO);

            const auto &elements = pipeline.vertexFormat.layout.getElements();
            const auto &bindingPoints = pipeline.vertexFormat.bindingPoints;
            const auto &offsets = pipeline.vertexFormat.offsets;

            if (elements.size() != bindingPoints.size() || elements.size() != offsets.size()) {
                throw std::runtime_error(
                    "Vertex format elements, binding points and offsets must be defined for each element");
            }

            for (auto i = 0; i < elements.size(); i++) {
                glEnableVertexAttribArray(i);
                glVertexAttribBinding(i, bindingPoints.at(i));

                if (isIntegerFormat(elements.at(i).component)) {
                    glVertexAttribIFormat(i,
                                          elements.at(i).getSize(),
                                          getType(elements.at(i).component),
                                          offsets.at(i));
                } else {
                    glVertexAttribFormat(i,
                                         elements.at(i).getSize(),
                                         getType(elements.at(i).component),
                                         GL_FALSE,
                                         offsets.at(i));
                }
            }

            oglCheckError();

            boundPipeline = handle;

            indexFormat = INDEX_UNDEFINED;

            oglDebugEndGroup();
        }

        void bindVertexBuffer(const Resource<Buffer> &buffer,
                              const unsigned int bindingPoint,
                              const size_t offset,
                              const size_t stride) override {
            if (!boundPipeline.has_value()) {
                throw std::runtime_error("Must bind pipeline before binding vertex buffer");
            }

            if (!buffer.isAssigned()) {
                throw std::runtime_error("Unassigned buffer resource");
            }

            if (!(buffer.getDescription().capabilityFlags & Buffer::CAPABILITY_VERTEX)) {
                throw std::runtime_error("Buffer must have CAPABILITY_VERTEX");
            }

            oglDebugStartGroup("RasterContextGL::bindVertexBuffer");

            glBindVertexBuffer(bindingPoint, resources.getBuffer(buffer).handle, offset, stride);

            oglCheckError();

            oglDebugEndGroup();
        }

        void bindIndexBuffer(const Resource<Buffer> &buffer, const IndexFormat format) override {
            if (!boundPipeline.has_value()) {
                throw std::runtime_error("Must bind pipeline before binding index buffer");
            }

            if (!buffer.isAssigned()) {
                throw std::runtime_error("Unassigned buffer resource");
            }

            if (!(buffer.getDescription().capabilityFlags & Buffer::CAPABILITY_INDEX)) {
                throw std::runtime_error("Buffer must have CAPABILITY_VERTEX");
            }

            oglDebugStartGroup("RasterContextGL::bindIndexBuffer");

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, resources.getBuffer(buffer).handle);

            oglCheckError();

            oglDebugEndGroup();

            indexFormat = format;
        }

        void bindStorageBuffer(const std::string &target,
                               const Resource<Buffer> &buffer,
                               const size_t offset,
                               const size_t size) override {
            if (!boundPipeline.has_value()) {
                throw std::runtime_error("Must bind pipeline before binding storage buffer");
            }

            if (!buffer.isAssigned()) {
                throw std::runtime_error("Unassigned buffer resource");
            }

            if (!(buffer.getDescription().capabilityFlags & Buffer::CAPABILITY_STORAGE)) {
                throw std::runtime_error("Buffer must have CAPABILITY_STORAGE");
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
                if (!textureArray.at(i).texture.isAssigned()) {
                    throw std::runtime_error("Unassigned texture resource");
                }
                const auto &texture = resources.getTexture(textureArray.at(i).texture);
                glActiveTexture(getTextureSlot(binding + i));
                glBindTexture(texture.textureType, texture.handle);
            }

            oglCheckError();

            oglDebugEndGroup();
        }

        void setShaderParameter(const std::string &name, const ShaderPrimitive &value) override {
            const auto location = pipelineCache.getCompiledShader(boundPipeline.value()).getParameterBinding(name);
            const auto paramType = pipelineCache.getCompiledShader(boundPipeline.value()).parameterTypes.at(location);

            if (paramType != value.getType()) {
                throw std::runtime_error("Shader parameter type mismatch");
            }

            switch (paramType.type) {
                case ShaderPrimitiveType::SCALAR: {
                    switch (paramType.component) {
                        case ShaderPrimitiveType::BOOLEAN:
                            glUniform1i(location, std::get<bool>(value.value));
                            break;
                        case ShaderPrimitiveType::UNSIGNED_INT:
                            glUniform1ui(location, std::get<unsigned int>(value.value));
                            break;
                        case ShaderPrimitiveType::SIGNED_INT:
                            glUniform1i(location, std::get<int>(value.value));
                            break;
                        case ShaderPrimitiveType::FLOAT:
                            glUniform1f(location, std::get<float>(value.value));
                            break;
                        case ShaderPrimitiveType::DOUBLE:
                            glUniform1d(location, std::get<double>(value.value));
                            break;
                    }
                }
                break;
                case ShaderPrimitiveType::VECTOR2: {
                    switch (paramType.component) {
                        case ShaderPrimitiveType::BOOLEAN: {
                            const auto &val = std::get<Vec2b>(value.value);
                            glUniform2i(location, val.x, val.y);
                            break;
                        }
                        case ShaderPrimitiveType::UNSIGNED_INT: {
                            const auto &val = std::get<Vec2u>(value.value);
                            glUniform2ui(location, val.x, val.y);
                            break;
                        }
                        case ShaderPrimitiveType::SIGNED_INT: {
                            const auto &val = std::get<Vec2i>(value.value);
                            glUniform2i(location, val.x, val.y);
                            break;
                        }
                        case ShaderPrimitiveType::FLOAT: {
                            const auto &val = std::get<Vec2f>(value.value);
                            glUniform2f(location, val.x, val.y);
                            break;
                        }
                        case ShaderPrimitiveType::DOUBLE: {
                            const auto &val = std::get<Vec2d>(value.value);
                            glUniform2d(location, val.x, val.y);
                            break;
                        }
                    }
                }
                break;
                case ShaderPrimitiveType::VECTOR3: {
                    switch (paramType.component) {
                        case ShaderPrimitiveType::BOOLEAN: {
                            const auto &val = std::get<Vec3b>(value.value);
                            glUniform3i(location, val.x, val.y, val.z);
                            break;
                        }
                        case ShaderPrimitiveType::UNSIGNED_INT: {
                            const auto &val = std::get<Vec3u>(value.value);
                            glUniform3ui(location, val.x, val.y, val.z);
                            break;
                        }
                        case ShaderPrimitiveType::SIGNED_INT: {
                            const auto &val = std::get<Vec3i>(value.value);
                            glUniform3i(location, val.x, val.y, val.z);
                            break;
                        }
                        case ShaderPrimitiveType::FLOAT: {
                            const auto &val = std::get<Vec3f>(value.value);
                            glUniform3f(location, val.x, val.y, val.z);
                            break;
                        }
                        case ShaderPrimitiveType::DOUBLE: {
                            const auto &val = std::get<Vec3d>(value.value);
                            glUniform3d(location, val.x, val.y, val.z);
                            break;
                        }
                    }
                }
                break;
                case ShaderPrimitiveType::VECTOR4: {
                    switch (paramType.component) {
                        case ShaderPrimitiveType::BOOLEAN: {
                            const auto &val = std::get<Vec4b>(value.value);
                            glUniform4i(location, val.x, val.y, val.z, val.w);
                            break;
                        }
                        case ShaderPrimitiveType::UNSIGNED_INT: {
                            const auto &val = std::get<Vec4u>(value.value);
                            glUniform4ui(location, val.x, val.y, val.z, val.w);
                            break;
                        }
                        case ShaderPrimitiveType::SIGNED_INT: {
                            const auto &val = std::get<Vec4i>(value.value);
                            glUniform4i(location, val.x, val.y, val.z, val.w);
                            break;
                        }
                        case ShaderPrimitiveType::FLOAT: {
                            const auto &val = std::get<Vec4f>(value.value);
                            glUniform4f(location, val.x, val.y, val.z, val.w);
                            break;
                        }
                        case ShaderPrimitiveType::DOUBLE: {
                            const auto &val = std::get<Vec4d>(value.value);
                            glUniform4d(location, val.x, val.y, val.z, val.w);
                            break;
                        }
                    }
                }
                break;
                case ShaderPrimitiveType::MAT2: {
                    switch (paramType.component) {
                        case ShaderPrimitiveType::FLOAT: {
                            const auto &val = std::get<Mat2f>(value.value);
                            glUniformMatrix2fv(location, 1, GL_FALSE, reinterpret_cast<const GLfloat *>(val.data));
                            break;
                        }
                        case ShaderPrimitiveType::DOUBLE: {
                            const auto &val = std::get<Mat2d>(value.value);
                            glUniformMatrix2dv(location, 1, GL_FALSE, reinterpret_cast<const GLdouble *>(val.data));
                            break;
                        }
                        default:
                            throw std::runtime_error("Unsupported matrix component");
                    }
                }
                break;
                case ShaderPrimitiveType::MAT3: {
                    switch (paramType.component) {
                        case ShaderPrimitiveType::FLOAT: {
                            const auto &val = std::get<Mat3f>(value.value);
                            glUniformMatrix3fv(location, 1, GL_FALSE, reinterpret_cast<const GLfloat *>(val.data));
                            break;
                        }
                        case ShaderPrimitiveType::DOUBLE: {
                            const auto &val = std::get<Mat3d>(value.value);
                            glUniformMatrix3dv(location, 1, GL_FALSE, reinterpret_cast<const GLdouble *>(val.data));
                            break;
                        }
                        default:
                            throw std::runtime_error("Unsupported matrix component");
                    }
                }
                break;
                case ShaderPrimitiveType::MAT4: {
                    switch (paramType.component) {
                        case ShaderPrimitiveType::FLOAT: {
                            const auto &val = std::get<Mat4f>(value.value);
                            glUniformMatrix4fv(location, 1, GL_FALSE, reinterpret_cast<const GLfloat *>(val.data));
                            break;
                        }
                        case ShaderPrimitiveType::DOUBLE: {
                            const auto &val = std::get<Mat4d>(value.value);
                            glUniformMatrix4dv(location, 1, GL_FALSE, reinterpret_cast<const GLdouble *>(val.data));
                            break;
                        }
                        default:
                            throw std::runtime_error("Unsupported matrix component");
                    }
                }
                break;
            }
        }

        void setViewport(const Vec2i viewportOffset, const Vec2i viewportSize) override {
            if (!boundPipeline.has_value()) {
                throw std::runtime_error("Must bind pipeline before setting viewport.");
            }

            oglDebugStartGroup("RasterContextGL::setViewport");

            glViewport(viewportOffset.x, frameBufferHeight - viewportOffset.y, viewportSize.x, viewportSize.y);
            oglCheckError();

            oglDebugEndGroup();
        }

        void setStencilReference(const int value) override {
            if (!boundPipeline.has_value()) {
                throw std::runtime_error("Must bind pipeline before setting stencil reference.");
            }

            const auto &pipeline = pipelineCache.getRasterPipeline(boundPipeline.value());

            if (!pipeline.enableDynamicStencilReference) {
                throw std::runtime_error("Dynamic stencil reference not enabled.");
            }

            oglDebugStartGroup("RasterContextGL::setStencilReference");

            glStencilFunc(convert(pipeline.stencilMode),
                          value,
                          pipeline.stencilFunctionMask);
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
        }

        void drawIndexed(const DrawCall &drawCall, const int indexOffset) override {
            if (!boundPipeline.has_value()) {
                throw std::runtime_error("Must bind pipeline before drawing.");
            }

            if (indexFormat == INDEX_UNDEFINED) {
                throw std::runtime_error("Must bind index buffer before drawing.");
            }

            const auto primitive = pipelineCache.getRasterPipeline(boundPipeline.value()).primitive;

            oglDebugStartGroup("RasterContextGL::drawIndexed");

            glDrawElementsBaseVertex(convert(primitive),
                                     static_cast<GLsizei>(drawCall.count),
                                     convert(indexFormat),
                                     reinterpret_cast<void *>(static_cast<uintptr_t>(drawCall.offset)),
                                     static_cast<GLint>(indexOffset));
            oglCheckError();

            oglDebugEndGroup();
        }

        void drawArrayInstanced(const DrawCall &drawCall, const unsigned int instanceCount) override {
            if (!boundPipeline.has_value()) {
                throw std::runtime_error("Must bind pipeline before drawing.");
            }

            const auto primitive = pipelineCache.getRasterPipeline(boundPipeline.value()).primitive;

            oglDebugStartGroup("RasterContextGL::drawArrayInstanced");

            glDrawArraysInstanced(convert(primitive),
                                  static_cast<GLint>(drawCall.offset),
                                  static_cast<GLsizei>(drawCall.count),
                                  static_cast<GLsizei>(instanceCount));
            oglCheckError();

            oglDebugEndGroup();
        }

        void drawIndexedInstanced(const DrawCall &drawCall,
                                  const int indexOffset,
                                  const unsigned int instanceCount) override {
            if (!boundPipeline.has_value()) {
                throw std::runtime_error("Must bind pipeline before drawing.");
            }

            if (indexFormat == INDEX_UNDEFINED) {
                throw std::runtime_error("Must bind index buffer before drawing.");
            }

            const auto primitive = pipelineCache.getRasterPipeline(boundPipeline.value()).primitive;

            oglDebugStartGroup("RasterContextGL::drawIndexedInstanced");

            glDrawElementsInstancedBaseVertex(convert(primitive),
                                              static_cast<GLsizei>(drawCall.count),
                                              convert(indexFormat),
                                              reinterpret_cast<void *>(static_cast<uintptr_t>(drawCall.offset)),
                                              static_cast<GLint>(indexOffset),
                                              static_cast<GLsizei>(instanceCount));
            oglCheckError();

            oglDebugEndGroup();
        }

        void drawArrayMulti(const std::vector<DrawCall> &drawCalls) override {
            if (!boundPipeline.has_value()) {
                throw std::runtime_error("Must bind pipeline before drawing.");
            }

            const auto primitive = pipelineCache.getRasterPipeline(boundPipeline.value()).primitive;

            if (drawCalls.empty()) {
                return;
            }

            oglDebugStartGroup("RasterContextGL::drawArrayMulti");

            std::vector<GLint> offsets;
            std::vector<GLsizei> counts;

            for (auto &drawCall: drawCalls) {
                offsets.emplace_back(static_cast<GLint>(drawCall.offset));
                counts.emplace_back(static_cast<GLsizei>(drawCall.count));
            }

            glMultiDrawArrays(convert(primitive),
                              static_cast<GLint *>(offsets.data()),
                              static_cast<GLsizei *>(counts.data()),
                              static_cast<GLsizei>(offsets.size()));

            oglCheckError();

            oglDebugEndGroup();
        }

        void drawIndexedMulti(const std::vector<std::pair<DrawCall, int> > &drawCalls) override {
            if (!boundPipeline.has_value()) {
                throw std::runtime_error("Must bind pipeline before drawing.");
            }

            if (indexFormat == INDEX_UNDEFINED) {
                throw std::runtime_error("Must bind index buffer before drawing.");
            }

            const auto primitive = pipelineCache.getRasterPipeline(boundPipeline.value()).primitive;

            if (drawCalls.empty()) {
                return;
            }

            oglDebugStartGroup("RasterContextGL::drawIndexedMulti");

            std::vector<void *> offsets;
            std::vector<GLsizei> counts;
            std::vector<GLint> indexOffsets;

            for (auto &pair: drawCalls) {
                offsets.emplace_back(reinterpret_cast<void *>(static_cast<uintptr_t>(pair.first.offset)));
                counts.emplace_back(static_cast<GLsizei>(pair.first.count));
                indexOffsets.emplace_back(static_cast<GLint>(pair.second));
            }

            glMultiDrawElementsBaseVertex(convert(primitive),
                                          static_cast<GLsizei *>(counts.data()),
                                          convert(indexFormat),
                                          static_cast<void **>(offsets.data()),
                                          static_cast<GLsizei>(drawCalls.size()),
                                          static_cast<GLint *>(indexOffsets.data()));

            oglCheckError();

            oglDebugEndGroup();
        }

        void drawArrayIndirect(const Resource<Buffer> &indirectBuffer, const size_t offset) override {
            if (!boundPipeline.has_value()) {
                throw std::runtime_error("Must bind pipeline before drawing.");
            }

            if (!indirectBuffer.isAssigned()) {
                throw std::runtime_error("Unassigned buffer resource");
            }

            const auto primitive = pipelineCache.getRasterPipeline(boundPipeline.value()).primitive;
            const auto &buffer = resources.getBuffer(indirectBuffer);

            oglDebugStartGroup("RasterContextGL::drawArrayIndirect");

            glBindBuffer(GL_DRAW_INDIRECT_BUFFER, buffer.handle);

            glDrawArraysIndirect(convert(primitive), reinterpret_cast<void *>(static_cast<uintptr_t>(offset)));

            oglCheckError();

            oglDebugEndGroup();
        }

        void drawIndexedIndirect(const Resource<Buffer> &indirectBuffer, const size_t offset) override {
            if (!boundPipeline.has_value()) {
                throw std::runtime_error("Must bind pipeline before drawing.");
            }

            if (!indirectBuffer.isAssigned()) {
                throw std::runtime_error("Unassigned buffer resource");
            }

            if (indexFormat == INDEX_UNDEFINED) {
                throw std::runtime_error("Must bind index buffer before drawing.");
            }

            const auto primitive = pipelineCache.getRasterPipeline(boundPipeline.value()).primitive;
            const auto &buffer = resources.getBuffer(indirectBuffer);

            oglDebugStartGroup("RasterContextGL::drawIndexedIndirect");

            glBindBuffer(GL_DRAW_INDIRECT_BUFFER, buffer.handle);

            glDrawElementsIndirect(convert(primitive),
                                   convert(indexFormat),
                                   reinterpret_cast<void *>(static_cast<uintptr_t>(offset)));

            oglCheckError();

            oglDebugEndGroup();
        }

        void drawArrayMultiIndirect(const Resource<Buffer> &indirectBuffer,
                                    const size_t offset,
                                    const size_t drawCount,
                                    const size_t stride) override {
            if (!boundPipeline.has_value()) {
                throw std::runtime_error("Must bind pipeline before drawing.");
            }

            if (!indirectBuffer.isAssigned()) {
                throw std::runtime_error("Unassigned buffer resource");
            }

            const auto primitive = pipelineCache.getRasterPipeline(boundPipeline.value()).primitive;
            const auto &buffer = resources.getBuffer(indirectBuffer);

            oglDebugStartGroup("RasterContextGL::drawArrayMultiIndirect");

            glBindBuffer(GL_DRAW_INDIRECT_BUFFER, buffer.handle);

            glMultiDrawArraysIndirect(convert(primitive),
                                      reinterpret_cast<void *>(static_cast<uintptr_t>(offset)),
                                      static_cast<GLsizei>(drawCount),
                                      static_cast<GLsizei>(stride));

            oglCheckError();

            oglDebugEndGroup();
        }

        void drawIndexedMultiIndirect(const Resource<Buffer> &indirectBuffer,
                                      const size_t offset,
                                      const size_t drawCount,
                                      const size_t stride) override {
            if (!boundPipeline.has_value()) {
                throw std::runtime_error("Must bind pipeline before drawing.");
            }

            if (!indirectBuffer.isAssigned()) {
                throw std::runtime_error("Unassigned buffer resource");
            }

            if (indexFormat == INDEX_UNDEFINED) {
                throw std::runtime_error("Must bind index buffer before drawing.");
            }

            const auto primitive = pipelineCache.getRasterPipeline(boundPipeline.value()).primitive;
            const auto &buffer = resources.getBuffer(indirectBuffer);

            oglDebugStartGroup("RasterContextGL::drawIndexedMultiIndirect");

            glBindBuffer(GL_DRAW_INDIRECT_BUFFER, buffer.handle);

            glMultiDrawElementsIndirect(convert(primitive),
                                        convert(indexFormat),
                                        reinterpret_cast<void *>(static_cast<uintptr_t>(offset)),
                                        static_cast<GLsizei>(drawCount),
                                        static_cast<GLsizei>(stride));

            oglCheckError();

            oglDebugEndGroup();
        }

        void drawArrayMultiIndirectCount(const Resource<Buffer> &indirectBuffer,
                                         const Resource<Buffer> &drawCountBuffer,
                                         const size_t indirectOffset,
                                         const size_t drawCountOffset,
                                         const size_t maxDrawCount,
                                         const size_t stride) override {
            if (!boundPipeline.has_value()) {
                throw std::runtime_error("Must bind pipeline before drawing.");
            }

            if (!indirectBuffer.isAssigned() || !drawCountBuffer.isAssigned()) {
                throw std::runtime_error("Unassigned buffer resource");
            }

            const auto primitive = pipelineCache.getRasterPipeline(boundPipeline.value()).primitive;

            const auto &indirectBuff = resources.getBuffer(indirectBuffer);
            const auto &drawCountBuff = resources.getBuffer(drawCountBuffer);

            oglDebugStartGroup("RasterContextGL::drawArrayMultiIndirectCount");

            glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuff.handle);
            glBindBuffer(GL_PARAMETER_BUFFER, drawCountBuff.handle);

            glMultiDrawArraysIndirectCount(convert(primitive),
                                           reinterpret_cast<void *>(static_cast<uintptr_t>(indirectOffset)),
                                           static_cast<GLintptr>(drawCountOffset),
                                           static_cast<GLsizei>(maxDrawCount),
                                           static_cast<GLsizei>(stride));

            oglCheckError();

            oglDebugEndGroup();
        }

        void drawIndexedMultiIndirectCount(const Resource<Buffer> &indirectBuffer,
                                           const Resource<Buffer> &drawCountBuffer,
                                           const size_t indirectOffset,
                                           const size_t drawCountOffset,
                                           const size_t maxDrawCount,
                                           const size_t stride) override {
            if (!boundPipeline.has_value()) {
                throw std::runtime_error("Must bind pipeline before drawing.");
            }

            if (!indirectBuffer.isAssigned() || !drawCountBuffer.isAssigned()) {
                throw std::runtime_error("Unassigned buffer resource");
            }

            if (indexFormat == INDEX_UNDEFINED) {
                throw std::runtime_error("Must bind index buffer before drawing.");
            }

            const auto primitive = pipelineCache.getRasterPipeline(boundPipeline.value()).primitive;

            const auto &indirectBuff = resources.getBuffer(indirectBuffer);
            const auto &drawCountBuff = resources.getBuffer(drawCountBuffer);

            oglDebugStartGroup("RasterContextGL::drawIndexedMultiIndirectCount");

            glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuff.handle);
            glBindBuffer(GL_PARAMETER_BUFFER, drawCountBuff.handle);

            glMultiDrawElementsIndirectCount(convert(primitive),
                                             convert(indexFormat),
                                             reinterpret_cast<void *>(static_cast<uintptr_t>(indirectOffset)),
                                             static_cast<GLintptr>(drawCountOffset),
                                             static_cast<GLsizei>(maxDrawCount),
                                             static_cast<GLsizei>(stride));

            oglCheckError();

            oglDebugEndGroup();
        }

        void setFrameBufferHeight(const int height) {
            frameBufferHeight = height;
        }

    private:
        const PassResources &resources;
        PipelineCacheGL &pipelineCache;

        std::optional<PipelineCache::Handle> boundPipeline;

        VertexArrayObject vertexArray;
        BufferGL emptySSBO;

        int frameBufferHeight = 0;

        IndexFormat indexFormat = INDEX_UNDEFINED;

        static bool isIntegerFormat(const ShaderPrimitiveType::Component component) {
            return component <= ShaderPrimitiveType::SIGNED_INT;
        }
    };
}

#endif //XENGINE_RASTERCONTEXTGL_HPP
