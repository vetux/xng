/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2023  Julian Zampiccoli
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

#include "rendergraphruntimeogl.hpp"

#include "contextgl.hpp"
#include "glsl/shadercompilerglsl.hpp"
#include "colorbytesize.hpp"

RenderGraphRuntimeOGL::RenderGraphRuntimeOGL() = default;

RenderGraphRuntimeOGL::~RenderGraphRuntimeOGL() = default;

void RenderGraphRuntimeOGL::setWindow(std::shared_ptr<Window> wndArg) {
    this->window = std::move(wndArg);
}

Window &RenderGraphRuntimeOGL::getWindow() {
    if (window == nullptr) {
        throw std::runtime_error("No window set");
    }
    return *window;
}

Vec2i RenderGraphRuntimeOGL::updateBackBuffer() {
    updateInternalBackBuffer();
    return backBufferColor->texture.size;
}

Vec2i RenderGraphRuntimeOGL::getBackBufferSize() {
    if (backBufferColor == nullptr) {
        throw std::runtime_error("No back buffer");
    }
    return backBufferColor->texture.size;
}

RenderGraphHandle RenderGraphRuntimeOGL::compile(const RenderGraph &graph) {
    return compileGraph(graph);
}

void RenderGraphRuntimeOGL::recompile(const RenderGraphHandle handle, const RenderGraph &graph) {
    recompileGraph(handle, graph);
}

RenderGraphStatistics RenderGraphRuntimeOGL::execute(const RenderGraphHandle graph) {
    oglDebugStartGroup("RenderGraphRuntimeOGL::execute");

    auto stats = RenderGraphStatistics();
    for (auto &buffer: contexts.at(graph).vertexBuffers) {
        stats.vertexVRamUsage += buffer.second->size;
    }
    for (auto &buffer: contexts.at(graph).indexBuffers) {
        stats.indexVRamUsage += buffer.second->size;
    }
    for (auto &tex: contexts.at(graph).textures) {
        auto numColors = (tex.second->texture.size.x
                          * tex.second->texture.size.y);
        if (tex.second->texture.isArrayTexture) {
            numColors *= tex.second->texture.arrayLayers;
        }
        stats.textureVRamUsage += numColors * getColorByteSize(tex.second->texture.format);
    }

    ContextGL context(backBufferColor, backBufferDepthStencil, contexts.at(graph), stats);
    for (auto &pass: contexts.at(graph).graph.passes) {
        oglDebugStartGroup(pass.name);
        pass.pass(context);
        oglDebugEndGroup();
    }

    presentBackBuffer();

    oglDebugEndGroup();

    return stats;
}

RenderGraphStatistics RenderGraphRuntimeOGL::execute(const std::vector<RenderGraphHandle> &graphs) {
    oglDebugStartGroup("RenderGraphRuntimeOGL::execute");

    auto stats = RenderGraphStatistics();
    for (auto graph: graphs) {
        for (auto &buffer: contexts.at(graph).vertexBuffers) {
            stats.vertexVRamUsage += buffer.second->size;
        }
        for (auto &buffer: contexts.at(graph).indexBuffers) {
            stats.indexVRamUsage += buffer.second->size;
        }
        for (auto &tex: contexts.at(graph).textures) {
            auto numColors = (tex.second->texture.size.x
                              * tex.second->texture.size.y);
            if (tex.second->texture.isArrayTexture) {
                numColors *= tex.second->texture.arrayLayers;
            }
            stats.textureVRamUsage += numColors * getColorByteSize(tex.second->texture.format);
        }

        oglDebugStartGroup("SubGraph");
        ContextGL context(backBufferColor, backBufferDepthStencil, contexts.at(graph), stats);
        for (auto &pass: contexts.at(graph).graph.passes) {
            oglDebugStartGroup(pass.name);
            pass.pass(context);
            oglDebugEndGroup();
        }
        oglDebugEndGroup();
    }
    presentBackBuffer();

    oglDebugEndGroup();

    return stats;
}

void RenderGraphRuntimeOGL::destroy(const RenderGraphHandle graph) {
    oglDebugStartGroup("RenderGraphRuntimeOGL::destroy");
    contexts.erase(graph);
    oglDebugEndGroup();
}

void RenderGraphRuntimeOGL::saveCache(const RenderGraphHandle graph, std::ostream &stream) {
    throw std::runtime_error("Not implemented");
}

void RenderGraphRuntimeOGL::loadCache(const RenderGraphHandle graph, std::istream &stream) {
    throw std::runtime_error("Not implemented");
}

void RenderGraphRuntimeOGL::updateInternalBackBuffer() {
    if (window == nullptr) {
        throw std::runtime_error("No window has been set");
    }
    oglDebugStartGroup("RenderGraphRuntimeOGL::updateInternalBackBuffer");

    if (backBuffer == nullptr) {
        backBuffer = std::make_shared<OGLFramebuffer>();
    }

    const auto fbSize = window->getFramebufferSize();
    if (backBufferColor == nullptr || backBufferColor->texture.size != fbSize) {
        RenderGraphTexture desc;
        desc.size = fbSize;
        desc.format = RGBA;
        backBufferColor = std::make_shared<OGLTexture>(desc);

        desc.format = DEPTH_STENCIL;
        backBufferDepthStencil = std::make_shared<OGLTexture>(desc);

        glBindFramebuffer(GL_FRAMEBUFFER, backBuffer->FBO);
        {
            backBuffer->attach(*backBufferColor, GL_COLOR_ATTACHMENT0, 0);
            backBuffer->attach(*backBufferDepthStencil, GL_DEPTH_STENCIL_ATTACHMENT, 0);

            glDrawBuffer(GL_COLOR_ATTACHMENT0);

            const auto clearColor = ColorRGBA::fuchsia();
            glClearColor(clearColor.r(), clearColor.g(), clearColor.b(), 0);
            glClearDepth(0);
            glClearStencil(0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        oglCheckError();
    }

    oglDebugEndGroup();
}

void RenderGraphRuntimeOGL::presentBackBuffer() const {
    oglDebugStartGroup("RenderGraphRuntimeOGL::presentScreenTexture");

    auto srcSize = backBufferColor->texture.size;
    auto dstSize = window->getFramebufferSize();

    // Skip presenting frames on resize, prevents the contents of the window from jittering when live resizing.
    if (dstSize == srcSize) {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, backBuffer->FBO);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        glBlitFramebuffer(0,
                          0,
                          srcSize.x,
                          srcSize.y,
                          0,
                          0,
                          dstSize.x,
                          dstSize.y,
                          GL_COLOR_BUFFER_BIT,
                          GL_LINEAR);

        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    }

    glFlush();

    window->swapBuffers();

    oglCheckError();
    try {
        oglCheckError();
    } catch (const std::exception &e) {
        // Because the framebuffer size can change randomly, I ignore errors that glBlitFramebuffer might throw.
    }

    oglDebugEndGroup();
}

RenderGraphHandle RenderGraphRuntimeOGL::compileGraph(const RenderGraph &graph) {
    oglDebugStartGroup("RenderGraphRuntimeOGL::compileGraph");

    const auto handle = RenderGraphHandle(graphCounter++);

    auto context = GraphResources();

    context.graph = graph;

    ShaderCompilerGLSL compiler;
    for (const auto &pair: graph.pipelineAllocation) {
        auto pip = compiler.compile(pair.second.shaders);
        context.pipelines[pair.first] = pair.second;
        context.compiledPipelines[pair.first] = pip;
        context.shaderPrograms[pair.first] = std::make_shared<OGLShaderProgram>(pip);
    }

    for (const auto &pair: graph.textureAllocation) {
        context.textures[pair.first] = std::make_shared<OGLTexture>(pair.second);
    }

    for (const auto &pair: graph.vertexBufferAllocation) {
        context.vertexBuffers[pair.first] = std::make_shared<OGLVertexBuffer>(pair.second);
    }

    for (const auto &pair: graph.indexBufferAllocation) {
        context.indexBuffers[pair.first] = std::make_shared<OGLIndexBuffer>(pair.second);
    }

    for (const auto &pair: graph.shaderBufferAllocation) {
        context.storageBuffers[pair.first] = std::make_shared<OGLShaderStorageBuffer>(pair.second);
    }

    contexts[handle] = context;

    oglDebugEndGroup();

    return handle;
}

RenderGraphHandle RenderGraphRuntimeOGL::recompileGraph(const RenderGraphHandle handle,
                                                        const RenderGraph &graph) {
    oglDebugStartGroup("RenderGraphRuntimeOGL::recompileGraph");

    auto context = GraphResources();

    context.graph = graph;

    ShaderCompilerGLSL compiler;
    for (const auto &pair: graph.pipelineAllocation) {
        auto pip = compiler.compile(pair.second.shaders);
        context.pipelines[pair.first] = pair.second;
        context.compiledPipelines[pair.first] = pip;
        context.shaderPrograms[pair.first] = std::make_shared<OGLShaderProgram>(pip);
    }

    for (const auto &pair: graph.textureAllocation) {
        context.textures[pair.first] = std::make_shared<OGLTexture>(pair.second);
    }

    for (const auto &pair: graph.vertexBufferAllocation) {
        context.vertexBuffers[pair.first] = std::make_shared<OGLVertexBuffer>(pair.second);
    }

    for (const auto &pair: graph.indexBufferAllocation) {
        context.indexBuffers[pair.first] = std::make_shared<OGLIndexBuffer>(pair.second);
    }

    for (const auto &pair: graph.shaderBufferAllocation) {
        context.storageBuffers[pair.first] = std::make_shared<OGLShaderStorageBuffer>(pair.second);
    }

    auto &oldContext = contexts.at(handle);
    for (const auto &pair: graph.inheritedResources) {
        if (oldContext.compiledPipelines.find(pair.second) != oldContext.compiledPipelines.end()) {
            context.pipelines[pair.first] = oldContext.pipelines[pair.second];
            context.compiledPipelines[pair.first] = oldContext.compiledPipelines[pair.second];
            context.shaderPrograms[pair.first] = oldContext.shaderPrograms[pair.second];
        } else if (oldContext.textures.find(pair.second) != oldContext.textures.end()) {
            context.textures[pair.first] = oldContext.textures[pair.second];
        } else if (oldContext.vertexBuffers.find(pair.second) != oldContext.vertexBuffers.end()) {
            context.vertexBuffers[pair.first] = oldContext.vertexBuffers[pair.second];
        } else if (oldContext.indexBuffers.find(pair.second) != oldContext.indexBuffers.end()) {
            context.indexBuffers[pair.first] = oldContext.indexBuffers[pair.second];
        } else if (oldContext.storageBuffers.find(pair.second) != oldContext.storageBuffers.end()) {
            context.storageBuffers[pair.first] = oldContext.storageBuffers[pair.second];
        } else {
            throw std::runtime_error("Invalid resource handle inherited by pass");
        }
    }

    contexts[handle] = context;

    oglDebugEndGroup();

    return handle;
}
