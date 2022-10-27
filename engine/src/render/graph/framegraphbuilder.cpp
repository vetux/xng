/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2022  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "render/graph/framegraphbuilder.hpp"

#include "render/graph/framegraphpass.hpp"

namespace xng {
    FrameGraphBuilder::FrameGraphBuilder(RenderTarget &backBuffer,
                                         const Scene &scene,
                                         const GenericMapString &properties)
            : backBuffer(backBuffer),
              scene(scene),
              properties(properties) {}

    FrameGraphResource FrameGraphBuilder::createMeshBuffer(const ResourceHandle<Mesh> &h) {
        auto it = uriResources.find(h.getUri());
        if (it == uriResources.end()) {
            auto ret = FrameGraphResource(resourceCounter++);
            graph.allocatedObjects[ret] = FrameGraph::PassAllocation{RenderObject::VERTEX_BUFFER,
                                                                     true,
                                                                     h.getUri()};
            currentPass.allocations.insert(ret);
            uriResources[h.getUri()] = ret;
            return ret;
        } else {
            auto ret = it->second;
            currentPass.allocations.insert(ret);
            return ret;
        }
    }

    FrameGraphResource FrameGraphBuilder::createTextureBuffer(const ResourceHandle<Texture> &h) {
        auto it = uriResources.find(h.getUri());
        if (it == uriResources.end()) {
            auto ret = FrameGraphResource(resourceCounter++);
            graph.allocatedObjects[ret] = FrameGraph::PassAllocation{RenderObject::TEXTURE_BUFFER,
                                                                     true,
                                                                     h.getUri()};
            currentPass.allocations.insert(ret);
            uriResources[h.getUri()] = ret;
            return ret;
        } else {
            auto ret = it->second;
            currentPass.allocations.insert(ret);
            return ret;
        }
    }

    FrameGraphResource FrameGraphBuilder::createShader(const ResourceHandle<Shader> &h) {
        auto it = uriResources.find(h.getUri());
        if (it == uriResources.end()) {
            auto ret = FrameGraphResource(resourceCounter++);
            graph.allocatedObjects[ret] = FrameGraph::PassAllocation{RenderObject::SHADER_PROGRAM,
                                                                     true,
                                                                     h.getUri()};
            currentPass.allocations.insert(ret);
            uriResources[h.getUri()] = ret;
            return ret;
        } else {
            auto ret = it->second;
            currentPass.allocations.insert(ret);
            return ret;
        }
    }

    FrameGraphResource FrameGraphBuilder::createPipeline(FrameGraphResource shader,
                                                         const RenderPipelineDesc& desc) {
        auto ret = FrameGraphResource(resourceCounter++);
        graph.allocatedObjects[ret] = FrameGraph::PassAllocation{RenderObject::RENDER_PIPELINE,
                                                                 false,
                                                                 std::make_pair<>(shader, desc)};
        currentPass.allocations.insert(ret);
        return ret;
    }

    FrameGraphResource FrameGraphBuilder::createTextureBuffer(const TextureBufferDesc &desc, bool persistent) {
        auto ret = FrameGraphResource(resourceCounter++);
        graph.allocatedObjects[ret] = FrameGraph::PassAllocation{RenderObject::TEXTURE_BUFFER,
                                                                 false,
                                                                 std::make_pair<>(desc, persistent)};
        currentPass.allocations.insert(ret);
        return ret;
    }

    FrameGraphResource FrameGraphBuilder::createShaderBuffer(const ShaderBufferDesc &desc) {
        auto ret = FrameGraphResource(resourceCounter++);
        graph.allocatedObjects[ret] = FrameGraph::PassAllocation{RenderObject::SHADER_BUFFER,
                                                                 false,
                                                                 desc};
        currentPass.allocations.insert(ret);
        return ret;
    }

    FrameGraphResource FrameGraphBuilder::createRenderTarget(const Vec2i &size, int samples) {
        auto ret = FrameGraphResource(resourceCounter++);
        graph.allocatedObjects[ret] = FrameGraph::PassAllocation{RenderObject::RENDER_TARGET,
                                                                 false,
                                                                 std::make_pair<>(size, samples)};
        currentPass.allocations.insert(ret);
        return ret;
    }

    void FrameGraphBuilder::write(FrameGraphResource target) {
        currentPass.writes.insert(target);
    }

    void FrameGraphBuilder::read(FrameGraphResource source) {
        currentPass.reads.insert(source);
    }

    void FrameGraphBuilder::setDependency(const std::type_index &pass) {
        currentPass.passDependency = std::make_unique<std::type_index>(pass);
    }

    FrameGraphResource FrameGraphBuilder::getBackBuffer() {
        return FrameGraphResource(0);
    }

    std::pair<Vec2i, int> FrameGraphBuilder::getBackBufferFormat() {
        return {backBuffer.getDescription().size, backBuffer.getDescription().samples};
    }

    const Scene &FrameGraphBuilder::getScene() {
        return scene;
    }

    FrameGraph::PassExecution *findPassRecursive(const std::type_index &needle, FrameGraph::PassExecution &exec) {
        if (exec.pass == needle) {
            return &exec;
        } else {
            for (auto &pass: exec.childPasses) {
                auto ptr = findPassRecursive(needle, pass);
                if (ptr != nullptr) {
                    return ptr;
                }
            }
            return nullptr;
        }
    }

    FrameGraph FrameGraphBuilder::build(const std::vector<std::reference_wrapper<FrameGraphPass>> &passes) {
        resourceCounter = 1;
        for (auto &pass: passes) {
            currentPass = {};
            pass.get().setup(*this, properties);
            passSetups[pass.get().getTypeName()] = std::move(currentPass);
        }

        // Brute force frame graph tree creation.
        while (!passSetups.empty()) {
            std::set<std::type_index> delPasses;
            for (auto &pair: passSetups) {
                FrameGraph::PassExecution exec;
                exec.pass = pair.first;
                exec.allocations = pair.second.allocations;
                exec.reads = pair.second.reads;
                exec.writes = pair.second.writes;

                if (pair.second.passDependency != nullptr) {
                    if (passSetups.find(*pair.second.passDependency) == passSetups.end()) {
                        throw std::runtime_error("No pass for the given dependency type found.");
                    }
                    for (auto &pass: graph.passExecutions) {
                        auto *ex = findPassRecursive(*pair.second.passDependency, pass);
                        if (ex != nullptr) {
                            ex->childPasses.emplace_back(exec);
                            delPasses.insert(pair.first);
                        }
                    }
                } else {
                    graph.passExecutions.emplace_back(exec);
                    delPasses.insert(pair.first);
                }
            }

            for (auto &pass: delPasses) {
                passSetups.erase(pass);
            }
        }

        return graph;
    }
}