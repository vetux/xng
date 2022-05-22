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

#include "asset/shader.hpp"
#include "asset/mesh.hpp"
#include "asset/texture.hpp"
#include "asset/scene.hpp"

namespace xengine {
    /**
     * Not really a framegraph architecture currently because the graph handles only allocation of resources with an uri or render targets and textures which do not require a cpu upload.
     *
     * The render passes allocate static render objects using a passed render device, and share render objects with the blackboard which makes transient resource allocation not possible.
     *
     * This is because i could not find a efficient way to identify individual resource instances just based on their data (Would need to compare the complete data vectors of meshes and images)
     * In a real framegraph you allocate the resources transiently and minimize resource lifetime throughout a frame in eg Vulkan.
     */
    class XENGINE_EXPORT FrameGraphBuilder {
    public:
        FrameGraphBuilder(RenderTarget &backBuffer,
                          ObjectPool &pool,
                          const Scene &scene,
                          Vec2i renderResolution,
                          int renderSamples);

        FrameGraphResource createMeshBuffer(const ResourceHandle<Mesh> &handle);

        FrameGraphResource createTextureBuffer(const ResourceHandle<Texture> &handle);

        FrameGraphResource createShader(const ResourceHandle<Shader> &handle);

        FrameGraphResource createTextureBuffer(const TextureBuffer::Attributes &attribs);

        FrameGraphResource createRenderTarget(Vec2i size, int samples);

        void write(FrameGraphResource target);

        void read(FrameGraphResource source);

        FrameGraphResource getBackBuffer();

        std::pair<Vec2i, int> getBackBufferFormat();

        std::pair<Vec2i, int> getRenderFormat();

        const Scene &getScene();

        FrameGraph build(const std::vector<std::shared_ptr<RenderPass>> &passes);

    private:
        ObjectPool &pool;
        RenderTarget &backBuffer;
        const Scene &scene;
        std::vector<std::function<RenderObject &()>> resources;
        std::vector<std::set<FrameGraphResource>> passResources;
        size_t currentPass = 0;

        Vec2i renderRes;
        int renderSamples;
    };
}
#endif //XENGINE_FRAMEGRAPHBUILDER_HPP
