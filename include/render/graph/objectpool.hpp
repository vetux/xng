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

#ifndef XENGINE_OBJECTPOOL_HPP
#define XENGINE_OBJECTPOOL_HPP

#include <map>

#include "render/platform/renderallocator.hpp"

#include "resource/uri.hpp"

#include "asset/texture.hpp"
#include "asset/shader.hpp"

#include "render/graph/gbuffer.hpp"

namespace xengine {
    /**
     * The object pool is used for storing the allocations in the FrameGraphBuilder across frames
     * for platforms which dont support explicit resource management eg. opengl.
     */
    class XENGINE_EXPORT ObjectPool {
    public:
        explicit ObjectPool(RenderAllocator &allocator);

        MeshBuffer &getMeshBuffer(const Mesh &mesh);

        MeshBuffer &getInstancedBuffer(const Mesh &mesh, const std::vector<Transform> &offsets);

        TextureBuffer &getTextureBuffer(const Texture &texture);

        ShaderProgram &getShaderProgram(const Shader &shader);

        RenderTarget &getRenderTarget(Vec2i size, int samples);

        GBuffer &getGBuffer(Vec2i size, int samples);

        /**
         * Called when all allocations of the frame have been done,
         * this deallocates unused render objects and keeps objects which were used in the current frame allocated.
         */
        void endFrame();

    private:
        RenderAllocator &allocator;

        std::map<Resource::Id, std::unique_ptr<RenderObject>> idObjects;
        std::map<Resource::Id, std::unique_ptr<RenderObject>> instancedMeshBuffers;
        std::map<std::pair<std::pair<int, int>, int>, std::unique_ptr<RenderObject>> renderTargets;
        std::map<std::pair<std::pair<int, int>, int>, std::unique_ptr<GBuffer>> gBuffers;

        std::set<Resource::Id> usedIds;
        std::set<Resource::Id> usedInstancedMeshes;
        std::set<std::pair<std::pair<int, int>, int>> usedTargets;
    };
}
#endif //XENGINE_OBJECTPOOL_HPP
