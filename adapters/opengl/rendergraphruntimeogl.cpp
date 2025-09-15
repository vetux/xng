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

RenderGraphRuntimeOGL::RenderGraphRuntimeOGL() = default;

RenderGraphRuntimeOGL::~RenderGraphRuntimeOGL() = default;

void RenderGraphRuntimeOGL::setWindow(Window &wndArg) {
    this->window = &wndArg;
}

RenderGraphRuntime::GraphHandle RenderGraphRuntimeOGL::compile(const RenderGraph &graph) {
    return compileGraph(graph);
}

void RenderGraphRuntimeOGL::recompile(const GraphHandle handle, const RenderGraph &graph) {
    recompileGraph(handle, graph);
}

void RenderGraphRuntimeOGL::execute(const GraphHandle graph) {
    oglDebugStartGroup("RenderGraphRuntimeOGL::execute");

    updateScreenTexture();
    ContextGL context(screenColorTexture, contexts.at(graph));
    for (auto &pass: contexts[graph].graph.passes) {
        oglDebugStartGroup(pass.name);
        pass.pass(context);
        oglDebugEndGroup();
    }
    presentScreenTexture();

    oglDebugEndGroup();
}

void RenderGraphRuntimeOGL::execute(const std::vector<GraphHandle> &graphs) {
    oglDebugStartGroup("RenderGraphRuntimeOGL::execute");

    updateScreenTexture();
    for (auto graph: graphs) {
        oglDebugStartGroup("SubGraph");
        ContextGL context(screenColorTexture, contexts.at(graph));
        for (auto &pass: contexts[graph].graph.passes) {
            oglDebugStartGroup(pass.name);
            pass.pass(context);
            oglDebugEndGroup();
        }
        oglDebugEndGroup();
    }
    presentScreenTexture();

    oglDebugEndGroup();
}

void RenderGraphRuntimeOGL::destroy(const GraphHandle graph) {
    oglDebugStartGroup("RenderGraphRuntimeOGL::destroy");
    contexts.erase(graph);
    oglDebugEndGroup();
}

void RenderGraphRuntimeOGL::saveCache(const GraphHandle graph, std::ostream &stream) {
    throw std::runtime_error("Not implemented");
}

void RenderGraphRuntimeOGL::loadCache(const GraphHandle graph, std::istream &stream) {
    throw std::runtime_error("Not implemented");
}

void RenderGraphRuntimeOGL::updateScreenTexture() {
    if (window == nullptr) {
        throw std::runtime_error("No window has been set");
    }
    oglDebugStartGroup("RenderGraphRuntimeOGL::updateScreenTexture");

    if (screenColorTexture == nullptr
        || screenColorTexture->handle == 0
        || screenColorTexture->texture.size != window->getFramebufferSize()) {
        RenderGraphTexture texture;
        texture.size = window->getFramebufferSize();
        screenColorTexture = std::make_shared<OGLTexture>(texture);
        screenFramebuffer = std::make_shared<OGLFramebuffer>();
        screenFramebuffer->attach(*screenColorTexture, GL_COLOR_ATTACHMENT0, 0);
        const auto clearColor = ColorRGBA::fuchsia();
        glClearTexImage(screenColorTexture->handle, 0, GL_RGBA, GL_UNSIGNED_BYTE, clearColor.data);
    }

    oglDebugEndGroup();
}

void RenderGraphRuntimeOGL::presentScreenTexture() const {
    oglDebugStartGroup("RenderGraphRuntimeOGL::presentScreenTexture");

    glBindFramebuffer(GL_READ_FRAMEBUFFER, screenFramebuffer->FBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0,
                      0,
                      screenColorTexture->texture.size.x,
                      screenColorTexture->texture.size.y,
                      0,
                      0,
                      screenColorTexture->texture.size.x,
                      screenColorTexture->texture.size.y,
                      GL_COLOR_BUFFER_BIT,
                      GL_NEAREST);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    oglCheckError();

    oglDebugEndGroup();
}

RenderGraphRuntime::GraphHandle RenderGraphRuntimeOGL::compileGraph(const RenderGraph &graph) {
    oglDebugStartGroup("RenderGraphRuntimeOGL::compileGraph");

    const auto handle = graphCounter++;

    auto context = GraphResources();

    context.graph = graph;

    ShaderCompilerGLSL compiler;
    for (const auto& pair: graph.pipelineAllocation) {
        auto pip = compiler.compile(pair.second.shaders);
        context.pipelines[pair.first] = pair.second;
        context.compiledPipelines[pair.first] = pip;
        context.shaderPrograms[pair.first] = std::make_shared<OGLShaderProgram>(pip);
    }

    for (const auto& pair : graph.textureAllocation) {
        context.textures[pair.first] = std::make_shared<OGLTexture>(pair.second);
    }

    for (const auto &pair : graph.vertexBufferAllocation) {
        context.vertexBuffers[pair.first] = std::make_shared<OGLVertexBuffer>(pair.second);
    }

    for (const auto &pair : graph.indexBufferAllocation) {
        context.indexBuffers[pair.first] = std::make_shared<OGLIndexBuffer>(pair.second);
    }

    for (const auto &pair : graph.shaderBufferAllocation) {
        context.storageBuffers[pair.first] = std::make_shared<OGLShaderStorageBuffer>(pair.second);
    }

    contexts[handle] = context;

    oglDebugEndGroup();

    return handle;
}

RenderGraphRuntime::GraphHandle RenderGraphRuntimeOGL::recompileGraph(const GraphHandle handle, const RenderGraph &graph) {
    oglDebugStartGroup("RenderGraphRuntimeOGL::recompileGraph");

    auto context = GraphResources();

    context.graph = graph;

    ShaderCompilerGLSL compiler;
    for (const auto& pair: graph.pipelineAllocation) {
        auto pip = compiler.compile(pair.second.shaders);
        context.pipelines[pair.first] = pair.second;
        context.compiledPipelines[pair.first] = pip;
        context.shaderPrograms[pair.first] = std::make_shared<OGLShaderProgram>(pip);
    }

    for (const auto& pair : graph.textureAllocation) {
        context.textures[pair.first] = std::make_shared<OGLTexture>(pair.second);
    }

    for (const auto &pair : graph.vertexBufferAllocation) {
        context.vertexBuffers[pair.first] = std::make_shared<OGLVertexBuffer>(pair.second);
    }

    for (const auto &pair : graph.indexBufferAllocation) {
        context.indexBuffers[pair.first] = std::make_shared<OGLIndexBuffer>(pair.second);
    }

    for (const auto &pair : graph.shaderBufferAllocation) {
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
