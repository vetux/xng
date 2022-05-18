/**
 *  xEngine - C++ game engine library
 *  Copyright (C) 2021  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef XENGINE_FRAMEGRAPHBUILDER_HPP
#define XENGINE_FRAMEGRAPHBUILDER_HPP

#include "render/graph/framegraphresource.hpp"
#include "render/graph/framegraph.hpp"
#include "render/graph/framegraphlayer.hpp"

#include "asset/shader.hpp"
#include "asset/mesh.hpp"
#include "asset/texture.hpp"

namespace xengine {
    class XENGINE_EXPORT FrameGraphBuilder {
    public:
        FrameGraphBuilder(RenderTarget &backBuffer,
                          ObjectPool &pool,
                          Vec2i renderResolution,
                          int renderSamples);

        FrameGraphResource createMeshBuffer(const Mesh &mesh);

        FrameGraphResource createInstancedMeshBuffer(const Mesh &mesh, const std::vector<Transform> &offsets);

        FrameGraphResource createTextureBuffer(const Texture &texture);

        FrameGraphResource createShader(const Shader &shader);

        FrameGraphResource createRenderTarget(Vec2i size, int samples);

        void write(FrameGraphResource target);

        void read(FrameGraphResource source);

        FrameGraphResource getBackBuffer();

        std::pair<Vec2i, int> getBackBufferFormat();

        std::pair<Vec2i, int> getRenderFormat();

        FrameGraph build(const std::vector<std::shared_ptr<RenderPass>> &passes);

        void addLayer(FrameGraphLayer layer) { layers.emplace_back(layer); }

        std::vector<FrameGraphLayer> getLayers() { return layers; }

    private:
        ObjectPool &pool;
        RenderTarget &backBuffer;
        std::vector<std::function<RenderObject &()>> resources;
        std::vector<std::set<FrameGraphResource>> passResources;
        size_t currentPass = 0;
        std::vector<FrameGraphLayer> layers;

        Vec2i renderRes;
        int renderSamples;
    };
}
#endif //XENGINE_FRAMEGRAPHBUILDER_HPP
