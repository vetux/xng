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

#ifndef XENGINE_FRAMEGRAPHPOOL_HPP
#define XENGINE_FRAMEGRAPHPOOL_HPP

#include <unordered_set>

#include "resource/resourcehandle.hpp"

#include "graphics/renderdevice.hpp"

#include "asset/texture.hpp"
#include "asset/mesh.hpp"
#include "asset/shader.hpp"

namespace xng {
    class FrameGraphPool {
    public:
        FrameGraphPool() = default;

        FrameGraphPool(RenderDevice &device);

        /**
         * Deallocates buffers which have not been used since the last call to collectGarbage.
         */
        void collectGarbage();

        VertexBuffer &getMesh(const ResourceHandle<Mesh>& handle);

        TextureBuffer &getTexture(const ResourceHandle<Texture>& handle);

        ShaderProgram &getShader(const ResourceHandle<Shader>& handle);

        TextureBuffer &createTextureBuffer(const TextureBufferDesc &desc);

        RenderTarget &createRenderTarget(const RenderTargetDesc &desc);

        RenderPipeline &getPipeline(const ResourceHandle<Shader>& shader, const RenderPipelineDesc &desc);

    private:
        struct PipelinePair {
            Uri uri;
            RenderPipelineDesc desc;

            bool operator==(const PipelinePair &other) const {
                return uri == other.uri && desc == other.desc;
            }
        };

        class PipelinePairHash {
        public:
            std::size_t operator()(const PipelinePair &k) const {
                size_t ret = 0;
                hash_combine(ret, k.uri);
                hash_combine(ret, k.desc);
                return ret;
            }
        };

        RenderDevice *device = nullptr;

        std::unordered_map<Uri, std::unique_ptr<RenderObject>> uriObjects;
        std::unordered_map<TextureBufferDesc, std::vector<std::unique_ptr<TextureBuffer>>> textures;
        std::unordered_map<RenderTargetDesc, std::vector<std::unique_ptr<RenderTarget>>> targets;
        std::unordered_map<PipelinePair, std::unique_ptr<RenderPipeline>, PipelinePairHash> pipelines;

        std::set<Uri> usedUris;
        std::unordered_map<TextureBufferDesc, int> usedTextures;
        std::unordered_map<RenderTargetDesc, int> usedTargets;
        std::unordered_set<PipelinePair, PipelinePairHash> usedPipelines;
    };
}
#endif //XENGINE_FRAMEGRAPHPOOL_HPP
