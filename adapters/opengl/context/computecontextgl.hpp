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

#ifndef XENGINE_COMPUTECONTEXTGL_HPP
#define XENGINE_COMPUTECONTEXTGL_HPP

#include "xng/rendergraph/context/computecontext.hpp"

namespace xng::opengl {
    class ComputeContextGL final : public rendergraph::ComputeContext {
    public:
        ComputeContextGL(PassResources &&resources, PipelineCacheGL &pipelineCache)
            : resources(std::move(resources)),
              pipelineCache(pipelineCache),
              emptySSBO(Buffer(1, Buffer::CAPABILITY_STORAGE, Buffer::MEMORY_GPU_ONLY)) {
        }

        ~ComputeContextGL() override = default;

        void bindPipeline(const PipelineCache::Handle &pipeline) override {
            oglDebugStartGroup("ContextGL::bindPipeline");

            const auto shaderProgram = pipelineCache.getShaderProgram(pipeline);

            glUseProgram(shaderProgram.programHandle);

            // Enable seamless cubemap filtering so texel lookups blend across face boundaries
            // For the render graph abstraction this is assumed as default on.
            // On Vulkan / DirectX this should be on without additional configuration.
            glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

            oglCheckError();

            boundPipeline = pipeline;

            oglDebugEndGroup();
        }

        void bindStorageBuffer(const std::string &target,
                               const Resource<StorageBuffer> &buffer,
                               const size_t offset,
                               const size_t size) override {
            if (!boundPipeline.has_value()) {
                throw std::runtime_error("Must bind pipeline before binding storage buffer");
            }

            oglDebugStartGroup("ComputeContextGL::bindStorageBuffer");

            const auto binding = pipelineCache.getCompiledShader(boundPipeline.value()).getShaderBufferBinding(target);

            const auto &buf = resources.getBuffer(buffer);
            if (size == 0) {
                if (buf.desc.size == 0) {
                    // Bind SSBO with a size of one byte to avoid undefined behavior
                    // caused by binding a SSBO with size = 0 per OpenGL spec.
                    // On AMD this was handled by the driver but Intel driver breaks if SSBO is 0
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

            oglDebugStartGroup("ComputeContextGL::bindTexture");

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

        void dispatch(const Vec3u groupCount) override {
            glDispatchCompute(groupCount.x, groupCount.y, groupCount.z);
        }

    private:
        PassResources resources;
        PipelineCacheGL &pipelineCache;

        std::optional<PipelineCache::Handle> boundPipeline;

        BufferGL emptySSBO;
    };
}

#endif //XENGINE_COMPUTECONTEXTGL_HPP
