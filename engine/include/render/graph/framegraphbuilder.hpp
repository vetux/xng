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

#ifndef XENGINE_FRAMEGRAPHBUILDER_HPP
#define XENGINE_FRAMEGRAPHBUILDER_HPP

#include "render/graph/framegraphresource.hpp"
#include "render/graph/framegraphpool.hpp"
#include "render/graph/framegraph.hpp"

#include "asset/scene.hpp"

namespace xng {
    class XENGINE_EXPORT FrameGraphBuilder {
    public:
        FrameGraphBuilder(RenderTarget &backBuffer,
                          const Scene &scene,
                          const GenericMapString &properties);

        /**
         * Setup and compile a frame graph using the supplied passes.
         *
         * @param passes
         * @return
         */
        FrameGraph build(std::vector<std::reference_wrapper<FrameGraphPass>> passes,
                         const std::map<std::type_index, std::unique_ptr<std::type_index>> &passDependencies);

        /// Pass Interface   --------------------------------------

        FrameGraphResource createMeshBuffer(const ResourceHandle<Mesh> &handle);

        FrameGraphResource createTextureBuffer(const ResourceHandle<Texture> &handle);

        FrameGraphResource createShader(const ResourceHandle<Shader> &handle);

        FrameGraphResource createPipeline(FrameGraphResource shader, const RenderPipelineDesc &desc);

        FrameGraphResource createTextureBuffer(const TextureBufferDesc &attribs);

        FrameGraphResource createShaderBuffer(const ShaderBufferDesc &desc);

        FrameGraphResource createRenderTarget(const Vec2i &size, int samples);

        void write(FrameGraphResource target);

        void read(FrameGraphResource source);

        /**
         * @return The resource handle of the back buffer to render into.
         */
        FrameGraphResource getBackBuffer();

        /**
         * @return The size and sample count of the buffer returned by getBackBuffer().
         */
        std::pair<Vec2i, int> getBackBufferFormat();

        /**
         * @return The scene containing the user specified data.
         */
        const Scene &getScene();

    private:
        struct PassSetup {
            std::set<FrameGraphResource> allocations;
            std::set<FrameGraphResource> writes;
            std::set<FrameGraphResource> reads;
        };

        struct PassEntry {
            std::reference_wrapper<FrameGraphPass> pass;
            std::vector<PassEntry> childEntries;
        };

        void executeEntryRecursive(PassEntry &entry);

        PassEntry *findEntryRecursive(std::type_index needle, PassEntry &entry);

        RenderTarget &backBuffer;
        const Scene &scene;
        const GenericMapString &properties;

        FrameGraph graph;

        PassSetup currentPass;

        std::map<Uri, FrameGraphResource> uriResources;

        int resourceCounter = 1;

        GenericMapString blackboard;
    };
}
#endif //XENGINE_FRAMEGRAPHBUILDER_HPP
