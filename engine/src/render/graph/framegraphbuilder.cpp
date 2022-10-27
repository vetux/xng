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
                                                         const RenderPipelineDesc &desc) {
        auto ret = FrameGraphResource(resourceCounter++);
        graph.allocatedObjects[ret] = FrameGraph::PassAllocation{RenderObject::RENDER_PIPELINE,
                                                                 false,
                                                                 std::make_pair<>(shader, desc)};
        currentPass.allocations.insert(ret);
        return ret;
    }

    FrameGraphResource FrameGraphBuilder::createTextureBuffer(const TextureBufferDesc &attribs) {
        auto ret = FrameGraphResource(resourceCounter++);
        graph.allocatedObjects[ret] = FrameGraph::PassAllocation{RenderObject::TEXTURE_BUFFER,
                                                                 false,
                                                                 attribs};
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

    FrameGraphResource FrameGraphBuilder::getBackBuffer() {
        return FrameGraphResource(0);
    }

    std::pair<Vec2i, int> FrameGraphBuilder::getBackBufferFormat() {
        return {backBuffer.getDescription().size, backBuffer.getDescription().samples};
    }

    const Scene &FrameGraphBuilder::getScene() {
        return scene;
    }

    FrameGraphBuilder::PassEntry *FrameGraphBuilder::findEntryRecursive(std::type_index needle,
                                                                        FrameGraphBuilder::PassEntry &entry) {
        if (entry.pass.get().getTypeName() == needle) {
            return &entry;
        } else {
            for (auto &child: entry.childEntries) {
                auto *ptr = findEntryRecursive(needle, child);
                if (ptr) {
                    return ptr;
                }
            }
            return nullptr;
        }
    }

    void FrameGraphBuilder::executeEntryRecursive(PassEntry &entry) {
        currentPass = {};
        entry.pass.get().setup(*this, properties, blackboard);
        FrameGraph::PassExecution exec;
        exec.pass = entry.pass.get().getTypeName();
        exec.allocations = currentPass.allocations;
        exec.reads = currentPass.reads;
        exec.writes = currentPass.writes;
        graph.passExecutions.emplace_back(exec);

        for (auto &child: entry.childEntries) {
            executeEntryRecursive(child);
        }
    }

    FrameGraph FrameGraphBuilder::build(std::vector<std::reference_wrapper<FrameGraphPass>> passes,
                                        const std::map<std::type_index, std::unique_ptr<std::type_index>> &passDependencies) {
        blackboard.clear();
        graph = {};
        resourceCounter = 1;

        std::vector<PassEntry> entries;

        // Brute force pass tree creation
        while (!passes.empty()) {
            std::set<int> delPasses;
            for (auto i = 0; i < passes.size(); i++) {
                auto &pass = passes.at(i);
                auto it = passDependencies.find(pass.get().getTypeName());
                for (auto &entry: entries) {
                    if (it != passDependencies.end()) {
                        auto *ptr = findEntryRecursive(it->first, entry);
                        if (ptr) {
                            ptr->childEntries.emplace_back(PassEntry{.pass = pass});
                            delPasses.insert(i);
                            break;
                        }
                    } else {
                        entries.emplace_back(PassEntry{.pass = pass});
                        delPasses.insert(i);
                        break;
                    }
                }
            }
            for (auto it = delPasses.rbegin(); it != delPasses.rend(); it++) {
                passes.erase(passes.begin() + *it);
            }
        }

        for (auto &entry: entries) {
            executeEntryRecursive(entry);
        }

        return graph;
    }
}