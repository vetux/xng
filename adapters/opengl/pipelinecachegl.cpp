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

#include "pipelinecachegl.hpp"

#include "glsl/shadercompilerglsl.hpp"

namespace xng::opengl {
    rendergraph::PipelineCache::Handle PipelineCacheGL::create(const rendergraph::RasterPipeline &desc) {
        auto shader = ShaderCompilerGLSL::compile(desc.shaders);
        auto program = ShaderProgram(shader);
        auto handle = allocateHandle();

        rasterPipelines.emplace(handle, desc);
        shaderPrograms.emplace(handle, std::move(program));
        compiledShaders.emplace(handle, std::move(shader.sourceCode));

        return handle;
    }

    rendergraph::PipelineCache::Handle PipelineCacheGL::create(const rendergraph::ComputePipeline &desc) {
        auto shader = ShaderCompilerGLSL::compile({desc.shader});
        auto program = ShaderProgram(shader);
        auto handle = allocateHandle();

        computePipelines.emplace(handle, desc);
        shaderPrograms.emplace(handle, std::move(program));
        compiledShaders.emplace(handle, std::move(shader.sourceCode));

        return handle;
    }

    void PipelineCacheGL::destroy(const Handle handle) {
        rasterPipelines.erase(handle);
        computePipelines.erase(handle);
        shaderPrograms.erase(handle);
        compiledShaders.erase(handle);
        freeHandles.insert(handle);
    }

    void PipelineCacheGL::clear() {
        rasterPipelines.clear();
        computePipelines.clear();
        shaderPrograms.clear();
        compiledShaders.clear();
        freeHandles.clear();
        nextHandle = 0;
    }

    void PipelineCacheGL::save(std::ostream &stream) {
    }

    void PipelineCacheGL::load(std::istream &stream) {
    }

    std::unordered_map<Shader::Stage, std::string> PipelineCacheGL::getCompiledShaderSource(Handle handle) {
        return compiledShaders.at(handle);
    }
}
