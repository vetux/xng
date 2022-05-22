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
#include <unordered_set>

#include "render/platform/renderallocator.hpp"

#include "resource/uri.hpp"

#include "asset/texture.hpp"
#include "asset/shader.hpp"

#include "render/graph/gbuffer.hpp"

namespace xengine {
    class TextureAttributesHashFunction {
    public:
        size_t operator()(const TextureBuffer::Attributes &p) const {
            size_t ret;
            hash_combine(ret, p.size.x);
            hash_combine(ret, p.size.y);
            hash_combine(ret, p.samples);
            hash_combine(ret, p.textureType);
            hash_combine(ret, p.format);
            hash_combine(ret, p.wrapping);
            hash_combine(ret, p.filterMin);
            hash_combine(ret, p.filterMag);
            hash_combine(ret, p.generateMipmap);
            hash_combine(ret, p.mipmapFilter);
            hash_combine(ret, p.fixedSampleLocations);
            return ret;
        }
    };

    /**
     * The object pool is used for storing the uri resource allocations.
     * This makes the framegraph abstraction depend on the resource abstraction which should be avoided.
     */
    class XENGINE_EXPORT ObjectPool {
    public:
        explicit ObjectPool(RenderAllocator &allocator);

        MeshBuffer &getMeshBuffer(const ResourceHandle<Mesh> &uri);

        TextureBuffer &getTextureBuffer(const ResourceHandle<Texture> &uri);

        ShaderProgram &getShaderProgram(const ResourceHandle<Shader> &uri);

        RenderTarget &getRenderTarget(Vec2i size, int samples);

        TextureBuffer &getTextureBuffer(TextureBuffer::Attributes attributes);

        /**
         * Called when all allocations of the frame have been done,
         * this deallocates unused render objects and keeps objects which were used in the current frame allocated.
         */
        void endFrame();

    private:
        RenderAllocator &allocator;

        std::map<Uri, std::unique_ptr<RenderObject>> uriObjects;
        std::map<std::pair<std::pair<int, int>, int>, std::unique_ptr<RenderObject>> renderTargets;
        std::unordered_map<TextureBuffer::Attributes,
                std::vector<std::unique_ptr<TextureBuffer>>,
                TextureAttributesHashFunction> textures;

        std::set<Uri> usedUris;
        std::set<std::pair<std::pair<int, int>, int>> usedTargets;
        std::unordered_map<TextureBuffer::Attributes, int, TextureAttributesHashFunction> usedTextures;
    };
}
#endif //XENGINE_OBJECTPOOL_HPP
