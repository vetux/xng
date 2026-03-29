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

#ifndef XENGINE_PIPELINECACHEGL_HPP
#define XENGINE_PIPELINECACHEGL_HPP

#include <unordered_map>
#include <unordered_set>

#include "glsl/compiledshader.hpp"
#include "xng/rendergraph/pipelinecache.hpp"

#include "resource/shaderprogram.hpp"

namespace xng::opengl {
    class PipelineCacheGL : public rendergraph::PipelineCache {
    public:
        PipelineCacheGL() = default;

        ~PipelineCacheGL() override = default;

        Handle create(const rendergraph::RasterPipeline &desc) override;

        Handle create(const rendergraph::ComputePipeline &desc) override;

        void destroy(Handle handle) override;

        void clear() override;

        void save(std::ostream &stream) override;

        void load(std::istream &stream) override;

        std::unordered_map<rendergraph::Shader::Stage, std::string> getCompiledShaderSource(Handle handle) override;

        [[nodiscard]] const std::unordered_map<Handle, rendergraph::RasterPipeline> &getRasterPipelines() const {
            return rasterPipelines;
        }

        [[nodiscard]] const rendergraph::RasterPipeline &getRasterPipeline(const Handle handle) const {
            return rasterPipelines.at(handle);
        }

        [[nodiscard]] const std::unordered_map<Handle, rendergraph::ComputePipeline> &getComputePipelines() const {
            return computePipelines;
        }

        [[nodiscard]] const rendergraph::ComputePipeline &getComputePipeline(const Handle handle) const {
            return computePipelines.at(handle);
        }

        ShaderProgram &getShaderProgram(const Handle handle) {
            return shaderPrograms.at(handle);
        }

        CompiledShader &getCompiledShader(const Handle handle) {
            return compiledShaders.at(handle);
        }

    private:
        Handle allocateHandle() {
            if (freeHandles.empty()) {
                return nextHandle++;
            }
            const Handle handle = *freeHandles.begin();
            freeHandles.erase(handle);
            return handle;
        }

        Handle nextHandle = 0;
        std::unordered_set<Handle> freeHandles{};

        std::unordered_map<Handle, rendergraph::RasterPipeline> rasterPipelines{};
        std::unordered_map<Handle, rendergraph::ComputePipeline> computePipelines{};

        std::unordered_map<Handle, ShaderProgram> shaderPrograms{};
        std::unordered_map<Handle, CompiledShader> compiledShaders{};
    };
}

#endif //XENGINE_PIPELINECACHEGL_HPP
